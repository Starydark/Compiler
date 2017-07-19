#include "ir.h"
#include "mips.h"
#include "common.h"

static int var_No = 1;
static int label_No = 1;
static int temp_No = 1;

void ir_print(FILE *fp){
	if(ir_list == NULL)
		return;
	/*int num = 0;
	InterCode q = ir_list;
	while(q != NULL){
		num ++;
		q = q->next;
	}
	InterCode p = ir_list;
	while(p != NULL){
		printCode(p, fp);
		p = p->next;
	}*/
	ir_to_mips(ir_list, tail, fp);
}

void clean_temp(Operand op1, Operand op2){
	temp_No --;
	if(op1->kind == 2){
		op1->kind = op2->kind;
		op1->u.no = op2->u.no;
		op1->argument = true;
	}
}

void clean_temp_var(){
	InterCode p = ir_list;
	while(p->next != NULL){
		bool flag = false;
		InterCode q = p->next;
		if(p->kind == 2 && p->ops.op_two[0]->kind == 2){
			if(q->kind == 2){
				if(equal(q->ops.op_two[1], p->ops.op_two[0])){
					//assign(q->ops.op_two[1], p->ops.op_two[1]);
					q->ops.op_two[1] = p->ops.op_two[1];
					flag = true;
				}
			}
			else if(q->kind > 2 && q->kind <= 6){
				if(equal(q->ops.op_three[1], p->ops.op_two[0])){
					q->ops.op_three[1] = p->ops.op_two[1];
					//assign(q->ops.op_three[1], p->ops.op_two[1]);
					flag = true;
				}
				else if(equal(q->ops.op_three[2], p->ops.op_two[0]) ){
					q->ops.op_three[2] = p->ops.op_two[1];
					//assign(q->ops.op_three[2], p->ops.op_two[1]);
					flag = true;		
				}
			}
			else if(q->kind == 8){
				if(equal(q->ops.op_three[1], p->ops.op_two[0])){
					q->ops.op_three[1] = p->ops.op_two[1];
					flag = true;
				}
			}
		}
		else if(p->kind >= 3 && p->kind <= 6){
			if(p->ops.op_three[1]->kind == 1 && p->ops.op_three[2]->kind == 1){
				int value = 0;
				switch(p->kind){
					case 3:{
						value = p->ops.op_three[1]->u.value + p->ops.op_three[2]->u.value;
						break;
					}
					case 4:{
						value = p->ops.op_three[1]->u.value - p->ops.op_three[2]->u.value;
						break;
					}
					case 5:{
						value = p->ops.op_three[1]->u.value * p->ops.op_three[2]->u.value;
						break;
					}
					case 6:{
						value = p->ops.op_three[1]->u.value / p->ops.op_three[2]->u.value;
						break;
					}
				}				
				Operand dst = p->ops.op_three[0];
				InterCode newnode = (InterCode)malloc(sizeof(struct InterCode_));
				newnode->kind = 2;
				newnode->ops.op_two[0] = dst;
				newnode->ops.op_two[1] = ir_num(value);
				InterCode temp = p;
				newnode->prev = p->prev;
				newnode->next = p->next;
				p->prev->next = newnode;
				p->next->prev = newnode;
				p = newnode->prev;
				free(temp);
				continue;
			}
			if(q->kind == 2 && q->ops.op_two[1]->kind == 2){
				if(equal(p->ops.op_three[0], q->ops.op_two[1]) && q->ops.op_two[0]->addr == 0){
					InterCode temp = q;
					p->ops.op_three[0] = q->ops.op_two[0];
					p->next = q->next;
					q->next->prev = p;
					q = q->prev;
					free(temp);					
				}
			}
		}	
		if(flag == true){
			InterCode temp = p;
			p = p->prev;
			p->next = q;
			q->prev = p;		
			free(temp);
		}	
		p = p->next;
	}
}

bool equal(Operand dst, Operand src){
	if(dst == src)
		return true;
	if(dst->addr != src->addr)
		return false;
	else if(dst->kind != src->kind)
		return false;
	else if(dst->kind == 1 && dst->u.value != src->u.value)
		return false;
	else if(dst->u.no != src->u.no)
		return false;
	return true;
}

void assign(Operand dst, Operand src){
	dst->addr = src->addr;
	dst->kind = src->kind;
	if(dst->kind == 1)
		dst->u.value = src->u.value;
	else 
		dst->u.no = src->u.no;
}

