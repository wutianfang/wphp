%{
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "wung_globals.h"
#include "wung_types.h"
#include "wung_language_parser.tab.h"

extern int yylex(wval*);
void yyerror (char const *);
#define YYSTYPE wval
%}


%pure-parser

%token T_BEGIN
%token T_NUMBER
%token T_INPUT_ERROR
%token T_LOWER_CHAR
%token T_UPPER_CHAR
%token T_END  0
%token T_WHITESPACE
%left '+' '-'
%right '*' '/'

%%
line :
	T_END
    | expr T_END {
		printf ("%d\n", $1.value.lval); 
	}
	;

expr :
    T_NUMBER {$$=$1; }
    | expr '+' expr { 
		$$.value.lval=$1.value.lval+$3.value.lval; 
	}
    | expr '-' expr { $$.value.lval=$1.value.lval-$3.value.lval; }
    | expr '*' expr { $$.value.lval=$1.value.lval*$3.value.lval; }
    | expr '/' expr { $$.value.lval=$1.value.lval/$3.value.lval; }
	;

%%
int main(int argc, char * argv[]) {
    SCNG(yy_cursor) = (unsigned char *)argv[1];
	int len = strlen(argv[1]);
	SCNG(yy_limit) = (unsigned char*)argv[1] + len - 1;

    return yyparse ();
}
void yyerror (char const *s) {
	
    fprintf (stderr, "%s xx\n", s);
}
