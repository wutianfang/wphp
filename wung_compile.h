#ifndef WUNG_COMPILE_H
#define WUNG_COMPILE_H
#include "wung.h"
#include "wung_types.h"
#include "wung_ast.h"

typedef union _parse_ele {
	wung_ast * ast;
	wung_ulong num;
}parse_ele;

/*
typedef union _wnode {
	char op_type;
	char flag;
	wval constant;
}wnode;
*/

typedef struct _wnode {
    char op_type;
    char flag;
    union {
        wval constant;
        int var;
        int num;
        int opline_num;
    }u;
} wnode;

typedef struct _wung_op {
    wnode op1;
    wnode op2;
    wnode result;
    char opcode;
    const void*handler;
    int lineno;
} wung_op;

typedef struct _wung_op_array {
    int last;
    wung_op *opcodes;
} wung_op_array;

int compile_string(char *);

void wung_compile_top_stmt(wung_ast * ast);
void wung_compile_stmt(wung_ast * ast);
void wung_compile_echo(wung_ast * ast);
void wung_compile_echo(wung_ast * ast);
void wung_emit_op(wnode * result, char opcode, wnode *op1, wnode * op2);

#endif
