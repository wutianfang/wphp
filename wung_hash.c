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
    ht->nInternalPointer = 0;
    ht->nNextFreeElement = 0;
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
        HT_SET_DATA_ADDR(ht, malloc(HT_SIZE(ht)));
        memset(&HT_HASH(ht, ht->nTableMask), HT_INVALID_IDX, HT_HASH_SIZE(ht->nTableMask)); 
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
    if (ht->u.flags & HASH_FLAG_INITIALIZED) {
        p = wung_hash_find(ht, key);
        if (p) {
            WVAL_COPY_VALUE(&(p->val), pData);
            return; 
        }
    } else {
        wung_hash_check_init(ht, 0);
    }

    wung_ulong h = wung_hash_string(key);
    wung_ulong idx  = ht->nNumUsed++;
    p = ht->arData + idx;
    WVAL_COPY_VALUE(&(p->val), pData);
    p->key = key;
    p->h = h;
    uint32_t nIndex = h|ht->nTableMask;
    pData->u2.next = HT_HASH(ht, nIndex);
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

void wung_hash_index_insert(HashTable *ht, wval *pData) {
    Bucket *p;
    if (!(ht->u.flags & HASH_FLAG_INITIALIZED)) {
        wung_hash_check_init(ht, 1);
    }
    uint32_t idx = ht->nNumUsed++;
    p = ht->arData + idx;
    WVAL_COPY_VALUE(&(p->val), pData);
}

Bucket * wung_hash_index_find(HashTable *ht, uint32_t h) {
    if (h>= ht->nNumUsed) {
        return NULL;
    }
    return ht->arData + h;
}


