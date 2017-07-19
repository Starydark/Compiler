#include "mips.h"
#include "stack.h"

int argNum = 0;

void mir_newCode(InterCode ir, struct Frame_* frame);
void load_to_reg(Operand op, int reg_type, struct Frame_* frame);
char *reg_name[] = {
    "$zero",
    "$at",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9",
    "$k0", "$k1",
    "$gp",
    "$sp",
    "$fp",
    "$ra"
};

char *mips_name[] = {
    "function", "label", "li", "la", "j", "jr", "jal", "move", "add", "addi", "sub", "mul", "div",
    "mflo", "lw", "sw", "beq", "bne", "bgt", "blt", "bge", "ble"
};

char *common_mips_header = ".data\n\
_prompt: .asciiz \"Enter an integer:\"\n\
_ret: .asciiz \"\\n\"\n\
.globl main\n\
.text\n\
read:\n\
li $v0, 4\n\
la $a0, _prompt\n\
syscall\n\
li $v0, 5\n\
syscall\n\
jr $ra\n\
\n\
write:\n\
li $v0, 1\n\
syscall\n\
li $v0, 4\n\
la $a0, _ret\n\
syscall\n\
move $v0, $0\n\
jr $ra\n\
\n";

MInterCode offsetCode;

void ir_to_mips(InterCode ir_list, InterCode ir_tail, FILE *fp){
	if(ir_list == NULL){
		return;
	}
	InterCode head = ir_list;
	InterCode tail = head->next;
	while(tail != NULL){
		if(tail->kind == 1) {
			mi_translate_func(head, tail->prev);
			head = tail;
		}
		tail = tail->next;
	}
	mi_translate_func(head, ir_tail);	
	fprintf(fp, "%s", common_mips_header);
	output(fp);
}

void mi_translate_func(InterCode head, InterCode tail){	
	Frame frame = new_frame();
	//mir_newCode()
	//argNum = 0;
	mir_newCode(head, frame);
	//printf("head%s\n", head->ops.op_one->u.name);
	MInterCode tt = new_mips(15, 2, new_reg(REG_FP), new_regoffset(REG_SP, 0));
	//printf("njanfda %s kind: %d \n", mips_name[tt->kind], tt->kind);
	mir_add(tt);
	mir_add(new_mips(15, 2, new_reg(REG_RA), new_regoffset(REG_SP, -4)));
	mir_add(new_mips(7, 2, new_reg(REG_FP), new_reg(REG_SP)));	
	offsetCode = new_mips(9, 3, new_reg(REG_SP), new_reg(REG_SP), new_regimm(0));
	mir_add(offsetCode);

	InterCode p = head->next;
	
	while(p->kind == 13){
		add_arg_var(frame, p->ops.op_one);
		p = p->next;
	}
	
	while(p != tail->next){
		mir_newCode(p, frame);
		p = p->next;
	}
	int offset = totalOffset(frame);
	offsetCode->ops[2]->u.value = offset;
	//release_frame(frame);
}

