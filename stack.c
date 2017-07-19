#include "stack.h"

void add_local_var(Frame frame, Operand op){
	LocalVar newVar = (LocalVar)malloc(sizeof(struct LocalVar_));
	newVar->op_type = op->kind;
	newVar->no = op->u.no;
	newVar->offset = frame->var_offset;
	frame->var_offset -= 4;
	if(frame->vars == NULL){
		frame->vars = newVar;
	}
	else {
		newVar->next = frame->vars;
		frame->vars = newVar;
	}
}

void add_arg_var(Frame frame, Operand op){
	LocalVar newVar = (LocalVar)malloc(sizeof(struct LocalVar_));
	newVar->op_type = op->kind;
	newVar->no = op->u.no;
	newVar->offset = frame->arg_offset;
	frame->arg_offset += 4;
	if(frame->vars == NULL){
		frame->vars = newVar;
	}
	else {
		newVar->next = frame->vars;
		frame->vars = newVar;
	}
	
}

void add_local_arr(Frame frame, Operand op, int size){
	LocalVar newVar = (LocalVar)malloc(sizeof(struct LocalVar_));
	newVar->op_type = op->kind;
	newVar->no = op->u.no;
	frame->var_offset -= size;
	newVar->offset = frame->var_offset + 4;
	if(frame->vars == NULL){
		frame->vars = newVar;
	}
	else {
		newVar->next = frame->vars;
		frame->vars = newVar;
	}
}

int findVar(Frame frame, Operand op){
	if(frame == NULL)
		return -1;
	LocalVar p = frame->vars;
	while(p != NULL){
		if(p->op_type == op->kind && p->no == op->u.no){
			return p->offset;
		}
		p = p->next;
	}
	return -1;
}

int totalOffset(Frame frame){
	return frame->var_offset;
}

Frame new_frame() {
	Frame frame = (Frame)malloc(sizeof(struct Frame_));
	frame->vars = NULL;
	frame->var_offset = -8;
	frame->arg_offset = 4;
	return frame;
}

void release_frame(Frame frame){
	if(frame == NULL)
		return;
	LocalVar p = frame->vars;
	while(p != NULL){
		LocalVar q = p;
		p = p->next;
		free(q);
	}
	free(frame);
}
