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
	WVAL_COPY_VALUE(&ast->val, val);
	return ast;
}

wung_ast * wung_ast_create_1_child(int kind, int attr, wung_ast * child1) {
	wung_ast * ast = wung_ast_alloc(sizeof(wung_ast));
	ast->kind = kind;
	ast->attr = attr;
	ast->child[0] = child1;
	return ast;
}

wung_ast * wung_ast_create_2_child(int kind, int attr, wung_ast * child1, wung_ast * child2) {
	wung_ast * ast = wung_ast_alloc( sizeof(wung_ast)*2 );
	ast->kind = kind;
	ast->attr = attr;
	ast->child[0] = child1;
	ast->child[1] = child1;
	return ast;
}