void mir_newCode(InterCode ir, Frame frame){
	if(ir->kind == 2 && ir->ops.op_two[0]->addr == 2){
		int arrOffset = findVar(frame, ir->ops.op_two[0]);
		mir_add(new_mips(14, 2, new_reg(REG_T1), new_regoffset(REG_FP, arrOffset)));
		if(findVar(frame, ir->ops.op_two[1]) == -1) {
			add_local_var(frame, ir->ops.op_two[1]);
		}
		load_to_reg(ir->ops.op_two[1], REG_T2, frame);
		mir_add(new_mips(15, 2, new_reg(REG_T2), new_regoffset(REG_T1, 0)));
		return;	
	}
	if (Debug == 1)
		printf("mir_add start\n");
	
	switch(ir->kind) {
		case 9: case 11: case 15: {
			if(findVar(frame, ir->ops.op_one) == -1) {
				add_local_var(frame, ir->ops.op_one);
			}
			load_to_reg(ir->ops.op_one, REG_T1, frame);
			break;
		}
		case 2: case 12: {
			if(findVar(frame, ir->ops.op_two[0]) == -1) {
				add_local_var(frame, ir->ops.op_two[0]);
			}
			load_to_reg(ir->ops.op_two[0], REG_T1, frame);
			if(ir->kind == 2) {
				if(findVar(frame, ir->ops.op_two[1]) == -1) {
					add_local_var(frame, ir->ops.op_two[1]);
				}
				load_to_reg(ir->ops.op_two[1], REG_T2, frame);
			}
			break;
		}
		case 8: {
			if(findVar(frame, ir->ops.op_three[0]) == -1) {
				add_local_var(frame, ir->ops.op_three[0]);
			}
			load_to_reg(ir->ops.op_three[0], REG_T2, frame);
			if(findVar(frame, ir->ops.op_three[1]) == -1) {
					add_local_var(frame, ir->ops.op_three[1]);
				}
			load_to_reg(ir->ops.op_three[1], REG_T3, frame);
			break;
		} 
		case 3: case 4: case 5: case 6:{
			if(findVar(frame, ir->ops.op_three[0]) == -1) {
				add_local_var(frame, ir->ops.op_three[0]);
			}
			load_to_reg(ir->ops.op_three[0], REG_T1, frame);
			if(findVar(frame, ir->ops.op_three[1]) == -1) {
					add_local_var(frame, ir->ops.op_three[1]);
				}
			load_to_reg(ir->ops.op_three[1], REG_T2, frame);
			if(findVar(frame, ir->ops.op_three[2]) == -1) {
				add_local_var(frame, ir->ops.op_three[2]);
			}
			load_to_reg(ir->ops.op_three[2], REG_T3, frame);
			break;
		}
		case 14: {
			if(findVar(frame, ir->ops.op_one) == -1) {
				add_local_var(frame, ir->ops.op_one);
			}
			break;
		}
		case 10: {
			if(findVar(frame, ir->ops.op_one) == -1) {
				add_local_arr(frame, ir->ops.op_one, ir->u.size);
			}
			break;
		}
		default:{
			//printf("load unexpeted type: %d\n", ir->kind);
		}
	}
	
	if(Debug == 1)
		printf("load successful\n");
	
	switch(ir->kind){
		case 0: { //LABEL
			mir_add(new_mips(1, 1, new_mlabel(ir->ops.op_one->u.no)));
			break;
		}
		case 1: { //FUNCTION
			MOperand op = new_func(ir->ops.op_one->u.name);	
			mir_add(new_mips(0, 1, op));
			
			break;
		}
		case 2: { //ASSIGEN
			/*newcode->kind = 7;
			newcode->ops[0] = op1;
			newcode->ops[1] = op2;
			*/
			mir_add(new_mips(7, 2, new_reg(REG_T1), new_reg(REG_T2)));
			break;
		}
		case 3: { //ADD
			/*newcode->kind = 8;
			newcode->ops[0] = op1;
			newcode->ops[1] = op2;
			newcode->ops[2] = op3;
			*/
			mir_add(new_mips(8, 3, new_reg(REG_T1), new_reg(REG_T2), new_reg(REG_T3)));
			break;
		} 
		case 4: { //SUB
			/*
			newcode->kind = 10;
			newcode->ops[0] = op1;
			newcode->ops[1] = op2;
			newcode->ops[2] = op3;
			*/
			mir_add(new_mips(10, 3, new_reg(REG_T1), new_reg(REG_T2), new_reg(REG_T3)));
			break;
		}
		case 5: { //MUL
			/*
			newcode->kind = 11;
			newcode->ops[0] = op1;
			newcode->ops[1] = op2;
			newcode->ops[2] = op3;
			*/
			mir_add(new_mips(11, 3, new_reg(REG_T1), new_reg(REG_T2), new_reg(REG_T3)));
			break;
		}
		case 6: { //DIV
			
			/*MInterCode newcode1 = (MInterCode)malloc(sizeof(struct MInterCode_));
			newcode1->kind = 12;
			newcode1->ops[0] = op2;
			newcode1->ops[1] = op3;
			newcode1->ops[2] = NULL;
			mir_add(newcode1);
			*/
			mir_add(new_mips(12, 2, new_reg(REG_T2), new_reg(REG_T3)));
			mir_add(new_mips(13, 1, new_reg(REG_T1)));
			//newcode->kind = 13
			//newcode->ops[0] = op1;
			break;
		}
		case 7: { //GOTO
			MOperand op = new_mlabel(ir->ops.op_one->u.no);
			mir_add(new_mips(4, 1, op));
			break;
		}
		case 8: { //IF_GOTO
			int relop_type;
			switch(ir->u.relop_type){
				case 0: {
					relop_type = 16;
					break;
				}
				case 1: {
					relop_type = 17;
					break;
				}
				case 2: {
					relop_type = 18;
					break;
				}
				case 3: {
					relop_type = 19;
					break;
				}
				case 4: {
					relop_type = 20;
					break;
				}
				case 5: {
					relop_type = 21;
					break;
				}
			}
			//newcode->ops[0] = op2;
			//newcode->ops[1] = op3;
			MOperand op = new_mlabel(ir->ops.op_three[2]->u.no);
			//newcode->ops[2] = op;
			mir_add(new_mips(relop_type, 3, new_reg(REG_T2), new_reg(REG_T3), op));
			break;
		}
		case 9: { //RETURN
			mir_add(new_mips(7, 2, new_reg(REG_V0), new_reg(REG_T1)));
			mir_add(new_mips(7, 2, new_reg(REG_SP), new_reg(REG_FP)));
			mir_add(new_mips(14, 2, new_reg(REG_RA), new_regoffset(REG_SP, -4)));
			mir_add(new_mips(14, 2, new_reg(REG_FP), new_regoffset(REG_SP, 0)));
			mir_add(new_mips(5, 1, new_reg(REG_RA)));
			break;
		}
		case 10: { //DEC
			break;
		}
		case 11: { //ARG
			argNum ++;
			mir_add(new_mips(15, 2, new_reg(REG_T1), new_regoffset(REG_SP, 0)));
			mir_add(new_mips(9, 3, new_reg(REG_SP), new_reg(REG_SP), new_regimm(-4)));
			break;
		}
		case 12: { //CALL			
			/*newcode->kind = 7;
			newcode->ops[0] = op1;
			MOperand op = new_reg(REG_V0);
			newcode->ops[1] = op;
			*/
			mir_add(new_mips(6, 1, new_func(ir->ops.op_two[1]->u.name)));
			mir_add(new_mips(7, 2, new_reg(REG_T1), new_reg(REG_V0)));
			break;
		}
		case 13: { //PARAM
			break;
		}
		case 14: { //READ
			mir_add(new_mips(15, 2, new_reg(REG_RA), new_regoffset(REG_SP, 0)));
			mir_add(new_mips(6, 1, new_func("read")));
			mir_add(new_mips(7, 2, new_reg(REG_T1), new_reg(REG_V0)));
			mir_add(new_mips(14, 2, new_reg(REG_RA), new_regoffset(REG_SP, 0)));
			break;
		}
		case 15: { //WRITE
			mir_add(new_mips(15, 2, new_reg(REG_RA), new_regoffset(REG_SP, 0)));
			mir_add(new_mips(7, 2, new_reg(REG_A0), new_reg(REG_T1)));
			mir_add(new_mips(6, 1, new_func("write")));
			mir_add(new_mips(14, 2, new_reg(REG_RA), new_regoffset(REG_SP, 0)));
			break;
		}
		default: {
			printf("error\n");	
		}
	}
	
	//store result
	int offset;	
	switch(ir->kind ){
		case 2: case 14: {
			offset = findVar(frame, ir->ops.op_one);
			mir_add(new_mips(15, 2, new_reg(REG_T1), new_regoffset(REG_FP, offset)));
			break;
		}
		case 3: case 4: case 5: case 6:{
			offset = findVar(frame, ir->ops.op_three[0]);
			mir_add(new_mips(15, 2, new_reg(REG_T1), new_regoffset(REG_FP, offset)));
			break;
		}
		case 12: {
			offset = findVar(frame, ir->ops.op_two[0]);
			mir_add(new_mips(15, 2, new_reg(REG_T1), new_regoffset(REG_FP, offset)));
			break;
		}
		
	}
	if(Debug == 1)
		printf("mir_add end\n");
	
}

