#ifndef _MIPS_H
#define _MIPS_H

#include "common.h"
#include "sy.h"
#include "ir.h"
#include "common.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>

//extern typedef struct Frame_* Frame;
typedef struct MOperand_* MOperand;
typedef struct MInterCode_* MInterCode;

struct MOperand_ {
	enum {
		OP_LABLE,
		OP_IMM,
		OP_REG,
		OP_FUNC,
		OP_ADDR
	}kind;
	union {
		int value;
		char *name;
		struct {
			int reg_type;
			int offset;
		}Reg;
	} u;
};

struct MInterCode_ {
	enum {
		MI_FUNCTION,
		MI_LABEL,
		MI_LI,
		MI_LA,
		MI_J,
		MI_JR,
		MI_JAL,
		MI_MOVE,
		MI_ADD, //8
		MI_ADDI,
		MI_SUB,
		MI_MUL,
		MI_DIV,
		MI_MFLO,
		MI_LW, //14
		MI_SW,
		MI_BEQ,
		MI_BNE,
		MI_BGT,
		MI_BLT,
		MI_BGE,
		MI_BLE
	}kind;
	MOperand ops[3];
	MInterCode prev, next;
};

MInterCode mir_list;
MInterCode mtail;

void output(FILE *fp);
void outputOP(MOperand op, FILE *fp);
void ir_to_mips(InterCode ir_list, InterCode ir_tail, FILE *fp);
void mi_translate_func(InterCode head, InterCode tail);
void mir_add(MInterCode newnode);
MOperand new_regimm(int value);
MOperand new_reg(int no);
MOperand new_regoffset(int no, int offset);
MOperand new_mlabel(int no);
MOperand new_func(char *name);
MInterCode new_mips(int flag, ...);

#endif
