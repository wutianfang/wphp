#include <stdio.h>
#include <stdlib.h>
#include "wung_globals.h"
#include "wung_types.h"
#include "wung_ast.h"
#include "wung_compile.h"
#include "wung_language_parser.tab.h"
wung_scanner_globals language_scanner_globals;


#define YYCTYPE char
#define YYCURSOR SCNG(yy_cursor)
#define YYLIMIT  SCNG(yy_limit)
#define YYMARKER SCNG(yy_marker)
#define YYTEXT SCNG(yy_text)
#define YYFILL(n)

int wung_lex(wval * wval);

int yylex(parse_ele * ele) {
	wval val;
	WVAL_UNDEF(&val);
	int retval;
begin:
	retval = wung_lex(&val);
	switch(retval) {
		case T_WHITESPACE:
			goto begin;
	}
	ele->ast = wung_ast_create_val(&val);
	return retval;
}

int wung_lex(wval * val) {
	YYTEXT = YYCURSOR;

	if (YYCURSOR > YYLIMIT) {
		return T_END;
	}

    /*!re2c
    re2c:yyfill:check = 0;

    "echo" {return T_ECHO;}
    [0-9]+ {
		WVAL_LONG(val,atoi((const char*)YYTEXT));
		return T_NUMBER;
	}
    "\""[a-zA-Z_ ]+"\"" {
        WVAL_STRING(val, ( YYTEXT + 1), (YYCURSOR - YYTEXT -2));
        //printf("val:%s\n", val->value.str->val);
        return T_STRING;
    }
    "+" {return '+';}
    "-" {return '-';}
    "*" {return '*';}
    "/" {return '/';}
    ";" {return ';';}
    "=" {return '=';}
    "\$"[a-zA-Z_][a-zA-Z_0-9]* {
        WVAL_STRING(val, ( YYTEXT + 1), (YYCURSOR - YYTEXT -1));
        //printf("val:%s\n", val->value.str->val);
        return T_VARIABLE; 
    }
    [ \n\t\r]+ {
		return T_WHITESPACE;
	}
	[^] {
		if (YYCURSOR > YYLIMIT) {
			return T_END;
		}
	}
    */
}

/**
int main(int argc,char* argv[]) {
    int token;
    SCNG(yy_cursor) = (unsigned char *)argv[1];
	int len = strlen(argv[1]);
	YYLIMIT  = (unsigned char*)argv[1] + len - 1;

    while(token = yylex(&token)) {
        if (token == T_END) {
            break;
        }
    }
    return 0;
}
*/
