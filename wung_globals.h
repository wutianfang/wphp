#include "wung.h"
#include "wung_types.h"
#include "wung_ast.h"
#include "wung_compile.h"
#include "wung_execute.h"

typedef struct _wung_scanner_globals {
    unsigned char *yy_cursor;
    unsigned char *yy_marker;
    unsigned char *yy_limit;
	unsigned char *yy_text;
}wung_scanner_globals;

typedef struct _wung_compiler_globals {
    wung_op_array *active_op_array;
    wung_ast * ast;
    int wung_lineno;
}wung_compiler_globals;

typedef struct _wung_executor_gloabls {
    wung_array symbol_table;
    HashTable included_files;
    int exit_status;

    wval          *vm_stack_top;
    wval          *vm_stack_end;
    wung_execute_data * current_execute_data;

    HashTable *function_table;
    HashTable *class_table; 
} wung_executor_gloabls;



extern wung_scanner_globals language_scanner_globals;
#define SCNG(v) language_scanner_globals.v

extern wung_compiler_globals compiler_globals;
#define CG(v)  compiler_globals.v
