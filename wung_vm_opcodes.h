#ifndef WUNG_VM_OPCODES_H
#define WUNG_VM_OPCODES_H
enum _WUNG_OPCODES {
	WUNG_ECHO,
	WUNG_ADD,
	WUNG_SUB,
	WUNG_MUL,
	WUNG_DIV,
	WUNG_ASSIGN,
	WUNG_CONCAT,
    WUNG_INIT_ARRAY,
    WUNG_ADD_ARRAY_ELEMENT,
    WUNG_FETCH_DIM_R,
    WUNG_FETCH_DIM_W,
    WUNG_ASSIGN_DIM,
    WUNG_OP_DATA,
};

#endif
