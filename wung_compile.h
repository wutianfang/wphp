#ifndef WUNG_COMPILE_H
#define WUNG_COMPILE_H
#include "wung.h"

typedef uint16_t wung_ast_kind;
typedef uint16_t wung_ast_attr;

typedef struct _wung_ast {
	wung_ast_kind kind;
	wung_ast_attr attr;
	uint32_t lineno;
	wval val;
	wung_ast *child[1];
}wung_ast

#endif
