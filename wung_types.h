#ifndef WU_TYPES_H
#define WU_TYPES_H
#include <stdlib.h>
#include <string.h>
#include "wung.h"

typedef struct _HashTable HashTable;

typedef union _wung_value{
	wung_ulong      lval;	
	double          dval;
	wung_string     *str;
    HashTable      *arr;
}wung_value;

typedef struct _wval{
	wung_value value;
    union {
        struct {
            wung_uchar type;
            wung_uchar type_flags;
            wung_uchar const_flags;
            wung_uchar reserved;
        }v;
	    uint32_t type_info;
    }u1;
    union {
        uint32_t next;
        uint32_t lineno;
        uint32_t num_args;
    }u2;
}wval;

typedef void (*dtor_func_t)(wval *pDest);

typedef struct _Bucket {
    wval val;
    int h;
    wung_string * key;
}Bucket;

struct _HashTable {
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
    uint32_t nNumUsed; // 下一个可用的槽位指针
    uint32_t nNumOfElements; // arData中真正存储的数据量，小余等于nNumUsed的值
    uint32_t nNextFreeElement; // 下一个可用的自增key
    uint32_t nTableSize; // 真正的表容量
    uint32_t nTableMask; //当前掩码
};

typedef struct _wung_execute_data wung_execute_data;


#define IS_UNDEF					0
#define IS_NULL						1
#define IS_FALSE					2
#define IS_TRUE						3
#define IS_LONG						4
#define IS_DOUBLE					5
#define IS_STRING					6
#define IS_ARRAY					7

#define WVAL_UNDEF(wval) do{ \
    W_TYPE_INFO_P(wval) = IS_UNDEF;\
}while(0)
	
#define WVAL_LONG(wval, l) do{ \
    W_TYPE_INFO_P(wval) = IS_LONG;\
	(*wval).value.lval = l; \
}while(0)

#define WVAL_STRING(wval, string, len) do{ \
    wung_string * __s = wung_string_init((char*)(string), (len)); \
    (wval)->value.str = __s; \
    W_TYPE_INFO_P(wval)= IS_STRING;\
}while(0)

#define WVAL_ARR(wval, ht) do {\
    (wval)->value.arr = ht;\
    W_TYPE_INFO_P(wval)= IS_ARRAY;\
}while(0)

#define WVAL_BOOL(wval, boolean) do{\
    W_TYPE_INFO_P(wval)= (boolean==IS_TRUE) ? IS_TRUE : IS_FALSE;\
}while(0)

#define WVAL_STRING_COMPARE(str1, str2) \
    ((str1==str2) || (str1->h==str2->h && str1->len==str2->len && memcmp(str1->val, str2->val, str1->len)==0))

#define WVAL_COPY_VALUE(z, v)   \
	do {						\
		memcpy(z, v, sizeof(wval)); \
	}while(0)

#define W_TYPE(zval) (zval).u1.v.type
#define W_TYPE_P(zval_p)  W_TYPE(*(zval_p))

#define W_TYPE_INFO(wval) (wval).u1.type_info
#define W_TYPE_INFO_P(wval) W_TYPE_INFO(*(wval))

#define W_NEXT(zval) (zval).u2.next
#define W_NEXT_P(zval_p)  W_NEXT_P(*(zval_p))

#define W_LVAL(zval) (zval).value.lval
#define W_LVAL_P(zval_p)  W_LVAL(*(zval_p))

#define W_ARR(zval)  (zval).value.arr
#define W_ARR_P(zval_p)  W_ARR(*(zval_p))

#define W_STR(zval) (zval).value.str
#define W_STR_P(zval_p)  W_STR(*(zval_p))


#endif
