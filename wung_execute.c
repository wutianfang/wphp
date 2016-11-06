#include <stdio.h>
#include <stdlib.h>
#include "wung_execute.h"

void wung_execute_ex(wung_execute_data * execute_data) {
    int i=0,max = 10;

    while(1) {
        if (EX(opline)->handler==0) {
            break;
        }
        (EX(opline)->handler)(execute_data);
        //wung_print_opline(EX(opline));
        WUNG_NEXT_OPCODE;
        if (i++>=max) {
           break;
        }
    }
}

int wung_execute(wung_op_array * op_array) {
    wung_execute_data execute_data;
    init_execute_data(&execute_data, op_array);
    wung_execute_ex(&execute_data);
    return 0;
}

void init_execute_data(wung_execute_data * execute_data, wung_op_array *op_array) {
    execute_data->opline = op_array->opcodes;
    execute_data->vars = malloc(sizeof(wval)*op_array->last_var);
    execute_data->temp_vars = malloc(sizeof(wval)*op_array->T);
    execute_data->func = op_array;
}

wval * get_val_by_node(wnode * node, wung_execute_data * execute_data) {
    switch(node->op_type) {
        case IS_TMP_VAR:
        return execute_data->temp_vars + node->u.var;

        case IS_CV:
        return execute_data->vars + node->u.var;

        case IS_CONST:
        return execute_data->func->literals + node->u.var;

        default:
        printf("get_val_by_node error!\n");
    }
    wval * ret;
    return ret;
}


