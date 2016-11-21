#ifndef WUNG_HASH_H
#define WUNG_HASH_H

#include "wung.h"
#include "wung_types.h"


#define HASH_FLAG_INITIALIZED      (1<<0)
#define HASH_FLAG_PACKED           (1<<1)

#define HT_INVALID_IDX ((uint32_t) -1)
#define HT_MIN_MASK ((uint32_t) -2)
#define HT_MIN_SIZE 8

#define HT_HASH_SIZE(nTableMask) \
    (uint32_t)(-(int32_t)(nTableMask)) * sizeof(uint32_t)
    //(((size_t)(uint32_t)-(int32_t)(nTableMask)) * sizeof(uint32_t))
#define HT_DATA_SIZE(nTableSize) \
    (nTableSize) * sizeof(Bucket)
#define HT_SIZE(ht) \
    HT_HASH_SIZE(ht->nTableMask) + HT_DATA_SIZE(ht->nTableSize)
#define HT_SET_DATA_ADDR(ht, ptr) do { \
    ht->arData = (Bucket*)(((char*)(ptr)) + HT_HASH_SIZE(ht->nTableMask)); \
}while(0);
#define HT_HASH_RESET(ht) \
    memset(&(HT_HASH(ht, ht->nTableMask)), HT_INVALID_IDX, HT_HASH_SIZE(ht->nTableMask));

#define HT_HASH_EX(data, idx)  ((uint32_t*)(data))[(int32_t)(idx)]
#define HT_HASH(ht, idx)  HT_HASH_EX((ht)->arData, idx)

# define HT_HASH_TO_BUCKET_EX(data, idx) ((data)+(idx))
# define HT_HASH_TO_BUCKET(ht, idx) ((ht)->arData+(idx))
    

// 只简单给ht个属性赋初值，nTableSize = 0;
// 虽然有设置 nTableSize 值，但并没有分配内存
// flag 没有 HASH_FLAG_PACKED 和 HASH_FLAG_INITIALIZED
// 原型是 _hash_init
void wung_hash_init(HashTable * ht, int size, dtor_func_t pDestructor);

// 根据真正给arData申请空间，空间大小由HT_SIZE(ht) 宏得到
// 给 flag 添加 HASH_FLAG_INITIALIZED，根据 packed参数设置 HASH_FLAG_PACKED
// 原型是 _zend_hash_real_init_ex
void wung_hash_real_init(HashTable * ht, int packed);

// 一般在各种插入、更新函数最开始调用，内部简单调用 wung_hash_real_init
// 原型zend_hash_check_init(ht, packed)
void wung_hash_check_init(HashTable * ht, int packed);

// 根据是否是packed，有两套整改接口

// 给classes数组添加或更新元素
// 原型是 _zend_hash_add_or_update_i
// 如果 flags里包含 HASH_ADD_NEW，不管已存在的key，直接插入。
void wung_hash_add_or_update_i(HashTable * ht,wung_string *key, wval* pData, uint32_t flags);

// 给packed数组添加或更新新元素
// 原型是 _zend_hash_index_add_or_update_i
// 如果 flags里包含 HASH_ADD_NEW，不管已存在的key，直接插入。
// 遇到以下情况，packed 转 classess
// * h 过大
// * 添加的新key不能让整个key保持递增  
// * 根本没有HASH_FLAG_PACKED 标志位
wval * wung_hash_index_add_or_update_i(HashTable *ht, uint32_t h, wval* pData, uint32_t flags);
void wung_hash_index_insert(HashTable *ht, wval *pData);

// 两种类型hashtable相互转化
void wung_hash_to_packed(HashTable * ht);
void wung_hash_packed_to_hash(HashTable * ht);

// 内存重新分配
void wung_hash_do_resize(HashTable * ht);
void wung_hash_packed_grow(HashTable * ht);
void wung_hash_rehash(HashTable * ht);

// 两种类型的查找函数
// 原型 zend_hash_index_find
Bucket * wung_hash_index_find(HashTable *ht, uint32_t h);
// 原型 zend_hash_find
Bucket * wung_hash_find(const HashTable *ht, wung_string *key);

// 两种删除
void wung_hash_del(HashTable *ht, wung_string *key);
void wung_hash_index_del(HashTable *ht, uint32_t key);

#endif
