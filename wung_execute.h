#ifndef WUNG_EXECUTE_H
#define WUNG_EXECUTE_H

#include "wung.h"
#include "wung_types.h"
#include "wung_hash.h"
#include "wung_compile.h"
#include "wung_ast.h"
#include "wung_vm_opcodes.h"
#include "wung_execute.h"


struct _wung_execute_data{
    wung_op * opline;
    wval * This;
    wval * return_value;
    struct _wung_execute_data *prev_execute_data;
    wval * vars;
    wval * temp_vars;
    wung_op_array * func;
};

#define EX(element)  ((execute_data)->element)
#define WUNG_NEXT_OPCODE EX(opline) = EX(opline)+1;

int wung_execute(wung_op_array * op_array);
void init_execute_data(wung_execute_data * execute_data, wung_op_array *op_array);
wval * get_val_by_node(wnode * node, wung_execute_data * execute_data);

wung_string * convert2str(wval * op);
int convert2long(wval * op);
wung_string * long2str(int num);
int compare_function(wval *op1, wval *op2);



#endif

