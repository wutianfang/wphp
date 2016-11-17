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
	uint32_t type_info;
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
    uint32_t nNumUsed; //已使用的大小
    uint32_t nTableSize; // 容量，nNumUsed >= nTableSize，需重新申请内存 
    uint32_t nInternalPointer; // 下一个指针位置,迭代器使用
    uint32_t nNextFreeElement; // 新元素指针
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
	(*wval).type_info = IS_UNDEF; \
}while(0)
	
#define WVAL_LONG(wval, l) do{ \
	(*wval).type_info = IS_LONG; \
	(*wval).value.lval = l; \
}while(0)

#define WVAL_STRING(wval, string, len) do{ \
    wung_string * __s = wung_string_init((char*)(string), (len)); \
    (wval)->value.str = __s; \
	(wval)->type_info = IS_STRING; \
}while(0)

#define WVAL_ARR(wval, ht) do {\
    (wval)->value.arr = ht;\
    (wval)->type_info = IS_ARRAY;\
}while(0)

#define WVAL_STRING_COMPARE(str1, str2) \
    ((str1==str2) || (str1->h==str2->h && str1->len==str2->len && memcmp(str1->val, str2->val, str1->len)==0))

#define WVAL_COPY_VALUE(z, v)   \
	do {						\
		memcpy(z, v, sizeof(wval)); \
	}while(0)
#define W_LVAL(zval) (zval).value.lval
#define W_LVAL_P(zval_p)  W_LVAL(*(zval_p))

#define W_ARR(zval)  (zval).value.arr
#define W_ARR_P(zval_p)  W_ARR(*(zval_p))


#endif
