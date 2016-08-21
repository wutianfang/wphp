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
%token <ast>T_INPUT_ERROR
%token <ast>T_LOWER_CHAR
%token <ast>T_UPPER_CHAR
%token T_END  0
%token T_WHITESPACE
%left '+' '-'
%right '*' '/'

%type <ast> top_statement_list top_statement
%type <ast> expr

%%
top_statement_list :
		top_statement_list top_statement {$$ = wung_ast_add_list($1, $2);}
	|	{ $$ = wung_ast_create_list(10, WUNG_AST_LIST, 0);}
	;
top_statement:
		T_ECHO expr ';' { $$ = wung_ast_create_1_child(WUNG_AST_ECHO, 0, $2); }	
expr :
		T_NUMBER { $$ = $1; }
    |	expr '+' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_ADD, $1, $3); }
    |	expr '-' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_SUB, $1, $3); }
    |	expr '*' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_MUL, $1, $3); }
    |	expr '/' expr { $$ = wung_ast_create_2_child(WUNG_AST_BINARY_OP, WUNG_DIV, $1, $3); }
	;

%%
int main(int argc, char * argv[]) {
    SCNG(yy_cursor) = (unsigned char *)argv[1];
	int len = strlen(argv[1]);
	SCNG(yy_limit) = (unsigned char*)argv[1] + len - 1;

    return yyparse();
}
void yyerror (char const *s) {

    fprintf (stderr, "%s xx\n", s);
}
