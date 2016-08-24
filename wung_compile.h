#ifndef WUNG_COMPILE_H
#define WUNG_COMPILE_H
#include "wung.h"

typedef union _parse_elem {
	wung_ast * ast;
	wung_ulong num;
}parse_ele;

typedef union _wnode {
	char op_type;
	char flag;
	wval constant;
}wnode;

#endif
