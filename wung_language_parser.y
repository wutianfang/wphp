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

%%
start:
    top_statement_list { CG(ast)=$1; }
    top_statement_list :
		top_statement_list top_statement {
            $$ = wung_ast_add_list($1, $2);
        }
	|	{ $$ = wung_ast_create_list(64, WUNG_AST_LIST, 0);}
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
    |	expr '+' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_ADD, $1, $3); }
    |	expr '-' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_SUB, $1, $3); }
    |	expr '*' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_MUL, $1, $3); }
    |	expr '/' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_DIV, $1, $3); }
    |   variable {$$ = $1;}
	;
variable :
    T_VARIABLE  { $$ = wung_ast_create_1_child(WUNG_AST_VAR ,0, $1); }

%%


int compile_string(char *string) {
    SCNG(yy_cursor) = (unsigned char *)string;
	int len = strlen(string);
	SCNG(yy_limit) = (unsigned char*)string + len - 1;
    yyparse();
    
    //wung_ast_print(CG(ast), 0);
    CG(active_op_array) = malloc(sizeof(wung_op_array));
    wung_init_op_array(CG(active_op_array));
    wung_compile_top_stmt(CG(ast));

    return 0;
}


void yyerror (char const *s) {

    fprintf (stderr, "%s xx\n", s);
}
