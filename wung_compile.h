#ifndef WUNG_COMPILE_H
#define WUNG_COMPILE_H
#include "wung.h"
#include "wung_types.h"
#include "wung_ast.h"
#include "wung_vm_opcodes.h"

typedef union _parse_ele {
	wung_ast * ast;
	wung_ulong num;
}parse_ele;


#define IS_CONST    (1<<0)
#define IS_TMP_VAR  (1<<1)
#define IS_VAR      (1<<2)
#define IS_UNUSED   (1<<3)  /* Unused variable */
#define IS_CV       (1<<4)  /* Compiled variable */

typedef struct _wnode {
    char op_type;
    char flag;
    union {
        int constant;
        int var;
        int num;
        int opline_num;
    }u;
} wnode;

typedef struct _wung_op {
    wnode *op1;
    wnode *op2;
    wnode *result;
    char opcode;
    //const *handler;
    int (*handler)(wung_execute_data * execute_data);
    int lineno;
} wung_op;

#define INITIAL_OP_ARRAY_SIZE 64

typedef struct _wung_op_array {
    int opcode_size; // opcode 数组长度
    int last; // opcode 数组指针
    wung_op *opcodes; // opcode 数组

    int last_var; //  变量数组指针
    int var_size;
    wung_string **vars; // 变量字符串数组

    int literal_size; 
    int last_literal;
    wval *literals; //常量数组和指针

    // *_size系列变量，在php7中都被保存在CG(context)中，并直接放在op_array中

} wung_op_array;

wung_op_array * compile_string(char *);

void wung_init_op_array(wung_op_array * op_array);
void wung_compile_expr(wnode * node, wung_ast * ast);
void wung_compile_var(wnode * node, wung_ast * ast);
void wung_compile_top_stmt(wung_ast * ast);
void wung_compile_stmt(wung_ast * ast);
void wung_compile_echo(wung_ast * ast);
void wung_compile_assign(wung_ast * ast);
wung_op * wung_emit_op(wnode * result, char opcode, wnode *op1, wnode * op2);
void pass_two(wung_op_array * op_array);
void wung_print_opline(wung_op * opline);

#endif
