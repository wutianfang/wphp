#ifndef WUNG_HASH_H
#define WUNG_HASH_H

#include "wung.h"
#include "wung_types.h"
#include "wung_types.h"

typedef struct _Bucket {
    wval val;
    int h;
    wung_string * key;
}Bucket;

typedef struct _HashTable {
    union {
        struct {
            char flags;
            char nApplyCount;
            char nIteratorsCount;
            char reserve;
        } v;
        uint32_t flags;
    }u;
    Bucket * arData;
    uint32_t nNumUsed; //已使用的大小
    uint32_t nTableSize; // 容量，nNumUsed >= nTableSize，需重新申请内存 
    uint32_t nInternalPointer; // 下一个指针位置,迭代器使用
    uint32_t nNextFreeElement; // 新元素指针
    uint32_t nTableMask; //当前掩码
}HashTable;

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
void wung_hash_index_add_or_update_i(HashTable *ht, uint32_t h, wval* pData, uint32_t flags);

// 两种类型hashtable相互转化
void wung_hash_to_packed(HashTable * ht);
void wung_hash_packed_to_hash(HashTable * ht);

// 两种类型的查找函数
// 原型 zend_hash_index_find
wval * wung_hash_index_find(HashTable *ht, uint32_t h);
// 原型 zend_hash_find
wval * wung_hash_find(const HashTable *ht, wung_string *key);

// 两种删除
void zend_hash_del(HashTable *ht, wung_string *key);
void zend_hash_index_del(HashTable *ht, uint32_t key);

#endif
