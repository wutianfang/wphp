#include <stdio.h>
#include <stdlib.h>

#include "wung_hash.h"

/*
int main(int argc, char* argv[]) {
    HashTable *ht = (HashTable*)malloc(sizeof(HashTable));
    wung_hash_init(ht, 8, 0);
    wung_string *key1 = wung_string_init("abc", 3);
    wung_string *key2 = wung_string_init("def", 3);
    wval *val1,*val2;
    val1 = (wval*)malloc(sizeof(wval));
    val2 = (wval*)malloc(sizeof(wval));
    WVAL_LONG(val1, 234);
    WVAL_LONG(val2, 456);

//    wung_hash_add_or_update_i(ht, key1, val1, 0);
//    wung_hash_add_or_update_i(ht, key2, val2, 0);
//    Bucket * bucket = wung_hash_find(ht, key2);
//    printf("%d.\n", bucket->val.value.lval);
    wung_hash_index_insert(ht, val1);
    wung_hash_index_insert(ht, val2);
    Bucket * bucket = wung_hash_index_find(ht, 0);
    printf("%llu.\n", bucket->val.value.lval);
    return 0;
}
*/

// 把hashtable大小校准到2的n次方
uint32_t wung_hash_check_size(uint32_t nSize) {
    if (nSize < HT_MIN_SIZE) {
        nSize = HT_MIN_SIZE;
    }
    // 下面做的，其实就是把最高位之后的每一位置1，
    // 先把后高位后面一位置1，然后是用最高2位为3、4为置1，然后是用高4位为5、8位置1，以此类推
    // 用9举例，第一步完了是8（00001000），第二步是12（00001100），
    // 第三部是15（00001111），后几步值不变，最后+1 就是 16 （00010000）
    // -1就是为了处理8、16、32这种特殊情况
    nSize -= 1;
    nSize |= (nSize >> 1);
    nSize |= (nSize >> 2);
    nSize |= (nSize >> 4);
    nSize |= (nSize >> 8);
    nSize |= (nSize >> 16);
    return nSize + 1;
}

void wung_hash_init(HashTable * ht, int size, dtor_func_t pDestructor) {
    size = wung_hash_check_size(size);
    ht->u.flags = 0;
    ht->nNumUsed = 0;
    ht->nTableSize = size;
    ht->nNextFreeElement = 0;
    ht->nNumOfElements = 0;
    ht->nTableMask = 0;
    ht->arData = NULL;
}

void wung_hash_real_init(HashTable * ht, int packed) {
    if (packed) {
        ht->u.v.flags |= HASH_FLAG_PACKED | HASH_FLAG_INITIALIZED;
        ht->arData = (Bucket *)malloc(HT_DATA_SIZE(ht->nTableSize));
    } else {
        ht->u.v.flags |= HASH_FLAG_INITIALIZED;
        ht->nTableMask = -ht->nTableSize;
        size_t arDataLen = HT_SIZE(ht);
        Bucket * arData = malloc(arDataLen);
        HT_SET_DATA_ADDR(ht, arData);
        memset(
            &HT_HASH(ht, ht->nTableMask),
            HT_INVALID_IDX,
            HT_HASH_SIZE(ht->nTableMask)
        ); 
    }
}

void wung_hash_check_init(HashTable * ht, int packed) {
    if (ht->u.flags & HASH_FLAG_INITIALIZED) {
        return;
    }
    wung_hash_real_init(ht, packed);
}

void wung_hash_add_or_update_i(HashTable * ht,wung_string *key, wval* pData, uint32_t flags) {
    Bucket *p;
    if (!(ht->u.flags & HASH_FLAG_INITIALIZED)) {
        wung_hash_check_init(ht, 0);
    } else if(ht->u.flags & HASH_FLAG_PACKED) {
        wung_hash_packed_to_hash(ht);
    } else {
        p = wung_hash_find(ht, key);
        if (p) {
            WVAL_COPY_VALUE(&(p->val), pData);
            return; 
        }
    }

    wung_ulong h = wung_hash_string(key);
    wung_ulong idx  = ht->nNumUsed++;
    p = ht->arData + idx;
    WVAL_COPY_VALUE(&(p->val), pData);
    p->key = key;
    p->h = h;
    uint32_t nIndex = h|ht->nTableMask;
    W_NEXT(p->val) = HT_HASH(ht, nIndex);
    HT_HASH(ht, nIndex) = idx;

    // idx 是元素在bucket数组中的位置
    // nIndex 是在翻译表数组中的位置，由key的哈希值h 和 nTableMask 或计算得到
    // 翻译表中存的是元素的idx值。
    // u2.next 指向的应该是上一个元素 的 idx值
}

Bucket * wung_hash_find(const HashTable *ht, wung_string *key) {
    wung_ulong h = wung_hash_string(key);
    uint32_t idx =HT_HASH(ht, h|ht->nTableMask);
    Bucket *p;
    while (idx!=HT_INVALID_IDX) {
        p = HT_HASH_TO_BUCKET(ht, idx);
        if (WVAL_STRING_COMPARE(p->key, key)) {
            return p;
        }
        idx = p->val.u2.next;
    }
    return NULL;
}


