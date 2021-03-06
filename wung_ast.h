#ifndef WUNG_AST_H
#define WUNG_AST_H

#include <stdlib.h>
#include <stdio.h>
#include "wung_types.h"
typedef struct _wung_ast wung_ast;
struct _wung_ast {
	int kind;
	int attr;
	wval val;
	int children;
    int children_size;
	wung_ast *child[1];
};

#define WUNG_AST_SPECIAL_SHIFT 6
#define WUNG_AST_LIST_SHIFT 7
#define WUNG_AST_NUM_CHILDREN_SHIFT 8

enum _ast_kind {
    WUNG_AST_ZVAL = 1 << WUNG_AST_SPECIAL_SHIFT, // 64
    WUNG_AST_LIST = 1 << WUNG_AST_LIST_SHIFT,
    WUNG_AST_ARRAY,
    WUNG_AST_ECHO = 1 << WUNG_AST_NUM_CHILDREN_SHIFT,
    WUNG_AST_VAR, // 257
    WUNG_AST_BINARY_OP = 2 << WUNG_AST_NUM_CHILDREN_SHIFT,
    WUNG_AST_GREATER,
    WUNG_AST_GREATER_EQUAL,
    WUNG_AST_ASSIGN,
    WUNG_AST_ARRAY_ELEM,
    WUNG_AST_DIM,
};

wung_ast *wung_ast_create_list(int kind, int attr);
wung_ast *wung_ast_add_list(wung_ast * ast, wung_ast * op);
wung_ast *wung_ast_create_val(wval * val);
wung_ast *wung_ast_create_1_child(int kind, int attr, wung_ast * child1);
wung_ast *wung_ast_create_2_child(int kind, int attr, wung_ast * child1, wung_ast * child2);
void wung_ast_print(wung_ast * ast, int level);
const char * wung_ast_print_kind(int kind);
#endif