void load_to_reg(Operand op, int reg_type, Frame frame){
	if(op->kind == 0 || op->kind == 2){
		int offset = findVar(frame, op);
		if(offset == -1){
			printf("Error in stackframe\n");
		}
		if(op->addr == 0){
			mir_add(new_mips(14, 2, new_reg(reg_type), new_regoffset(REG_FP, offset)));
		}
		else if(op->addr == 1){
			mir_add(new_mips(3, 2, new_reg(reg_type), new_regoffset(REG_FP, offset)));
		}
		else if(op->addr == 2){
			mir_add(new_mips(14, 2, new_reg(reg_type), new_regoffset(REG_FP, offset)));
			mir_add(new_mips(14, 2, new_reg(reg_type), new_regoffset(reg_type, 0)));
		}
	}
	else if(op->kind == 1){
		mir_add(new_mips(2, 2, new_reg(reg_type), new_regimm(op->u.value)));
	}
	else {
		printf("error in loadToReg\n");
	}
}

void mir_add(MInterCode newnode){
	if(mir_list == NULL){
		mir_list = newnode;
		newnode->prev = NULL;
		newnode->next = NULL;
	}
	else {
		MInterCode p = mir_list;
		while(p->next != NULL)
			p = p->next;
		p->next = newnode;
		newnode->prev = p;
		newnode->next = NULL;
	}
	mtail = newnode;
}