wval * wung_hash_index_add_or_update_i(HashTable *ht, uint32_t h, wval* pData, uint32_t flags) {
    Bucket *target_ptr;
    if (!(ht->u.flags & HASH_FLAG_INITIALIZED)) {
        wung_hash_check_init(ht, h < ht->nTableSize);
        if (h < ht->nTableSize) {
            target_ptr = ht->arData + h;
            goto add_to_pack;
        }
    } else if (ht->u.flags & HASH_FLAG_PACKED) {
        target_ptr = ht->arData + h;
        if (h < ht->nNumUsed ) {
            if (W_TYPE(target_ptr->val)==IS_UNDEF) {
                wung_hash_packed_to_hash(ht);
                goto add_to_hash;
            } else {
                goto update_to_pack;
            }
        } else if (h < ht->nTableSize) {
            goto add_to_pack;
        } else if ( (h>>1) < ht->nTableSize && (ht->nTableSize>>1) < ht->nNumOfElements) {
            wung_hash_packed_grow(ht);
            goto add_to_pack;
        } else {
            wung_hash_packed_to_hash(ht);
            goto add_to_hash;
        }
    } else {
        target_ptr = wung_hash_index_find(ht, h);
        if (target_ptr==NULL) {
            goto add_to_hash;
        } else {
            goto update_to_hash;
        }
    }

update_to_pack:
    WVAL_COPY_VALUE(&(target_ptr->val), pData);
    ht->nNextFreeElement = h+1;
    return &target_ptr->val;

update_to_hash:
    WVAL_COPY_VALUE(&(target_ptr->val), pData);
    // php源码里，有语句，如果h> nNextFreeElement，则更新nNextFreeElement。
    // 但是没想明白，什么情况下， 会 h> nNextFreeElement ？
    return &target_ptr->val;

add_to_pack:
    target_ptr = ht->arData + h;
    if (h > ht->nNumUsed) {
        Bucket * temp_ptr = ht->arData + ht->nNumUsed;
        while( temp_ptr!=target_ptr) {
            WVAL_UNDEF(&temp_ptr->val);
            temp_ptr++;
        }
    }
    WVAL_COPY_VALUE(&(target_ptr->val), pData);
    target_ptr->key = NULL;
    target_ptr->h = h;
    ht->nNumUsed = h+1;
    ht->nNumOfElements++;
    if (h >= ht->nNextFreeElement) {
        ht->nNextFreeElement = h+1;
    }
    return &target_ptr->val;    

add_to_hash:
    ht->nNumOfElements++;
    if (h >= ht->nNextFreeElement) {
        ht->nNextFreeElement = h+1;
    }
    uint32_t idx = ht->nNumUsed++;
    target_ptr = ht->arData + idx;
    WVAL_COPY_VALUE(&(target_ptr->val), pData);
    target_ptr->key = NULL;
    target_ptr->h = h;
    uint32_t nIndex = h|ht->nTableMask;
    W_NEXT(target_ptr->val) = HT_HASH(ht, nIndex);
    HT_HASH(ht, nIndex) = idx;
    return &target_ptr->val;    
}


void wung_hash_index_insert(HashTable *ht, wval *pData) {
    wung_hash_index_add_or_update_i(ht, ht->nNextFreeElement++, pData, 0);
}

Bucket * wung_hash_index_find(HashTable *ht, uint32_t h) {
    if (ht->u.flags & HASH_FLAG_PACKED) {
        if (h>= ht->nNumUsed) {
            return NULL;
        }
        return ht->arData + h;
    } else {
        uint32_t idx = HT_HASH(ht, h|ht->nTableMask);
        Bucket *p;
        while (idx!=HT_INVALID_IDX) {
            p = HT_HASH_TO_BUCKET(ht, idx);
            if (p->key==NULL && p->h==h) {
                return p;
            }
            idx = W_NEXT(p->val);
        }
        return NULL;
    }
}

void wung_hash_packed_to_hash(HashTable * ht) {
    ht->u.flags &= ~HASH_FLAG_PACKED;

    Bucket* oldArData = ht->arData;
    ht->nTableMask = -ht->nTableSize;
    HT_SET_DATA_ADDR(ht, malloc(HT_SIZE(ht)));
    memcpy(ht->arData, oldArData ,  sizeof(Bucket) * ht->nNumUsed);
    free(oldArData);

    wung_hash_rehash(ht);
}

void wung_hash_rehash(HashTable * ht) {
    uint32_t i=0, newNumUsed=0, idx, nIndex;
    Bucket * target_ptr = ht->arData;
    Bucket * index_ptr = target_ptr;
    HT_HASH_RESET(ht);

    while( (i++) < ht->nNumUsed) {
        if (W_TYPE(index_ptr->val)!=IS_UNDEF) {
            do {
                if (target_ptr!=index_ptr) {
                    WVAL_COPY_VALUE(&target_ptr->val, &index_ptr->val);
                }
                target_ptr->h = index_ptr->h;
                target_ptr->key = index_ptr->key;
                nIndex = target_ptr->h | ht->nTableMask;
                W_NEXT(target_ptr->val) = HT_HASH(ht, nIndex);
                HT_HASH(ht, nIndex) = newNumUsed;

                target_ptr++; 
                newNumUsed++;
            }while(0);
        }
        index_ptr++;
    }
}

void wung_hash_packed_grow(HashTable * ht) {
    Bucket * old_bucket = ht->arData;
    size_t old_data_size = HT_DATA_SIZE(ht->nTableSize);
    ht->nTableSize += ht->nTableSize;
    ht->arData = (Bucket*)malloc(HT_DATA_SIZE(ht->nTableSize));
    memcpy(ht->arData, old_bucket, old_data_size);
    free(old_bucket);
}
