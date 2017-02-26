#include "wung_ast.h"
#include "wung_types.h"

size_t wung_ast_size(uint32_t children_size) {
    return sizeof(wung_ast) + (children_size-1)*(sizeof(wung_ast*));
}
wung_ast * wung_ast_alloc(uint32_t children_size) {
    size_t total_size = wung_ast_size(children_size);
	return (wung_ast*)malloc(total_size);
}

wung_ast * wung_ast_create_list(int kind, int attr) {
	size_t ast_size = sizeof(wung_ast);
    uint32_t init_children = 4;
    wung_ast * ast = wung_ast_alloc(4);
	ast->kind = kind;
	ast->attr = attr;
	ast->children = 0;
    ast->children_size = init_children;
	return ast;
}

wung_ast * wung_ast_add_list(wung_ast * ast, wung_ast * op) {
    wung_ast * ret_ast;
    if (ast->children_size <= ast->children) {
        size_t old_size = wung_ast_size(ast->children_size);
        ast->children_size += ast->children_size;
        ret_ast = wung_ast_alloc(ast->children_size);
        memcpy(ret_ast, ast, old_size);
        free(ast);
    } else {
        ret_ast = ast;
    }
	ret_ast->child[ret_ast->children++] = op;
	return ret_ast;
}
wung_ast * wung_ast_create_val(wval * val) {
	wung_ast * ast = wung_ast_alloc(sizeof(wung_ast));
	ast->kind = WUNG_AST_ZVAL;
	ast->attr = 0;
    ast->children = 0;
	WVAL_COPY_VALUE(&ast->val, val);
	return ast;
}

wung_ast * wung_ast_create_1_child(int kind, int attr, wung_ast * child1) {
	wung_ast * ast = wung_ast_alloc(sizeof(wung_ast));
	ast->kind = kind;
	ast->attr = attr;
    ast->child[0] = child1;
    ast->children = 1;
	return ast;
}

wung_ast * wung_ast_create_2_child(int kind, int attr, wung_ast * child1, wung_ast * child2) {
	wung_ast * ast = wung_ast_alloc( sizeof(wung_ast)*2 );
	ast->kind = kind;
	ast->attr = attr;
    ast->children = 2;
    ast->child[0] = child1;
    ast->child[1] = child2;

	return ast;
}
const char * wung_ast_print_kind(int kind) {
    switch(kind) {
        case WUNG_AST_ZVAL:
            return "AST_ZVAL";
        case WUNG_AST_LIST:
            return "AST_LIST";
        case WUNG_AST_ECHO:
            return "AST_ECHO";
        case WUNG_AST_ASSIGN:
            return "AST_ASSIGN";
        case WUNG_AST_BINARY_OP:
            return "AST_BINARY_OP";
        case WUNG_AST_VAR:
            return "AST_VAR";
        case WUNG_AST_ARRAY:
            return "AST_ARRAY";
        case WUNG_AST_ARRAY_ELEM:
            return "AST_ARRAY_ELEM";
        case WUNG_AST_DIM:
            return "AST_DIM";
        case WUNG_AST_GREATER:
            return "AST_GREATER";
        case WUNG_AST_GREATER_EQUAL:
            return "AST_GREATER_EQUAL";
        default:
            return "ERROR";
    }
}

void wung_ast_print(wung_ast * ast, int level) {
    return;
    if (!ast) {
        return;
    }
    int i=0;
    for (i=0; i<level; i++) {
        printf("\t");
    }
    printf("kind: %s attr: %d (%p)\n", wung_ast_print_kind(ast->kind), ast->attr, ast);
    for (i=0; i< ast->children; i++) {
        wung_ast_print(ast->child[i], level+1);
    }
}