MOperand new_regimm(int value){
	MOperand op = (MOperand)malloc(sizeof(struct MOperand_));
	op->kind = 1;
	op->u.value = value;
	return op;
}

MOperand new_reg(int no){
	MOperand op = (MOperand)malloc(sizeof(struct MOperand_));
	op->kind = 2;
	op->u.Reg.reg_type = no;
	return op;
}

MOperand new_regoffset(int no, int offset){
	MOperand op = (MOperand)malloc(sizeof(struct MOperand_));
	op->kind = 4;
	op->u.Reg.reg_type = no;
	op->u.Reg.offset = offset;
	return op;
}

MOperand new_mlabel(int no){
	MOperand op = (MOperand)malloc(sizeof(struct MOperand_));
	op->kind = 0;
	op->u.value = no;
	return op;
}

MOperand new_func(char *name){
	MOperand op = (MOperand)malloc(sizeof(struct MOperand_));
	op->kind = 3;
	op->u.name = (char *)malloc(sizeof(char) * 30);
	strcpy(op->u.name, name);
	return op;
}

MInterCode new_mips(int flag, ...){
	MInterCode newcode = (MInterCode)malloc(sizeof(struct MInterCode_));
	for(int i = 0;i < 3; i++)
		newcode->ops[i] = NULL;
	newcode->kind = flag;
	va_list pArg;
	va_start(pArg, flag);
	int sum = va_arg(pArg, int);
	for(int i = 0;i < sum;i ++){
		newcode->ops[i] = va_arg(pArg, MOperand);
	}
	return newcode;
}

void output(FILE *fp){
	MInterCode p = mir_list;
	while(p != NULL){
		if(p->kind != 0 && p->kind != 1)
			fprintf(fp, "%s", mips_name[p->kind]);
		if(p->ops[0]){
			if(p->kind != 0 && p->kind != 1)
				fprintf(fp, " ");
			outputOP(p->ops[0], fp);
		}
		if(p->ops[1]){
			fprintf(fp, ", ");
			outputOP(p->ops[1], fp);
		}
		if(p->ops[2]){
			fprintf(fp, ", ");
			outputOP(p->ops[2], fp);
		}
		if(p->kind == 0 || p->kind == 1){
			fprintf(fp, ":");
		}
		fprintf(fp, "\n");
		p = p->next;
	}
}

void outputOP(MOperand op, FILE *fp){
	switch(op->kind){
		case 0: {
			fprintf(fp, "label%d", op->u.value);
			break;
		}
		case 1: {
			fprintf(fp, "%d", op->u.value);
			break;
		}
		case 2: {
			fprintf(fp, "%s", reg_name[op->u.Reg.reg_type]);
			break;
		}
		case 3: {
			fprintf(fp, "%s", op->u.name);
			break;
		}
		case 4: {
			fprintf(fp, "%d(%s)", op->u.Reg.offset, reg_name[op->u.Reg.reg_type]);
			break;
		}
	}
}
