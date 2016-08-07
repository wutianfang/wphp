#include <stdio.h>
#include <stdlib.h>
#include "wung_globals.h"
#include "wung_types.h"
#include "wung_language_parser.tab.h"
wung_scanner_globals language_scanner_globals;


#define YYCTYPE char
#define YYCURSOR SCNG(yy_cursor)
#define YYLIMIT  SCNG(yy_limit)
#define YYMARKER SCNG(yy_marker)
#define YYTEXT SCNG(yy_text)
#define YYFILL(n)

int wung_lex(wval * wval);

int yylex(wval * val) {
	WVAL_UNDEF(val);
	int retval;
begin:
	retval = wung_lex(val);
	switch(retval) {
		case T_WHITESPACE:
			goto begin;
	}
	return retval;
}

int wung_lex(wval * val) {
	YYTEXT = YYCURSOR;

	if (YYCURSOR > YYLIMIT) {
		return T_END;
	}

    /*!re2c
    "end" {return T_END;}
    [0-9]+ {
		WVAL_LONG(val,atoi((const char*)YYTEXT));
		return T_NUMBER;
	}
    "+" {return '+';}
    "-" {return '-';}
    "*" {return '*';}
    "/" {return '/';}
    [a-z]+ {
		return T_LOWER_CHAR;
	}
    [A-Z]+ {
		return T_UPPER_CHAR;}
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
