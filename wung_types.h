#ifndef WU_TYPES_H
#define WU_TYPES_H
#include <stdlib.h>
#include <string.h>
#include "wung.h"

typedef union _wung_value{
	wung_ulong         lval;	
	double            dval;
	wung_string      *str;
}wung_value;

typedef struct _wval{
	wung_value value;
	int type_info;
}wval;

#define IS_UNDEF					0
#define IS_NULL						1
#define IS_FALSE					2
#define IS_TRUE						3
#define IS_LONG						4
#define IS_DOUBLE					5
#define IS_STRING					6

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

#define WVAL_COPY_VALUE(z, v)   \
	do {						\
		memcpy(v, z, sizeof(wval)); \
	}while(0)

	

#endif
