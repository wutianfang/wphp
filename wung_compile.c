#include "wung.h"
#include "wung_types.h"
#include "wung_globals.h"
#include "wung_compile.h"

wung_compiler_globals compiler_globals;

void wung_compile_top_stmt(wung_ast * ast) {
    if (ast->kind == WUNG_AST_LIST) {
        for (int i=0; ast->children; i++) {
            wung_compile_top_stmt(ast->child[i]);
        }
    }
    wung_compile_stmt(ast);

}

void wung_compile_stmt(wung_ast * ast) {
    /*
    switch(ast->kind) {
    case WUNG_AST_ECHO:
        break;
        defaut:
        printf("asdf\n");
    }
    */
}

void wung_compile_echo(wung_ast * ast) {

}

void wung_emit_op(wnode * result, char opcode, wnode *op1, wnode * op2) {

}
