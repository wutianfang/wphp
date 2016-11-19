#include "wung.h"
#include "wung_types.h"
#include "wung_globals.h"
#include "wung_compile.h"
#include "wung_ast.h"
#include "wung_vm.h"

wung_compiler_globals compiler_globals;

void wung_init_op_array(wung_op_array * op_array) {
    op_array->last = 0;
    op_array->opcode_size = INITIAL_OP_ARRAY_SIZE;
    op_array->opcodes = (wung_op*)malloc(sizeof(wung_op)*INITIAL_OP_ARRAY_SIZE);

    op_array->vars = NULL;
    op_array->last_var =0;
    op_array->var_size = 0;

    op_array->literal_size = 0;
    op_array->literals = NULL;
    op_array->last_literal = 0;

    op_array->T = 0;
}

int wung_add_literal(wung_op_array * op_array, wval * val) {
    int i = op_array->last_literal++;
    if (op_array->last_literal >= op_array->literal_size) {
        op_array->literal_size += 16;
        op_array->literals = (wval*)realloc(
            op_array->literals, 
            op_array->literal_size * sizeof(wval)
        );
    }
    WVAL_COPY_VALUE(op_array->literals+i, val);
    return i;
}

void wung_compile_top_stmt(wung_ast * ast) {
    if (ast->kind == WUNG_AST_LIST) {
        for (int i=0; i<ast->children; i++) {
            wung_compile_top_stmt(ast->child[i]);
        }
        return;
    }
    wung_compile_stmt(ast);
}

void wung_compile_stmt(wung_ast * ast) {
    switch(ast->kind) {
    case WUNG_AST_ECHO:
        wung_compile_echo(ast); 
        break;
    case WUNG_AST_ASSIGN:
        wung_compile_assign(ast);
        break;
    default:
        printf("error ast on compile stmt\n");
    }
}

void wung_compile_echo(wung_ast * ast) {
    wnode * node = (wnode *) malloc(sizeof(wnode));
    wung_compile_expr(node, ast->child[0]);
    wung_op * opline = wung_emit_op(NULL, WUNG_ECHO, node, NULL); 
}

void wung_compile_assign(wung_ast * ast) {
    wung_ast * right_ast = ast->child[1];
    wung_ast * left_ast = ast->child[0];

    wnode * right_node = (wnode *) malloc(sizeof(wnode));
    wnode * left_node = (wnode *) malloc(sizeof(wnode));
    wnode * offset_node;

    wung_compile_expr(right_node, right_ast);
    switch(left_ast->kind) {
        case WUNG_AST_VAR:
            wung_compile_var(left_node, left_ast);
            wung_emit_op(NULL, WUNG_ASSIGN, left_node, right_node);
            break;

        case WUNG_AST_DIM:
            offset_node = (wnode*) malloc(sizeof(wnode));

            wung_compile_var(left_node, left_ast->child[0]);
            wung_compile_expr(offset_node, left_ast->child[1]);
            wung_emit_op(NULL, WUNG_ASSIGN_DIM, left_node, offset_node);

            wung_emit_op(NULL, WUNG_OP_DATA, right_node, NULL);
            break;

        default:
            printf("error assign type %s .", wung_ast_print_kind(ast->kind));
    }
}

void wung_compile_binary_op(wnode * result, wung_ast * ast) {
    wung_ast * left_ast = ast->child[0];
    wung_ast * right_ast = ast->child[1];

    wnode * left_node = (wnode *) malloc(sizeof(wnode));
    wnode * right_node = (wnode *) malloc(sizeof(wnode));
    //wnode * left_node, *right_node;

    wung_compile_expr(left_node, left_ast);
    wung_compile_expr(right_node, right_ast);

    wung_make_tmp_var(result, CG(active_op_array));

    wung_emit_op(result, ast->attr, left_node, right_node);  
}

int lookup_cv(wung_op_array * op_array, wung_string *name) {
    int i=0;
    wung_ulong name_hash = wung_hash_func(name->val, name->len);
    for (i=0; i< op_array->last_var; i++) {
        wung_string * v = op_array->vars[i];
        if (v->len==name->len
        && memcmp(v->val, name->val, name->len)==0) {
            return i;
        }
    }
    op_array->last_var = i+1;
    if (i>=op_array->var_size) {
        op_array->var_size += 16;
        op_array->vars = (wung_string**)realloc(
            op_array->vars,
            sizeof(wung_string) * op_array->var_size
        );
    }
    op_array->vars[i] = name;
    return i;
}

void wung_compile_var(wnode * result, wung_ast * ast) {
    switch(ast->kind) {
        case WUNG_AST_VAR:
            wung_compile_simple_var(result, ast);
            break;
        case WUNG_AST_DIM:
            wung_compile_dim(result, ast);
            break;
    }
}

void wung_compile_simple_var(wnode * result, wung_ast * ast) {
    wung_ast * name_ast = ast->child[0];
    wung_string * name = name_ast->val.value.str;
    result->op_type = IS_CV;
    result->u.var = lookup_cv(CG(active_op_array), name);
}

