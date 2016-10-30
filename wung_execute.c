#include <stdio.h>
#include <stdlib.h>
#include "wung_execute.h"

void wung_execute_ex(wung_execute_data * execute_data) {
    int i=0,max = 10;

    while(1) {
        if (EX(opline)->handler==0) {
            break;
        }
        //(EX(opline)->handler)(execute_data);
        wung_print_opline(EX(opline));
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
}



