#ifndef WUNG_EXECUTE_H
#define WUNG_EXECUTE_H

#include "wung.h"
#include "wung_types.h"
#include "wung_compile.h"
#include "wung_ast.h"
#include "wung_vm_opcodes.h"
#include "wung_execute.h"


struct _wung_execute_data{
    wung_op * opline;
    wval * This;
    wval * return_value;
    struct _wung_execute_data *prev_execute_data;
};

#define EX(element)  ((execute_data)->element)
#define WUNG_NEXT_OPCODE EX(opline) = EX(opline)+1;

int wung_execute(wung_op_array * op_array);
void init_execute_data(wung_execute_data * execute_data, wung_op_array *op_array);

#endif

