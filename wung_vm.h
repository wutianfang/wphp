#include <stdlib.h>
#include <stdio.h>
#include "wung_types.h"
#include "wung_ast.h"
#include "wung_vm_opcodes.h"
#include "wung_execute.h"
#include "wung_compile.h"

static int WUNG_ECHO_HANDLER(wung_execute_data * execute_data ) {
    printf("WUNG_ECHO_HANDLER\n");
    return 0;
}

static int WUNG_ADD_HANDLER(wung_execute_data * execute_data ) {
    printf("WUNG_ADD_HANDLER\n");
    return 0;
}

static int WUNG_SUB_HANDLER(wung_execute_data * execute_data ) {
    printf("WUNG_SUB_HANDLER\n");
    return 0;
}
static int WUNG_DIV_HANDLER(wung_execute_data * execute_data ) {
    printf("WUNG_DIV_HANDLER\n");
    return 0;
}
static int WUNG_MUL_HANDLER(wung_execute_data * execute_data ) {
    printf("WUNG_MUL_HANDLER\n");
    return 0;
}
static int WUNG_ASSIGN_HANDLER(wung_execute_data * execute_data ) {
    printf("WUNG_ASSIGN_HANDLER\n");
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
    }
}


