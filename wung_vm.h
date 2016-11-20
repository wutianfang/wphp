#include <stdlib.h>
#include <stdio.h>
#include "wung_types.h"
#include "wung_ast.h"
#include "wung_vm_opcodes.h"
#include "wung_execute.h"
#include "wung_compile.h"
#include "wung_hash.h"

# define USE_OPLINE const wung_op *opline = EX(opline);

static int WUNG_ECHO_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    switch(op1->type_info) {
        case IS_STRING:
            printf("%s", op1->value.str->val);
            break;
        case IS_LONG:
            printf("%llu", op1->value.lval);
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
    USE_OPLINE
    wval *val_ptr = get_val_by_node(opline->op1, execute_data);
    wval *key_ptr = get_val_by_node(opline->op2, execute_data);
    wval *result = get_val_by_node(opline->result, execute_data);
    HashTable *ht = (HashTable*)malloc(sizeof(HashTable));
    wung_hash_init(ht, opline->extended_value, NULL);
    if (val_ptr!=NULL) {
        if (key_ptr!=NULL) {
            wung_hash_add_or_update_i(ht, W_STR_P(key_ptr), val_ptr, 0);
        } else {
            wung_hash_index_insert(ht, val_ptr);
        }
    }
    WVAL_ARR(result, ht);
    return 0;
}

static int WUNG_ADD_ARRAY_ELEMENT_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *val_ptr = get_val_by_node(opline->op1, execute_data);
    wval *key_ptr = get_val_by_node(opline->op2, execute_data);
    wval *result = get_val_by_node(opline->result, execute_data);

    if (key_ptr!=NULL) {
        wung_hash_add_or_update_i(W_ARR_P(result), W_STR_P(key_ptr), val_ptr, 0);
    } else {
        wung_hash_index_insert(W_ARR_P(result), val_ptr);
    }
    return 0;
}

static int WUNG_ASSIGN_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    wval *op2 = get_val_by_node(opline->op2, execute_data);
    WVAL_COPY_VALUE(op1, op2);
    return 0;
}

static int WUNG_FETCH_DIM_R_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE
    wval *op1 = get_val_by_node(opline->op1, execute_data);
    wval *key_ptr = get_val_by_node(opline->op2, execute_data);

    HashTable *ht = W_ARR_P(op1);
    Bucket * bucket;
    switch(W_TYPE_P(key_ptr)) {
        case IS_LONG:
            bucket = wung_hash_index_find(ht, W_LVAL_P(key_ptr));
            break;
        case IS_STRING:
            bucket = wung_hash_find(ht, W_STR_P(key_ptr));
            break;
    }

    wval *result = get_val_by_node(opline->result, execute_data);
    WVAL_COPY_VALUE(result, &(bucket->val));
    return 0;
}

static int WUNG_FETCH_DIM_W_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE

    printf("WUNG_FETCH_DIM_W_HANDLER\n");
    return 0;
}

static int WUNG_ASSIGN_DIM_HANDLER(wung_execute_data * execute_data ) {
    USE_OPLINE

    wval *var_ptr = get_val_by_node(opline->op1, execute_data);
    wval *dim_ptr = get_val_by_node(opline->op2, execute_data);
    wval *value_ptr = get_val_by_node((opline+1)->op1, execute_data);

    HashTable *ht = W_ARR_P(var_ptr);
    if (dim_ptr==NULL) {
        wung_hash_index_insert(ht, value_ptr);
    } else {
        Bucket * bucket;
        switch(W_TYPE_P(dim_ptr)) {
            case IS_LONG:
                wung_hash_index_add_or_update_i(ht, W_LVAL_P(dim_ptr), value_ptr, 0);
                break;
            case IS_STRING:
                wung_hash_add_or_update_i(ht, W_STR_P(dim_ptr), value_ptr, 0);
                break;
        }
    }

    return 0;
}

static int WUNG_OP_DATA_HANDLER(wung_execute_data * execute_data ) {
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

        case WUNG_FETCH_DIM_R:
        op->handler = WUNG_FETCH_DIM_R_HANDLER;
        break;

        case WUNG_FETCH_DIM_W:
        op->handler = WUNG_FETCH_DIM_W_HANDLER;
        break;

        case WUNG_ASSIGN_DIM:
        op->handler = WUNG_ASSIGN_DIM_HANDLER;
        break;

        case WUNG_OP_DATA:
        op->handler = WUNG_OP_DATA_HANDLER;
        break;
    }
}