void wung_compile_dim(wnode * node, wung_ast * ast) {
    wung_ast * var_ast = ast->child[0];
    wung_ast * offset_ast = ast->child[1];

    wnode * var_node = (wnode *) malloc(sizeof(wnode));
    wnode * offset_node = (wnode *) malloc(sizeof(wnode));
    
    wung_compile_expr(offset_node, offset_ast);
    wung_compile_var(var_node, var_ast);

    wung_make_tmp_var(node, CG(active_op_array));
    wung_op * opline = wung_emit_op(node, WUNG_FETCH_DIM_R, var_node, offset_node);
}

void wung_compile_array(wnode * result, wung_ast * ast) {
    int i;
    wung_op * opline;
    wnode * node;
    result->op_type = IS_CV;
    for(i=0; i< ast->children; i++) {
        node = (wnode *) malloc(sizeof(wnode));
        wung_compile_expr(node, ast->child[i]->child[0]);
        if (i==0) {
            opline = wung_emit_op(result, WUNG_INIT_ARRAY, node, NULL); 
            opline->extended_value = ast->children;
        } else {
            opline = wung_emit_op(result, WUNG_ADD_ARRAY_ELEMENT, node, NULL); 
        }
    }
}

void wung_compile_expr(wnode * result, wung_ast * ast) {
    switch(ast->kind) {
        case WUNG_AST_ZVAL:
           result->u.constant = wung_add_literal(CG(active_op_array), &(ast->val));
           result->op_type = IS_CONST;
        break;
        
        case WUNG_AST_BINARY_OP:
            wung_compile_binary_op(result, ast);
            break;

        case WUNG_AST_VAR:
        case WUNG_AST_DIM:
            wung_compile_var(result, ast);            
            break; 
        case WUNG_AST_ARRAY:
            wung_compile_array(result, ast);            
            break; 

        default:
        printf("error kind:%s on wung_compile_expr\n", wung_ast_print_kind(ast->kind));
        exit(1);
    }
}


wung_op * get_next_op(wung_op_array * op_array) {
    op_array->last++;
    if (op_array->last>=op_array->opcode_size) {
        op_array->opcode_size *= 4;
        op_array->opcodes = (wung_op*)realloc(op_array->opcodes, sizeof(wung_op) * op_array->opcode_size);
    }
    return op_array->opcodes + op_array->last-1;
}

wung_op *  wung_emit_op(wnode * result, char opcode, wnode *op1, wnode * op2) {
    wung_op * op = get_next_op(CG(active_op_array)); 
    memset(op, 0, sizeof(wung_op));
    op->opcode = opcode;
    op->op1 = op1;
    op->op2 = op2;
    op->result = result;
    return op;
}
void wung_make_tmp_var(wnode* result, wung_op_array * op_array) {
    result->op_type = IS_TMP_VAR;
    result->u.constant = op_array->T++;
}

void pass_two(wung_op_array * op_array) {
    int i=0;
    for(i=0; i< op_array->last; i++) {
        wung_vm_set_opcode_handler(op_array->opcodes+i);
    }
}

const char * opcode2str(int opcode) {
    switch(opcode) {
        case WUNG_ECHO:return "WUNG_ECHO";
        case WUNG_ADD:return "WUNG_ADD\t";
        case WUNG_SUB:return "WUNG_SUB";
        case WUNG_MUL:return "WUNG_MUL";
        case WUNG_DIV:return "WUNG_DIV";
        case WUNG_ASSIGN:return "WUNG_ASSIGN";
        case WUNG_CONCAT:return "WUNG_CONCAT";
        case WUNG_INIT_ARRAY:return "WUNG_INIT_ARRAY";
        case WUNG_ADD_ARRAY_ELEMENT:return "WUNG_ADD_ARRAY_ELEMENT";
        case WUNG_FETCH_DIM_R:return "WUNG_FETCH_DIM_R";
        case WUNG_FETCH_DIM_W:return "WUNG_FETCH_DIM_W";
        case WUNG_ASSIGN_DIM:return "WUNG_ASSIGN_DIM";
        case WUNG_OP_DATA:return "WUNG_OP_DATA";
    }
    return "ERROR";
}
const char * wnode_type_2_str(char type) {
    switch(type) {
        case IS_CONST:return "CONST";
        case IS_TMP_VAR:return "TMP_VAR";
        case IS_VAR:return "VAR";
        case IS_UNUSED:return "UNUSED";
        case IS_CV:return "CV";
    }
    return "ERROR";
}

void wung_print_opline(const wung_op * opline) {
    const char * opcode = opcode2str(opline->opcode);
    printf("opline:%s\t%s:%d", 
            opcode2str(opline->opcode),
            wnode_type_2_str(opline->op1->op_type),
            opline->op1->u.constant
    ); 

    if(opline->op2) {
        printf("\t%s:%d\t",
            wnode_type_2_str(opline->op2->op_type),
            opline->op2->u.constant
        );
    } else {
        printf("\t");
    }
    if(opline->result) {
        printf("\t%s:%d\n",
            wnode_type_2_str(opline->result->op_type),
            opline->result->u.constant
        );
    } else {
        printf("\n");
    }
}
