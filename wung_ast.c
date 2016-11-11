#include "wung_ast.h"
#include "wung_types.h"

wung_ast * wung_ast_alloc(size_t size) {
	return (wung_ast*)malloc(size);
}

wung_ast * wung_ast_create_list(int init_children, int kind, int attr) {
	size_t ast_size = sizeof(wung_ast);
	wung_ast * ast = wung_ast_alloc(ast_size + (init_children-1)*ast_size);
	ast->kind = kind;
	ast->attr = attr;
	ast->children = 0;
	return ast;
}

wung_ast * wung_ast_add_list(wung_ast * ast, wung_ast * op) {
	ast->child[ast->children++] = op;
	return ast;
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
char * wung_ast_print_kind(int kind) {
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
        default:
            return "ERROR";
    }
}

void wung_ast_print(wung_ast * ast, int level) {
    if (!ast) {
        return;
    }
    for (int i=0; i<level; i++) {
        printf("\t");
    }
    printf("kind: %s attr: %d (%p)\n", wung_ast_print_kind(ast->kind), ast->attr, ast);
    for (int i=0; i< ast->children; i++) {
        wung_ast_print(ast->child[i], level+1);
    }
}
