#include <stdlib.h>
#include <stdio.h>
#include "wung_types.h"
#include "wung_ast.h"
#include "wung_vm_opcodes.h"
#include "wung_execute.h"
#include "wung_compile.h"

# define USE_OPLINE const wung_op *opline = EX(opline);

static int WUNG_ECHO_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    switch(op1->type_info) {
        case IS_STRING:
            printf("%s", op1->value.str->val);
            break;
        case IS_LONG:
            printf("%d", op1->value.lval);
            break;
    }
    return 0;
}

static int WUNG_ADD_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    wval *op2 = get_val_by_node(opline->op2, execute_data);
    wval *result = get_val_by_node(opline->result, execute_data);

    int val1 = convert2long(op1);
    int val2 = convert2long(op2);
    WVAL_LONG(result, val1 + val2);
    return 0;
}

static int WUNG_SUB_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    wval *op2 = get_val_by_node(opline->op2, execute_data);
    wval *result = get_val_by_node(opline->result, execute_data);

    int val1 = convert2long(op1);
    int val2 = convert2long(op2);
    WVAL_LONG(result, val1 - val2);
    return 0;
}
static int WUNG_DIV_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    wval *op2 = get_val_by_node(opline->op2, execute_data);
    wval *result = get_val_by_node(opline->result, execute_data);

    int val1 = convert2long(op1);
    int val2 = convert2long(op2);
    WVAL_LONG(result, val1 / val2);
    return 0;
}
static int WUNG_MUL_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    wval *op2 = get_val_by_node(opline->op2, execute_data);
    wval *result = get_val_by_node(opline->result, execute_data);

    int val1 = convert2long(op1);
    int val2 = convert2long(op2);
    WVAL_LONG(result, val1 * val2);
    return 0;
}

static int WUNG_CONCAT_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    wval *op2 = get_val_by_node(opline->op2, execute_data);
    wval *result = get_val_by_node(opline->result, execute_data);

    wung_string *str1 = convert2str(op1);
    wung_string *str2 = convert2str(op2);
    WVAL_STRING(result, str1->val, str1->len + str2->len - 1);
    strcpy(
        result->value.str->val + str1->len ,
        str2->val
    );
    return 0;
}

static int WUNG_INIT_ARRAY_HANDLER(wung_execute_data * execute_data ) {
    printf("WUNG_INIT_ARRAY_HANDLER\n");
    return 0;
}

static int WUNG_ADD_ARRAY_ELEMENT_HANDLER(wung_execute_data * execute_data ) {
    printf("WUNG_ADD_ARRAY_ELEMENT_HANDLER\n");
    return 0;
}

static int WUNG_ASSIGN_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    wval *op2 = get_val_by_node(opline->op2, execute_data);
    WVAL_COPY_VALUE(op1, op2);
    return 0;
}
static void wung_vm_set_opcode_handler(wung_op* op) {
    switch (op->opcode) {
        case WUNG_ECHO: 
        op->handler = WUNG_ECHO_HANDLER;
        break;

        case WUNG_ADD:
        op->handler = WUNG_ADD_HANDLER;
        break;

        case WUNG_SUB:
        op->handler = WUNG_SUB_HANDLER;
        break;

        case WUNG_MUL:
        op->handler = WUNG_MUL_HANDLER;
        break;

        case WUNG_DIV:
        op->handler = WUNG_DIV_HANDLER;
        break;

        case WUNG_ASSIGN:
        op->handler = WUNG_ASSIGN_HANDLER;
        break;

        case WUNG_CONCAT:
        op->handler = WUNG_CONCAT_HANDLER;
        break;

        case WUNG_INIT_ARRAY:
        op->handler = WUNG_INIT_ARRAY_HANDLER;
        break;

        case WUNG_ADD_ARRAY_ELEMENT:
        op->handler = WUNG_ADD_ARRAY_ELEMENT_HANDLER;
        break;
    }
}