void printCode(InterCode p, FILE *fp){
	switch(p->kind){
		case 0:{
			fprintf(fp, "LABEL ");
			printOp(p->ops.op_one, fp);
			fprintf(fp, " :");
			break;
		}
		case 1:{
			fprintf(fp, "FUNCTION ");
			printOp(p->ops.op_one, fp);
			fprintf(fp, " :");
			break;
		}
		case 2:{
			printOp(p->ops.op_two[0], fp);
			fprintf(fp, " := ");
			printOp(p->ops.op_two[1], fp);
			break;
		}
		case 3:{
			printOp(p->ops.op_three[0], fp);
			fprintf(fp, " := ");
			printOp(p->ops.op_three[1], fp);
			fprintf(fp, " + ");
			printOp(p->ops.op_three[2], fp);
			break;
		}
		case 4:{
			printOp(p->ops.op_three[0], fp);
			fprintf(fp, " := ");
			printOp(p->ops.op_three[1], fp);
			fprintf(fp, " - ");
			printOp(p->ops.op_three[2], fp);
			break;
		}
		case 5:{
			printOp(p->ops.op_three[0], fp);
			fprintf(fp, " := ");
			printOp(p->ops.op_three[1], fp);
			fprintf(fp, " * ");
			printOp(p->ops.op_three[2], fp);
			break;
		}
		case 6:{
			printOp(p->ops.op_three[0], fp);
			fprintf(fp, " := ");
			printOp(p->ops.op_three[1], fp);
			fprintf(fp, " / ");
			printOp(p->ops.op_three[2], fp);
			break;
		}
		case 7:{
			fprintf(fp, "GOTO ");
			printOp(p->ops.op_one, fp);
			break;
		}
		case 8:{
			fprintf(fp, "IF ");
			printOp(p->ops.op_three[0], fp);
			printRelop(p->u.relop_type, fp);
			printOp(p->ops.op_three[1], fp);
			fprintf(fp, " GOTO ");
			printOp(p->ops.op_three[2], fp);
			break;
		}
		case 9:{
			fprintf(fp, "RETURN ");
			printOp(p->ops.op_one, fp);
			break;
		}
		case 10:{
			fprintf(fp, "DEC ");
			printOp(p->ops.op_one, fp);
			fprintf(fp, " %d", p->u.size);
			break;
		}
		case 11:{
			fprintf(fp, "ARG ");
			printOp(p->ops.op_one, fp);
			break;
		}
		case 12:{
			printOp(p->ops.op_two[0], fp);
			fprintf(fp, " := CALL ");
			printOp(p->ops.op_two[1], fp);
			break;
		}
		case 13:{
			fprintf(fp, "PARAM ");
			printOp(p->ops.op_one, fp);
			break;
		}
		case 14:{
			fprintf(fp, "READ ");
			printOp(p->ops.op_one, fp);
			break;
		}
		case 15:{
			fprintf(fp, "WRITE ");
			printOp(p->ops.op_one, fp);
			break;
		}
	}
	fprintf(fp, "\n");
}

void printOp(Operand op, FILE *fp){
	if(op == NULL)
		return;
	if(op->addr == 1){
		fprintf(fp, "&");
	}
	else if(op->addr == 2){
		fprintf(fp, "*");
	}
	switch(op->kind){
		case 0:{
			fprintf(fp, "v%d", op->u.no);
			break;
		}
		case 1:{
			fprintf(fp, "#%d", op->u.value);
			break;
		}
		case 2:{
			fprintf(fp, "t%d", op->u.no);
			break;
		}
		case 3:{
			fprintf(fp, "label%d", op->u.no);
			break;
		}
		case 4:{
			fprintf(fp, "%s", op->u.name);
			break;
		}
	}
	//printf("\nendnd===\n");
}

void printOP(Operand op){
	if(op->addr == 1){
		printf("&");
	}
	else if(op->addr == 2){
		printf("*");
	}
	switch(op->kind){
		case 0:{
			printf("v%d", op->u.no);
			break;
		}
		case 1:{
			printf("#%d", op->u.value);
			break;
		}
		case 2:{
			printf("t%d", op->u.no);
			break;
		}
		case 3:{
			printf("label%d", op->u.no);
			break;
		}
		case 4:{
			printf("%s", op->u.name);
			break;
		}
	}
}

void printRelop(int type, FILE *fp){
	switch(type){
		case 0:{
			fprintf(fp, " == ");
			break;
		}
		case 1:{
			fprintf(fp, " != ");
			break;
		}
		case 2:{
			fprintf(fp, " > ");
			break;
		}
		case 3:{
			fprintf(fp, " < ");
			break;
		}
		case 4:{
			fprintf(fp, " >= ");
			break;
		}
		case 5:{
			fprintf(fp, " <= ");
			break;
		}
	}
}

Operand new_temp(){
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->addr = 0;
	op->kind = 2;
	op->u.no = temp_No++;
	return op;
}

Operand new_var(){
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->addr = 0;
	op->kind = 0;
	op->u.no = var_No++;
	op->argument = true;
	return op;
}

Operand new_label(){
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->addr = 0;
	op->kind = 3;
	op->u.no = label_No++;
	return op;
}

Operand ir_num(int num){
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->addr = 0;
	op->kind = 1;
	op->u.value = num;
	return op;
}

void ir_newCode(int flag, ...){
	InterCode newcode = (InterCode)malloc(sizeof(struct InterCode_));
	newcode->kind = flag;
	va_list pArg;
	va_start(pArg, flag);
	switch(flag){
		case 0: case 1: case 7: case 9: case 11: case 13: case 14: case 15: {
			//newcode->ops.op_one = (Operand)malloc(sizeof(struct Operand_));
			newcode->ops.op_one = va_arg(pArg, Operand);
			break;
		}
		case 2: case 12: {
			for(int i = 0;i < 2; i++)
				newcode->ops.op_two[i] = va_arg(pArg, Operand);
			break;
		}
		case 3: case 4: case 5: case 6:{
			for(int i = 0;i < 3; i++)
				newcode->ops.op_three[i] = va_arg(pArg, Operand);
			break;		
		}
		case 8:{ //args relop_type 
			for(int i = 0;i < 3; i++)
				newcode->ops.op_three[i] = va_arg(pArg, Operand);
			newcode->u.relop_type = va_arg(pArg, int);
			break;
		}
		case 10: { //array size
			newcode->ops.op_one = va_arg(pArg, Operand);
			newcode->u.size = va_arg(pArg, int);
			break;
		}
	}
	ir_add(newcode);
	//ir_print();
}

void ir_add(InterCode newnode){
	if(ir_list == NULL){
		ir_list = newnode;
		newnode->prev = NULL;
		newnode->next = NULL;
	}
	else {
		InterCode p = ir_list;
		while(p->next != NULL)
			p = p->next;
		p->next = newnode;
		newnode->prev = p;
		newnode->next = NULL;
	}
	tail = newnode;
}
