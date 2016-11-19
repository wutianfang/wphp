%{
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "wung_globals.h"
#include "wung_types.h"
#include "wung_language_parser.tab.h"
#include "wung_ast.h"
#include "wung_compile.h"
#include "wung_vm_opcodes.h"

extern int yylex(parse_ele*);
void yyerror (char const *);
#define YYSTYPE parse_ele
%}


%pure-parser

%token T_BEGIN
%token <ast>T_ECHO
%token <ast>T_NUMBER
%token <ast>T_VARIABLE
%token <ast>T_STRING
%token <ast>T_INPUT_ERROR
%token <ast>T_LOWER_CHAR
%token <ast>T_UPPER_CHAR
%token T_END  0
%token T_WHITESPACE
%left '+' '-'
%right '*' '/'

%type <ast> top_statement_list top_statement statement
%type <ast> variable
%type <ast> expr
%type <ast> array_scalar non_empty_array_pair_list array_pair

%%
start:
    top_statement_list { CG(ast)=$1; }
    top_statement_list :
		top_statement_list top_statement {
            $$ = wung_ast_add_list($1, $2);
        }
	|	{ $$ = wung_ast_create_list(WUNG_AST_LIST, 0);}
	;
top_statement:
    statement { $$ = $1; }
    ;
statement:
		T_ECHO expr ';' { $$ = wung_ast_create_1_child(WUNG_AST_ECHO, 0, $2); }	
    |   variable '=' expr ';' { $$ = wung_ast_create_2_child(WUNG_AST_ASSIGN, 0, $1, $3);}
    ;
expr :
		T_NUMBER { $$ = $1; }
    |	T_STRING { $$ = $1; }
    |	expr '+' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_ADD, $1, $3); }
    |	expr '-' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_SUB, $1, $3); }
    |	expr '*' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_MUL, $1, $3); }
    |	expr '/' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_DIV, $1, $3); }
    |	expr '.' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_CONCAT, $1, $3); }
    |   variable {$$ = $1;}
    |	'(' expr ')'  { $$ = $2;}
    |   array_scalar { $$ = $1;}
    ;
variable :
        T_VARIABLE  { $$ = wung_ast_create_1_child(WUNG_AST_VAR ,0, $1); }
    |   T_VARIABLE '[' T_NUMBER ']' { 
            $$ = wung_ast_create_2_child(
                WUNG_AST_DIM, 0,
                wung_ast_create_1_child(WUNG_AST_VAR, 0, $1),
                $3);
        }
    |  T_VARIABLE '[' ']' {
            $$ = wung_ast_create_2_child(
                WUNG_AST_DIM, 0,
                wung_ast_create_1_child(WUNG_AST_VAR, 0, $1),
                NULL);
        }

array_scalar :
        '[' ']' { $$ = wung_ast_create_list(WUNG_AST_ARRAY, 0); }
    |   '[' non_empty_array_pair_list ']' { $$ = $2; }
    ;
non_empty_array_pair_list :
        non_empty_array_pair_list ',' array_pair {  $$ = wung_ast_add_list($1, $3); }
    |   array_pair {
            $$ = wung_ast_create_list(WUNG_AST_ARRAY, 0);
            $$ = wung_ast_add_list($$, $1);
        }
    ;
array_pair :
        expr { $$ = wung_ast_create_2_child(WUNG_AST_ARRAY_ELEM, 0, $1, NULL); }
    ;

%%


wung_op_array * compile_string(char *string) {
    SCNG(yy_cursor) = (unsigned char *)string;
	int len = strlen(string);
	SCNG(yy_limit) = (unsigned char*)string + len - 1;
    yyparse();
    
    wung_ast_print(CG(ast), 0);
    CG(active_op_array) = (wung_op_array*)malloc(sizeof(wung_op_array));
    wung_init_op_array(CG(active_op_array));
    wung_compile_top_stmt(CG(ast));
    pass_two(CG(active_op_array));

    return CG(active_op_array);
}


void yyerror (char const *s) {
    fprintf (stderr, "%s xx\n", s);
    exit(1);
}
