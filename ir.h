#ifndef _IR_H
#define _IR_H
	
#include "sy.h"
#include "hash.h"
#include "common.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "string.h"

typedef struct Operand_* Operand;
typedef struct InterCode_* InterCode;

struct Operand_ {
	enum {
		NONE,
		OP_AND, //&
		OP_STAR //*
	} addr;	
	enum {
		VARIABLE,
		IMMEDIATE,
		TEMP,
		OP_LABEL,
		OP_FUNCTION
	} kind;
	union {
		int no;
		int value;
		char *name;
	} u;
	bool argument;
};

struct InterCode_ {
	enum {
		LABEL,
		FUNCTION,
		ASSIGN,
		ADD,
		SUB,
		MUL,
		DIV,
		GOTO,
		IF_GOTO,
		RETURN, //9
		DEC,
		ARG,
		CALL,
		PARAM,
		READ,
		WRITE
	}kind;
	union {
		Operand op_one;
		Operand op_two[2];
		Operand op_three[3];
	} ops;
	union {
		enum {
			RELOP_EQ,
			RELOP_NEQ,
			RELOP_G,
			RELOP_L,
			RELOP_GE,
			RELOP_LE
		} relop_type;
		int size;
	} u;
	InterCode prev, next;
};

InterCode ir_list;
InterCode tail;

Operand new_temp();
Operand new_label();
Operand new_var();
Operand ir_num(int num);
void ir_print(FILE* fp);
void ir_add(InterCode newnode);
void ir_newCode(int flag, ...);
void printCode(InterCode p, FILE* fp);
void printOp(Operand op, FILE* fp);
void printRelop(int type, FILE* fp);
void clean_temp();
void printOP(Operand op);
void clean_temp_var();
void assign(Operand dst, Operand src);
bool equal(Operand dst, Operand src);

#endif
