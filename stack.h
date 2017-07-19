#ifndef _STACK_H
#define _STACK_H

#include "ir.h"
#include "mips.h"
#include "common.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>

typedef struct Frame_* Frame;
typedef struct LocalVar_* LocalVar;

struct LocalVar_{
	int op_type;// variable or temp_var
	int no;
	int offset;
	LocalVar next;
};

struct Frame_{
	LocalVar vars;
	int var_offset;
	int arg_offset;
};

void add_local_var(Frame frame, Operand op);
void add_arg_var(Frame frame, Operand op);
void add_local_arr(Frame frame, Operand op, int size);
int findVar(Frame frame, Operand op);
int totalOffset(Frame frame);
Frame new_frame();

void release_frame(Frame frame);

#endif
