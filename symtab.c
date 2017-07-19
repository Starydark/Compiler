#include "symtab.h"

void traversal(Node *root){
	
	//StatementNum = 0;
	if(root == NULL)
		return;
	int type = root->type;
	//printf("%d\n", type);
	switch(type){
		//ExtDef
		case 29: {
			traversal_ExtDef(root);
			break;
		}
		default: {
			for(int i = 0;i < root->childrenNum; i++){
				traversal(root->children[i]);
			}		
		}
	}
}

void traversal_ExtDef(Node *root){
	int type = root->children[1]->type;
	//printf("%d\n", type);
	switch(type){
		//ExtDef -> Specifier SEMI
		case 1:{
			trav_SpecSemi(root->children[0]);
			break;
		}
		//ExtDef -> Specifier ExtDecList SEMI
		case 30: {
			//int vartype = getType(root->children[0]);
			trav_ExtDecList(root->children[0], root->children[1]);
			break;
		}
		//ExtDef -> Specifier FunDec CompSt
		case 36: {
			
			trav_Func(root);
			break;
		}
	}
}

//ExtDef -> Specifier FunDec CompSt
void trav_Func(Node *root){
	//printf("trav_Func start\n");
	hashnode *newnode = (hashnode *) malloc(sizeof(hashnode) );
	//函数返回值
	int funcType = getType(root->children[0]);
	newnode->type = (Type) malloc(sizeof(struct Type_));
	if(funcType == TYPEOFSTRUCTURE){
		trav_SpecSemi(root->children[0]);
		newnode->type->u.func.structName = (char*) malloc(sizeof(char) * 50);
		strcpy(newnode->type->u.func.structName, getStructName(root->children[0]));
		//printf("Aend\n");
	}
		
	if(root->children[2]->type == 1){
		newnode->type->kind = 5;
		//newnode->type->u.func.line = root->line;		
		//newnode->type->u.func.index = -1;
	}
	else
		newnode->type->kind = 4;
	newnode->type->u.func.type = funcType;
	newnode->type->u.func.param = NULL;
	//函数名
	trav_FuncDec(root->children[1], newnode);
	
	//Todo: hash_add

	funcstatement(newnode, root->line);	

	//printf("trav_Func end\n");
	
	//1代表是SEMI
	if(root->children[2]->type != 1){
		trav_CompSt(root->children[2], newnode);
		//printf("trav_Func end\n");
	}
	
}

void funcstatement(hashnode* newnode, int line){
	unsigned int index = hash_pjw(newnode->name);
	bool add = false;
	int index1;
	if(hashtable[index] == NULL && newnode->type->kind == 4) {
		hash_add(newnode);
		return;
	}
	else if(hashtable[index] == NULL && newnode->type->kind == 5){
		//printf("shengming%s\n", newnode->name);
		newnode->type->u.func.index = StatementNum;
		linetable[StatementNum++] = line;
		flagNum ++;
		hash_add(newnode);
		//printf("%d\t%d\n", flagNum, linetable[StatementNum-1]);
		return;
	}	
	else {
		hashnode *p = hashtable[index];
		while(p != NULL){
			if(strcmp(newnode->name, p->name) == 0){
				//函数重定义
				if(newnode->type->kind == 4 && p->type->kind == 4){
					printf("Error type 4 at Line %d: Redefined function \"%s\"\n", line, newnode->name);
					return;
				}
				//声明与定义
				else if(newnode->type->kind == 4 && p->type->kind == 5){
					if(!compareFuncparamList(newnode->type, p->type)) {
						printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\"\n", line, newnode->name);
						return;
					}
					else {
						index1 = p->type->u.func.index;
						add = true;
					}
				}
				//声明与定义
				else if(newnode->type->kind == 5 && p->type->kind == 4){
					if(!compareFuncparamList(newnode->type, p->type)) {
						printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\"\n", line, newnode->name);
						return;					
					}
					else {
						//index1 = newnode->type->u.func.index;
						add = true;
					}
				}
				//都是声明
				else if(newnode->type->kind == 5 && p->type->kind == 5){
					//声明冲突
					if(!compareFuncparamList(newnode->type, p->type)) {
						printf("Error type 19 at Line %d: Inconsistent declaration of function \"%s\"\n", line, newnode->name);
						return;
					}
					
				}
			}
			p = p->next;
		}
	}
	
	if(newnode->type->kind == 5 && !add) {
		newnode->type->u.func.index = StatementNum;
		linetable[StatementNum++] = line;
		flagNum ++;
		//printf("shengming%s\n", newnode->name);
	}
	else if(newnode->type->kind == 4 && add){
		linetable[index1] = -1;
		flagNum --;
		//printf("dingyi%s\n", newnode->name);
	}
	hash_add(newnode);	
}

bool compareFuncparamList(Type func1, Type func2){
	bool Notsame = true;	
	if(func1->u.func.type != func2->u.func.type){
		//printf("%d\t%d\n", func1->u.func.type, func2->u.func.type);		
		Notsame = false;
	}
	
	else {
		FuncparamList list1 = func1->u.func.param;
		FuncparamList list2 = func2->u.func.param;
		while(list1 != NULL && list2 != NULL){
			if(list1->type->kind != list2->type->kind) {		
				Notsame = false;
			}
			
			else if(list1->type->kind == 1) {
				if(strcmp(list1->type->u.structname, list2->type->u.structname) != 0){
					Notsame = false;
				}
			}
			else if(list1->type->kind == 2){
				Type p1 = list1->type;
				Type p2 = list2->type;
				while(p1->kind == 2 && p2->kind == 2){
					if(p1->u.array.size != p2->u.array.size){
						Notsame = false;
						break;
					}
					p1 = p1->u.array.elem;
					p2 = p2->u.array.elem;
				}
				if(p1->kind ==2 || p1->kind == 2){
					Notsame = false;
				}
				else if(p1->kind != p2->kind){
					Notsame = false;
				}
				else if(p1->kind == 0 && p1->u.basic != p2->u.basic){
					Notsame = false;
				}
			}
			//printf("%d\t%d\n", list1->type->kind, list2->type->kind);
			list1 = list1->next;
			list2 = list2->next;
		}
		if(list1 != NULL || list2 != NULL)
			Notsame = false;
	}
	return Notsame;
}

void trav_CompSt(Node* compst, hashnode *newnode){
	//printf("111\n");
	trav_DefList(compst->children[1], newnode);
	trav_StmtList(compst->children[2], newnode);
	//printf("Aend\n");
}

void trav_StmtList(Node *stmtlist, hashnode *newnode){
	if(stmtlist == NULL)
		return;
	trav_Stmt(stmtlist->children[0], newnode);
	trav_StmtList(stmtlist->children[1], newnode);
}

void trav_Stmt(Node *stmt, hashnode *newnode){
	int childrenNum = stmt->childrenNum;
	//printf("stmtA\n");
	//printf("%d\n", childrenNum);
	switch(childrenNum){
		case 1:{
			trav_CompSt(stmt->children[0], newnode);
			break;
		}	
		case 2:{
			//printf("fdsa\n");
			trav_Exp(stmt->children[0], NULL);
			//printf("trav_Func end\n");
			break;
		}
		//return Exp SEMI
		case 3:{
			int type = getExpType(stmt->children[1]);
			//printf("%d\n", type);
			if(type == -1)
				break;
			if(type != newnode->type->u.func.type){
				printf("Error type 8 at Line %d: Type mismatched for return.\n", stmt->line);	
				return;		
			}
			//printf("trav_Func end\n");
			Operand t1 = new_temp();
			trav_Exp(stmt->children[1], t1);
			ir_newCode(9, t1);
			break;
		}
		case 5: {
			//分析条件语句类型
			int type = stmt->children[0]->type;
			if(type == 19){//if
				Operand lab1 = new_label();
				translate_Cond(stmt->children[2], NULL, lab1);
				trav_Stmt(stmt->children[4], newnode);
				ir_newCode(0, lab1);
			}
			else if(type == 21){ //while
				Operand lab1 = new_label();
				Operand lab2 = new_label();
				ir_newCode(0, lab1);
				translate_Cond(stmt->children[2], NULL, lab2);
				trav_Stmt(stmt->children[4], newnode);
				ir_newCode(7, lab1);
				ir_newCode(0, lab2);
			}
			//trav_Exp(stmt->children[2], NULL);
			//trav_Stmt(stmt->children[4], newnode);
			break;
		}
		case 7: {
			//分析条件语句类型
			Operand lab1 = new_label();
			translate_Cond(stmt->children[2], NULL, lab1);
			trav_Stmt(stmt->children[4], newnode);
			Operand lab2 = new_label();
			ir_newCode(7, lab2);
			ir_newCode(0, lab1);
			trav_Stmt(stmt->children[6], newnode);
			ir_newCode(0, lab2);
			break;
		}
	}
}

void translate_Exp(Node* exp, Operand op){ //Exp RELOP/And/OR Exp
	//if(op)
	//Operand lab1 = new_label();
	Operand lab2 = new_label();
	Operand t2 = ir_num(0);
	if(op)
		ir_newCode(2, op, t2);
	translate_Cond(exp, NULL, lab2);
	//ir_newCode(0, lab1);
	Operand t3 = ir_num(1);
	if(op)
		ir_newCode(2, op, t3);
	ir_newCode(0, lab2);
}

int get_relop(Node *exp){
	if(strcmp(exp->name.content, "==") == 0)
		return 0;
	else if(strcmp(exp->name.content, "!=") == 0)
		return 1;
	else if(strcmp(exp->name.content, ">") == 0)
		return 2;
	else if(strcmp(exp->name.content, "<") == 0)
		return 3;
	else if(strcmp(exp->name.content, ">=") == 0)
		return 4;
	else if(strcmp(exp	->name.content, "<=") == 0)
		return 5;			
}

void translate_Cond(Node *exp, Operand label_true, Operand label_false){
	if(exp->childrenNum == 1){
		translate_else(exp, label_true, label_false);
	}
	else {
		int type = exp->children[1]->type;
	//printf("%d\n", type);
	switch(type){
		case 18:{//Exp Relop Exp
			//printf("%s\n", exp->children[0]->children[0]->name.content);
			Operand t1 = new_temp();
			trav_Exp(exp->children[0], t1);
			//printOp(t1);
			Operand t2 = new_temp();
			trav_Exp(exp->children[2], t2);
			int relop_type = get_relop(exp->children[1]);
			if(label_true && label_false){
				ir_newCode(8, t1, t2, label_true, relop_type);
			}
			else if(label_true){
				ir_newCode(8, t1, t2, label_true, relop_type);
			}
			else if(label_false){
				ir_newCode(8, t1, t2, label_false, (7-relop_type)%6);
			}
			//printf("end\n");
			break;
		}
		case 8: {//And
			if(label_false){
				translate_Cond(exp->children[0], NULL, label_false);
				translate_Cond(exp->children[2], label_true, label_false);
			}
			else{
				Operand label1 = new_label();
				translate_Cond(exp->children[0], NULL, label1);
				translate_Cond(exp->children[2], label_true, label_false);
				ir_newCode(0 , label1);
			}
			break;
		}
		case 9:{//Or
			if(label_true){
				translate_Cond(exp->children[0], label_true, NULL);
				translate_Cond(exp->children[2], label_true, label_false);
			}
			else {
				Operand label1 = new_label();
				translate_Cond(exp->children[0], label1, NULL);
				translate_Cond(exp->children[2], label_true, label_false);
				ir_newCode(0 , label1);
			}
			break;
		}
		case 46:{//Not Exp
			if(exp->children[0]->type == 11){
				translate_Cond(exp->children[1], label_false, label_true);			
			}
			else 
				translate_else(exp, label_true, label_false);
			break;
		}
		default:{
			translate_else(exp, label_true, label_false);
			/*
			Operand t1 = new_temp();
			trav_Exp(exp, t1);
			if(label_true && label_false){
				ir_newCode(8, t1, ir_num(0), label_true, 1);
				ir_newCode(7, label_false);
			}
			else if(label_true){
				ir_newCode(8, t1, ir_num(0), label_true, 1);
			}
			else if(label_false){
				ir_newCode(8, t1, ir_num(0), label_false, 0);
			}
			*/
		}
	}
	}	
}

void translate_else(Node *exp, Operand label_true, Operand label_false){
	Operand t1 = new_temp();
	trav_Exp(exp, t1);
	if(label_true && label_false){
		ir_newCode(8, t1, ir_num(0), label_true, 1);
		ir_newCode(7, label_false);	
	}
	else if(label_true){
		ir_newCode(8, t1, ir_num(0), label_true, 1);
	}
	else if(label_false){
		ir_newCode(8, t1, ir_num(0), label_false, 0);
	}
}

void trav_Exp(Node *exp, Operand op){
	//printf("Astart\t");
	int childrenNum = exp->childrenNum;
	//printf("opz%d%d\n", op->kind, op->u.no);
	switch(childrenNum){
		case 1: {
			//Exp->ID
			//printf("%d\n", exp->children[0]->type);
			if(exp->children[0]->type == 26){
				hashnode* index = hash_lookup(exp->children[0]->name.content, 0);
				if(NULL == index){
					printf("Error type 1 at Line %d: Undefined variable %s\n", exp->line, exp->children[0]->name.content);
					break;
				}
				else {
					if(op && index->operand){						
						clean_temp(op, index->operand);						
					}
					else if(op){
						index->operand = new_var();
						clean_temp(op, index->operand);
					}
					if(index->type->kind == 2 && index->operand->argument == true){
						op->addr = 1;
					}
				}
			}
			else if(exp->children[0]->type == 24) { //Exp->INT
				if(op){
					Operand t1 = ir_num(exp->children[0]->name.intValue);
					ir_newCode(2, op, t1);
				}
			}
			break;
		}
		case 2: {
			//Exp->Minux/Not Exp
			if(exp->children[0]->type == 5){
				Operand t1 = new_temp();
				trav_Exp(exp->children[1], t1);
				Operand t2 = ir_num(0);
				ir_newCode(4, op, t2, t1);
			}
			else {
				translate_Exp(exp->children[1], op);
			}
			break;
		}
		case 3: {
		
			int type = exp->children[1]->type;
			//printf("%d\n", type);
			//Exp->LP Exp RP
			if(type == 46){
				trav_Exp(exp->children[1], op);
			}
			//Exp->ID LP RP
			else if(type == 12){
				if(strcmp(exp->children[0]->name.content, "read") == 0){
					ir_newCode(14, op);
					return;
				}
				if(NULL == hash_lookup(exp->children[0]->name.content, 4)){
					if(hash_lookup(exp->children[0]->name.content, 0) != NULL){
						printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp->line, exp->children[0]->name.content);
					}
					else
						printf("Error type 2 at Line %d: Undefined function \"%s\"\n", exp->line, exp->children[0]->name.content);
				}
				else {
					Operand t = (Operand)malloc(sizeof(struct Operand_));
					t->addr = 0;
					t->kind = 4;
					t->u.name = (char*)malloc(sizeof(char) * 50);
					strcpy(t->u.name, exp->children[0]->name.content);
					if(op == NULL)
						op = new_temp();
					ir_newCode(12, op, t);
				}
			}
			//Exp->Exp Dot ID
			else if(type == 10){
				int type = getExpType(exp->children[0]);
				if(type == -1)
					break;
				if(type != TYPEOFSTRUCTURE){
					printf("Error type 13 at Line %d: Illegal use of \".\".\n", exp->line);
				}
				else if(!ExistField(exp->children[0], exp->children[2]->name.content)){
					printf("Error type 14 at Line %d: Non-existent field \"%s\"\n", exp->line, exp->children[2]->name.content);
				}
			}
			//Exp->Exp +-*/=&&|| Exp
			else {
				
				if(exp->children[1]->type == 3){
					if(!left_hand(exp)){
						printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", exp->line);
						return;
					}	
				}
				int type1 = getExpType(exp->children[0]);	
				int type2 = getExpType(exp->children[2]);
				if(type1 == -1 || type2 == -1)
					break;
				if(type1 != type2){
					//assign Op
					if(exp->children[1]->type == 3)
						printf("Error type 5 at Line %d: Type mismatched for assignment.\n", exp->line);
					else
						printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp->line);
				}
				else {		
				
					if(type1 ==TYPEOFSTRUCTURE ) {
						if( !matchStruct(exp->children[0], exp->children[2]))
							return;
					}	
					//printf("Astart%d\n", type);
					switch(type){
						case 3: {//assign
							Operand dst = new_temp();
							//hashnode *index = hash_lookup(exp->children[0]->children[0]->name.content, 0);
							trav_Exp(exp->children[0], dst);
							Operand t1 = new_temp();
							trav_Exp(exp->children[2], t1);
							//if(index->operand == NULL){
							//	index->operand = new_var();
							//}
							ir_newCode(2, dst, t1);
							if(op)
								ir_newCode(2, op, dst);	
							else {
								
							}
							break;			
						}
						case 4: case 5: case 6: case 7:{//+-*/
							Operand t1 = new_temp();
							trav_Exp(exp->children[0], t1);
							Operand t2 = new_temp();
							trav_Exp(exp->children[2], t2);
							if(op == NULL)
								return;
							ir_newCode(type - 1, op, t1, t2);
							break;
						}
						case 8: case 9: case 18:{//And/or/Relop
							translate_Exp(exp, op);
							break;
						}
						
					}
					//trav_Exp(exp->children[0], NULL);
					//trav_Exp(exp->children[2], NULL);
				}
			}
			break;
		}
		case 4: {
			//Exp->ID LP Args RP
			if(exp->children[0]->type == 26){
				//printf("%s\n", exp->children[0]->name.content);	
				if(strcmp(exp->children[0]->name.content, "write") == 0){
					//printf("Astart\n");
					translate_Args(exp->children[2], 0);
					//printf("Aend\n");
					return;
				}
				hashnode* getnode = hash_lookup(exp->children[0]->name.content, 4);
				if(NULL == getnode){
					if(hash_lookup(exp->children[0]->name.content, 0) != NULL){
						printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp->line, exp->children[0]->name.content);
					}
					else
						printf("Error type 2 at Line %d: Undefined function \"%s\"\n", exp->line, exp->children[0]->name.content);
					return;
				}
				else {
					//if(exp->children[2]->childrenNum != )
					unsigned int index = hash_pjw(exp->children[0]->name.content);
					hashnode *q = hashtable[index];		
					while(q != NULL){
						if(strcmp(q->name, exp->children[0]->name.content) == 0){
							if(q->type->kind == 4)
								break;
						}
						q = q->next;
					}
					
					FuncparamList arr = q->type->u.func.param;
					
					Node *real = exp->children[2];		

					while(arr != NULL){
						//不匹配
						if(!match(arr->type, real->children[0])){
							printf("Error type 9 at Line %d: Function is not applicable for arguments\n", exp->line); 
							return;
						}
						arr = arr->next;
						//实参过多
						if(arr == NULL && real->childrenNum > 1){
							printf("Error type 9 at Line %d: Function is not applicable for arguments\n", exp->line);
							return;
						}
						if(real->childrenNum > 1)
							real = real->children[2];
						//实参少
						else if(arr != NULL){
							printf("Error type 9 at Line %d: Function is not applicable for arguments\n", exp->line);
							return;
						}				
					}
					
					translate_Args(exp->children[2], 1);
					Operand func = (Operand)malloc(sizeof(struct Operand_));
					func->addr = 0;
					func->kind = 4;
					func->u.name = (char*)malloc(sizeof(char) * 50);
					strcpy(func->u.name, getnode->name);
					if(op == NULL)
						op = new_temp();
					ir_newCode(12, op, func);					
				}
				
			}
			//Exp->Exp LB Exp RB
			else if(exp->children[0]->type == 46){
				int type1 = getExpType(exp->children[0]);				
				if(type1 == -1)
					return;
				if(type1 != TYPEOFARRAY){
					printf("Error type 10 at Line %d: is not array.\n",exp->line);
					return;			
				}
				int type = getExpType(exp->children[2]);
				if(type == -1)
					break;
				if(type != TYPEOFINT){
					printf("Error type 12 at Line %d: is not an integer.\n",exp->line);
					return;
				}
				translate_Arr(exp, op);
			}
			break;
		}
	}
	//printf("Aendn\n");
}

void translate_Arr(Node *exp, Operand op){
	Node** expArr = (Node **)malloc(sizeof(Node*) * 20);
	int index = 0;
	Node *name = exp;
	while(name->childrenNum == 4){
		expArr[index++] = name;
		name = name->children[0];
	}
	hashnode *indexInHash = hash_lookup(name->children[0]->name.content, 0);
	if(indexInHash == NULL)
		return;
	index --;
	
	Type arrayList = indexInHash->type;
	Operand address = (Operand)malloc(sizeof(struct Operand_));
	address->kind = 0;	
	address->u.no = indexInHash->operand->u.no;
	if(indexInHash->operand->argument == true){
		address->addr = 1;	
	}
	else {
		address->addr = 0;
	}
	Operand temp = new_temp();
		
	if(index >= 0){
		arrayList = arrayList->u.array.elem;
		Operand offset = new_temp();
		trav_Exp(expArr[index]->children[2], offset);		
		int size = getSize(arrayList);
		if(offset->kind == 0){			
			Operand offset1 = new_temp();
			ir_newCode(5, offset1, offset, ir_num(size));
			ir_newCode(3, temp, address, offset1);
		}
		else {
			Operand temp1 = new_temp();
			ir_newCode(5, temp1, offset, ir_num(size));
			ir_newCode(3, temp, address, temp1);
		}
		index --;
	}	
	while(index >= 0){
		arrayList = arrayList->u.array.elem;
		Operand offset = new_temp();
		trav_Exp(expArr[index]->children[2], offset);
		int size = getSize(arrayList);
		if(offset->kind == 0){
			Operand offset1 = new_temp();
			ir_newCode(5, offset1, offset, ir_num(size));
			Operand temp2 = new_temp();
			ir_newCode(3, temp2, temp, offset1);
			ir_newCode(2, temp, temp2);
		}
		else {
			Operand temp3 = new_temp();
			ir_newCode(5, temp3, offset, ir_num(size));
			ir_newCode(2, offset, temp3);
			Operand temp2 = new_temp();
			ir_newCode(3, temp2, temp, offset);
			ir_newCode(2, temp, temp2);
		}
		index --;
	}
	op->addr = 2;
	op->kind = temp->kind;
	op->u.no = temp->u.no;
}

void translate_Args(Node *exp, int flag){//flag: wirte
	if(exp->childrenNum == 3){	
		translate_Args(exp->children[2], flag);
	}
	Operand t1 = new_temp();
	trav_Exp(exp->children[0], t1);
	
	//printOP(t1);
	int type = getExpType(exp->children[0]);
	if(type == TYPEOFARRAY && t1->kind == 2){
		t1->addr = 0;
	}
	
	if(flag == 0)
		ir_newCode(15, t1);
	else 
		ir_newCode(11, t1);
	 
}

bool left_hand(Node *exp){
	Node *left = exp->children[0];
	bool flag = false;		
	//左值的三种case
	//ID		
	if(left->childrenNum == 1 && left->children[0]->type == 26){
		flag = true;
	}
	//Exp DOT ID
	else if(left->childrenNum == 3 && left->children[1]->type == 10 ) {
		flag = true;		
	}
	//Exp LB Exp RB
	else if(left->childrenNum == 4 && left->children[1]->type == 14 )
		flag = true;
					
	return flag;
}

bool matchStruct(Node *exp1, Node *exp2)
{
	Node *structVar1 = exp1->children[0];
	Node *structVar2 = exp2->children[0];
	int index1 = hash_pjw(structVar1->name.content);
	hashnode *q1 = hashtable[index1];
	int index2 = hash_pjw(structVar2->name.content);
	hashnode *q2 = hashtable[index2];	
	while(q1 != NULL){
		if(strcmp(q1->name, structVar1->name.content) == 0){
			break;
		}
		q1 = q1->next;
	}
	while(q2 != NULL){
		if(strcmp(q2->name, structVar2->name.content) == 0){
			break;
		}
		q2 = q2->next;
	}
	
	int temp1 = hash_pjw(q1->type->u.structname);
	hashnode *Node1 = hashtable[temp1];
	int temp2 = hash_pjw(q2->type->u.structname);
	hashnode *Node2 = hashtable[temp2];
	while(Node1 != NULL){
		if(strcmp(Node1->name, q1->type->u.structname) == 0){
			break;
		}
		Node1 = Node1->next;
	}
	while(Node2 != NULL){
		if(strcmp(Node2->name, q2->type->u.structname) == 0){
			break;
		}
		Node2 = Node2->next;
	}
	
	if( strcmp(Node1->name, Node2->name) != 0) {
		printf("Error type 5 at Line %d: Type mismatched for assignment.\n", exp1->line);
	}
	return true;
}

int getExpType(Node* exp){
	int childrenNum = exp->childrenNum;
	
	switch(childrenNum){
		case 1: {			
			//Exp->ID
			if(exp->children[0]->type == 26){
				if(NULL == hash_lookup(exp->children[0]->name.content, 0)){
					printf("Error type 1 at Line %d: Undefined variable %s\n", exp->line, exp->children[0]->name.content);
					return -1;
				}
				unsigned int index = hash_pjw(exp->children[0]->name.content);
				hashnode *q = hashtable[index];		
				while(q != NULL){
					if(strcmp(q->name, exp->children[0]->name.content) == 0){
						if(q->type->kind == 0){
							return q->type->u.basic;
						}
						else if(q->type->kind == 1)
							return TYPEOFSTRUCTURE;
						else if(q->type->kind == 2)
							return TYPEOFARRAY;
						else if(q->type->kind == 3)
							return 5;
					}
					q = q->next;
				}
				
			}
			else if(exp->children[0]->type == 25) {
				return TYPEOFFLOAT;
			}
			else if(exp->children[0]->type == 24) {
				return TYPEOFINT;
			}
			break;
		}
		case 2: {
			return getExpType(exp->children[1]);
			break;
		}		
		case 3: {
			int type = exp->children[1]->type;
			//Exp->LP Exp RP
			if(type == 46){
				return getExpType(exp->children[1]);
			}
			else if(type == 12){
				if(strcmp("read", exp->children[0]->name.content) == 0)
					return TYPEOFINT;
				if(NULL == hash_lookup(exp->children[0]->name.content, 4)){
					if(hash_lookup(exp->children[0]->name.content, 0) != NULL){
						printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp->line, exp->children[0]->name.content);
					}
					else
						printf("Error type 2 at Line %d: Undefined function \"%s\"\n", exp->line, exp->children[0]->name.content);
					
				}
				else {
					unsigned int index = hash_pjw(exp->children[0]->name.content);
					hashnode *q = hashtable[index];		
					while(q != NULL){
						if(strcmp(q->name, exp->children[0]->name.content) == 0){
							if(q->type->kind == 4)
								break;
						}
						q = q->next;
					}
					
					FuncparamList arr = q->type->u.func.param;
					int functype = q->type->u.func.type;
					return functype;
				}
			}	
			//Exp->Exp Dot ID
			else if(type == 10){
				int type = getExpType(exp->children[0]);
				if(type == -1)
					return -1;
				if(type != TYPEOFSTRUCTURE){
					printf("Error type 13 at Line %d: Illegal use of \".\".\n", exp->line);
					return -1;
				}	
				else if(!ExistField(exp->children[0], exp->children[2]->name.content)){
					printf("Error type 14 at Line %d: Non-existent field \"%s\"\n", exp->line, exp->children[2]->name.content);
					return -1;
				}			
				unsigned int index = hash_pjw(exp->children[2]->name.content);
				hashnode *q = hashtable[index];		
				while(q != NULL){
					
					if(strcmp(q->name, exp->children[2]->name.content) == 0){
						if(q->type->kind == 0){
							return q->type->u.basic;
						}
						else if(q->type->kind == 1)
							return TYPEOFSTRUCTURE;
						else if(q->type->kind == 2)
							return TYPEOFARRAY;
					}
					q = q->next;
				}
				
				
			}
			//Exp->Exp +-*/=&&|| Exp
			else {
				
				int type1 = getExpType(exp->children[0]);	
				int type2 = getExpType(exp->children[2]);
				if(type1 == -1 || type2 == -1)
					break;		
				if(type1 != type2){
					//assignOp
					if(exp->children[1]->type == 3) {
						printf("Error type 5 at Line %d: Type mismatched for operands.\n", exp->line);
						return -1;
					}
					else {
						printf("Error type 7 at Line %d: Type mismatched for operands.\n", exp->line);
						return -1;
					}
				}
				else {
					return type1;
				}
			}
			break;
		}
		case 4: {
			//printf("%s\n", exp->children[0]->name.content);
			//Exp->Exp LB Exp RB
			if(exp->children[0]->type == 46){
				//printf("%s\n", exp->children[0]->name.content);
				int type1 = getExpType(exp->children[0]);
				if(type1 == -1)
					break;
				if(type1 != TYPEOFARRAY){
					printf("Error type 10 at Line %d: is not array.\n",exp->line);
					return -1;		
				}
				int type = getExpType(exp->children[2]);
				if(type == -1) {
					return -1;
				}
				if(type != TYPEOFINT){
					printf("Error type 12 at Line %d: is not an integer.\n",exp->line);
					return -1;
				}
				return getTypeOfArray(exp);
			}
			//Exp->ID LP args RP
			else if(exp->children[0]->type == 26) {
				
				if(strcmp(exp->children[0]->name.content, "write") == 0){
					//translate_Args(exp->children[2], 0);
					return 0;
				}				
				//if(strcmp("write", ))
				if(NULL == hash_lookup(exp->children[0]->name.content, 4)){			
					if(hash_lookup(exp->children[0]->name.content, 0) != NULL){
						printf("Error type 11 at Line %d: \"%s\" is not a function.\n", exp->line, exp->children[0]->name.content);
					}
					else
						printf("Error type 2 at Line %d: Undefined function \"%s\"\n", exp->line, exp->children[0]->name.content);
				}
				else {
					//if(exp->children[2]->childrenNum != )
					unsigned int index = hash_pjw(exp->children[0]->name.content);
					hashnode *q = hashtable[index];		
					while(q != NULL){
						if(strcmp(q->name, exp->children[0]->name.content) == 0){
							if(q->type->kind == 4)
								break;
						}
						q = q->next;
					}
					
					FuncparamList arr = q->type->u.func.param;
					int functype = q->type->u.func.type;
					Node *real = exp->children[2];		

					while(arr != NULL){
						//不匹配
						if(!match(arr->type, real->children[0])){
							printf("Error type 9 at Line %d: Function is not applicable for arguments\n", exp->line); 
							return -1;
						}
						arr = arr->next;
						//实参过多
						if(arr == NULL && real->childrenNum > 1){
							printf("Error type 9 at Line %d: Function is not applicable for arguments\n", exp->line);
							return -1;
						}
						if(real->childrenNum > 1)
							real = real->children[2];
						//实参少
						else if(arr != NULL){
							printf("Error type 9 at Line %d: Function is not applicable for arguments\n", exp->line);
							return -1;
						}				
					}
					//printf("%d\n", functype);
					return functype;
				}
				
			}
			break;
		}
	}
	return -1;
}

int getTypeOfArray(Node *exp){
	int num = 0;
	Node *p = exp;
	while(p->childrenNum == 4){
		num ++;
		p = p->children[0];
	}
	if(p->childrenNum == 3){
		p = p->children[2];
	}
	else if(p->childrenNum == 1)
		p = p->children[0];
	//printf("%s\n", p->name.content);
	unsigned int index = hash_pjw(p->name.content);
	hashnode *q = hashtable[index];	
	while(q != NULL){
		//printf("%s\n", p->children[0]->name.content);
		if(strcmp(q->name, p->name.content) == 0){
			if(q->type->kind == 2)
				break;
		}
		q = q->next;
	}
	Type temp = q->type;	
	while(num > 0){
		temp = temp->u.array.elem;
		num --;
	}
	if(temp->kind == 0)
		return temp->u.basic;
	else if(temp->kind == 1)
		return TYPEOFSTRUCTURE;
	else if(temp->kind == 2)
		return TYPEOFARRAY;
}

bool match(Type virtual, Node *exp){
	//int kind = virtual->kind;
	int virtualType;
	if(virtual->kind == 0){
		virtualType = virtual->u.basic;
	}
	else if(virtual->kind == 1)
		virtualType = TYPEOFSTRUCTURE;
	else if(virtual->kind == 2) {
		virtualType = TYPEOFARRAY;
	}
	
	int realType = getExpType(exp);
	if(realType != virtualType)
		return false;
	else if(realType == TYPEOFSTRUCTURE){
		//printf("Astar\n");
		
		Node *structname = exp;
		//printf("%s\n",structname->children[0]->name.content);
		int flag = 1;
		while(structname->childrenNum != 1 && structname->childrenNum != 3 && structname->childrenNum != 0){			
			structname = structname->children[0];
			//printf("\n%d\n", structname->childrenNum);
		}
		//printf("%s\n",structname->name.content);
		if(structname->childrenNum == 1) {
			flag = 0;
			structname = structname->children[0];
		}
		else if(structname->childrenNum == 3){
			//flag = 2;
			structname = structname->children[2];
		}
		//printf("%s\n",structname->children[0]->name.content);
		unsigned int index = hash_pjw(structname->name.content);
		hashnode *q = hashtable[index];	
		while(q != NULL){
			if(strcmp(q->name, structname->name.content) == 0){				
				if(flag == 0 && q->type->kind == 1 || q->type->kind == 2) {
					Type array = q->type;		
					while(array->kind == 2)
						array = array->u.array.elem;
					if(strcmp(array->u.structname, virtual->u.structname) != 0)
						return false;
				}
				else if(flag == 1 && q->type->kind == 4){
					if(strcmp(q->type->u.func.structName, virtual->u.structname) != 0)	
						return false;
				}
			}
			q = q->next;
		}
	}
	return true;		
}

bool ExistField(Node *exp, char *name){
	unsigned int index;	
	char *structname = (char *)malloc(sizeof(char) * 50);
	if(exp->childrenNum == 1 || exp->childrenNum == 3){
		if(exp->childrenNum == 3) {
			if(!ExistField(exp->children[0], exp->children[2]->name.content)){
				return false;
			}
			//printf("%s\n", exp->children[2]->name.content);
			index = hash_pjw(exp->children[2]->name.content);
			strcpy(structname, exp->children[2]->name.content);
		}
		else {
			index = hash_pjw(exp->children[0]->name.content);
			strcpy(structname, exp->children[0]->name.content);
		}
		hashnode *q = hashtable[index];		
		while(q != NULL){
			if(strcmp(q->name, structname) == 0){
				if(q->type->kind == 1)
					break;
			}
			q = q->next;
		}
		strcpy(structname, q->type->u.structname);
		//printf("%s\n", structname);
	}	
	else if(exp->childrenNum == 4){
		Node *p = exp->children[0];
		while(p -> childrenNum != 1){
			p = exp->children[0];
		}
		index = hash_pjw(p->children[0]->name.content);
		hashnode *q = hashtable[index];		
		while(q != NULL){
			if(strcmp(q->name, p->children[0]->name.content) == 0){
				if(q->type->kind == 2)
					break;
			}
			q = q->next;
		}
		
		Type arr = q->type;
		while(arr->kind != 1){
			//printf("大小%d\t", arr->u.array.size);
			arr = arr->u.array.elem;
		}
		strcpy(structname, arr->u.structname);
	}
	index = hash_pjw(structname);
	hashnode *q = hashtable[index];		
	while(q != NULL){
		if(strcmp(q->name, structname) == 0){
			//printf("%s\n", q->name);
			if(q->type->kind == 3) {
				//printf("Bend\n");
				FieldList structfield = q->type->u.structrue;
				while(structfield != NULL){
					if(strcmp(structfield->name, name) == 0){
						//printf("Bend\n");		
						return true;
					}
					structfield = structfield->next;
				}
			}
		}
		q = q->next;
	}
	return false;
}

//FunDec -> ID LP VarList RP
//	| ID LP RP 
void trav_FuncDec(Node* funcdec, hashnode* newnode){
	newnode->name = (char *) malloc(sizeof(char) * 50);
	strcpy(newnode->name, funcdec->children[0]->name.content );
	
	//生成函数名的中间代码
	Operand op = (Operand)malloc(sizeof(struct Operand_));
	op->addr = 0;
	op->kind = 4;
	op->u.name = (char *)malloc(sizeof(char) * 50);
	strcpy(op->u.name, newnode->name);
	ir_newCode(1, op);
	
	if(funcdec->childrenNum == 3) {
		newnode->type->u.func.param = NULL;
	}
	else { //有形参VarList
		trav_VarList(funcdec->children[2], newnode);
	}
	
}

//VarList -> ParamDec COMMA VarList
//	| ParamDec
void trav_VarList(Node *varList, hashnode* newnode) {
	//printf("trav_VarList start\n");
	trav_ParamDec(varList->children[0], newnode);

	if(varList->childrenNum == 3) {
		trav_VarList(varList->children[2], newnode);
	}
	//printf("trav_VarList end\n");
}

//ParamDec -> Specifier VarDec
void trav_ParamDec(Node *ParamDec, hashnode *newnode) {
	//形参加入到hash table中
	//trav_VarDec(ParamDec->children[0], ParamDec->children[1]);
	
	hashnode *paramNode = (hashnode *) malloc(sizeof(hashnode) );
	newVar(ParamDec->children[0], ParamDec->children[1], paramNode);
	paramNode->next = NULL;	
	//判断参数名是否重复定义,4是指定义
	if(hash_lookup(paramNode->name, 0) != NULL && newnode->type->kind == 4){
		printf("Error type 3 at Line %d: Redefined variable \"%s\"\n", ParamDec->children[1]->line, paramNode->name);
		return;
	}
	else if(newnode->type->kind == 4) { 		
		hash_add(paramNode);
	}
	
	//生成param的中间代码
	Operand op = new_var();
	ir_newCode(13, op);
	if(paramNode->type->kind == 2)
		op->argument = false;
	paramNode->operand = op;
		
	//将形参链接到函数的hashnode后面
	FuncparamList fpl = (FuncparamList) malloc(sizeof(struct FuncparamList_));
	fpl->name = (char *)malloc(sizeof(char) *50);
	strcpy(fpl->name, paramNode->name);
	//fpl->type = (Type) malloc(sizeof(struct Type_) );
	fpl->type = paramNode->type;
	fpl->next =NULL;

	FuncparamList tail = newnode->type->u.func.param;
	if( tail == NULL){
		newnode->type->u.func.param = (FuncparamList) malloc(sizeof(struct FuncparamList_));
		tail = newnode->type->u.func.param;
		tail->type = fpl->type;
		newnode->type->u.func.param->name = (char *)malloc(sizeof(char) * 50);
		strcpy(tail->name, fpl->name);
		tail->next = NULL;
		//tail = fpl;
		//printf("|||||||%s\t%d\n", tail->name, tail->type->kind);
	}
	else {
		while( tail->next != NULL)
			tail = tail->next;
		tail->next = fpl;
		//printf("|||||||%s\t%d\n", tail->name, tail->type->kind);
	}
	
	//printf("trav_ParamDec end\n");
}

//ExtDef -> Specifier SEMI
void trav_SpecSemi(Node *type){
	hashnode* newnode = (hashnode *) malloc(sizeof(hashnode));
	int varType = getType(type);
	if(varType == TYPEOFSTRUCTURE){
		printf("Cannot translate: Code contains variables or parameters of strcture type\n");
		exit(0);
		Node* structname = type->children[0];
		if(structname->childrenNum == 5){
			
			FLAG ++;
			newnode->name = (char*)malloc(sizeof(char) * 50);
			Node *p = structname->children[1];
							
			if(p == NULL) {
				strcpy(newnode->name, Node_types[indexNum++]);
			}
			else
				strcpy(newnode->name, p->children[0]->name.content);
			
			trav_DefList(structname->children[3], newnode);
			FLAG --;
		}
		//StructSpecifier-> STRUCT Tag
		else {
			if(NULL == hash_lookup(structname->children[1]->children[0]->name.content, 0)){
				printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", type->line, structname->children[1]->children[0]->name.content);
			}
			else{
				int type1 = getExpType(structname->children[1]);
				if(type1 != 5)
					printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", type->line, structname->children[1]->children[0]->name.content);
			}
			return;
		}
	}
	else
		return;
	newnode->next = NULL;
	if(hash_lookup(newnode->name, 0) != NULL){
		printf("Error type 16 at Line %d: Duplicated name \"%s\"\n", type->children[0]->line, newnode->name);
	}
	else {
		hash_add(newnode);
	}
}

//DefList -> Def DefList
//	| null
void trav_DefList(Node *deflist, hashnode *newnode){
	if(deflist == NULL) {	
		return;
	}
	trav_Def(deflist->children[0], newnode);
	if(deflist->childrenNum == 2)
		trav_DefList(deflist->children[1], newnode);
}

//Def
void trav_Def(Node *def, hashnode* newnode) {
	trav_DecList(def->children[0], def->children[1], newnode);
}

void trav_DecList(Node *type, Node *declist, hashnode *newnode){
	trav_Dec(type, declist->children[0], newnode);
	if(declist->childrenNum == 3)
		trav_DecList(type, declist->children[2], newnode);
}

void trav_Dec(Node *type, Node *dec, hashnode *newnode){
	//printf("dec\t");
	//printf("%d\n", FLAG);
	hashnode *var = (hashnode*)malloc(sizeof(hashnode));
	newVar(type, dec->children[0], var);
	
	var->next = NULL;	
	//printf("%s!!%d\n", var->name, var->type->kind);	
	if(hash_lookup(var->name, 0) != NULL){
		if(FLAG >= 1) {
			printf("Error type 15 at Line %d: Redefined field \"%s\"\n", dec->children[0]->line, var->name);
		}
		else {
			printf("Error type 3 at Line %d: Redefined variable \"%s\"\n", dec->children[0]->line, var->name);
			//return;
		}
	}
	else {
		hash_add(var); 	
		if(var && var->type->kind == 2){
			Operand op = new_var();
			int size = getSize(var->type);
			var->operand = op;
			ir_newCode(10, op, size);
		}
	}
	if(dec->childrenNum == 3 && FLAG >= 1){
		//结构体	域初始化 error	
		printf("Error type 15 at Line %d: struct field inition \"%s\"\n", dec->children[0]->line, var->name);
		//return;
	}
	else if(dec->childrenNum == 3 && FLAG == 0){ // 变量初始化
		int type1 = getExpType(dec->children[0]);
		int type2 = getExpType(dec->children[2]);
		if(type1 != type2)
			printf("Error type 5 at Line %d: Type mismatched for assignment.\n", dec->children[0]->line);
		
		//变量初始化生成中间代码
		Operand op1 = new_var();
		var->operand = op1;
		Operand op2 = new_temp();
		trav_Exp(dec->children[2], op2);
		ir_newCode(2, op1, op2);
	}
	
	if(FLAG >= 1) {	
		FieldList tail;
		if(newnode->type == NULL){
			newnode->type = (Type)malloc(sizeof(struct Type_));
			newnode->type->u.structrue = (FieldList)malloc(sizeof(struct FieldList_));
			newnode->type->kind = 3;
			tail = newnode->type->u.structrue;
			tail->name = (char*)malloc(sizeof(char) * 50);
			strcpy(tail->name, var->name);
			tail->type = var->type;
			tail->next = NULL;		
		}
		else {
			tail = newnode->type->u.structrue;
			while(tail->next != NULL){
				tail = tail->next; 
			}	
		
			FieldList p = (FieldList)malloc(sizeof(struct FieldList_));
			p->name = (char*)malloc(sizeof(char) * 50);
			strcpy(p->name, var->name);
		
			p->type = var->type;
			p->next = NULL;
			tail->next = p;
		}	
	}
}

//ExtDecList -> VarDec
// | VarDec COMMA ExtDecList
void trav_ExtDecList(Node *type, Node *name){
	trav_VarDec(type, name->children[0]);
	if(name->childrenNum == 3){
		trav_ExtDecList(type, name->children[2]);
	}
}

void trav_VarDec(Node *type, Node *name){
	hashnode *newNode = (hashnode*)malloc(sizeof(hashnode));

	newVar(type, name, newNode);

	newNode->next = NULL;
	
	if(hash_lookup(newNode->name, 0) != NULL){
		printf("Error type 3 at Line %d: Redefined variable \"%s\"\n", type->line, newNode->name);
	}
	else
		hash_add(newNode);
	//printf("%s!!\n", newNode->name);
}

void newVar(Node *type, Node *name, hashnode* newNode){
	int sizeArr[10];
	int index = 0;	
	Node *temp = name;
	//var is array
	while(temp->childrenNum != 1){
		sizeArr[index++] = temp->children[2]->name.intValue;
		temp = temp->children[0];
	}

	newNode->name = (char*)malloc(sizeof(char) * 50);	
	strcpy(newNode->name, temp->children[0]->name.content);
	//printf("%s\n", newNode->name);

	newNode->type = (Type)malloc(sizeof(struct Type_));
	Type pre = newNode->type;
	Type tail;
	while(index > 0){
		tail = (Type)malloc(sizeof(struct Type_));
		tail->kind = 2;
		tail->u.array.size = sizeArr[--index];
		pre->u.array.elem = tail;
		//printf("%d\n", tail->u.array.size);
		pre = tail;			
	}
	tail = (Type)malloc(sizeof(struct Type_));
	int varType = getType(type);
	//printf("%d\n", varType);
	if(varType == TYPEOFSTRUCTURE){
		trav_SpecSemi(type);
		tail->kind = 1;
		tail->u.structname = (char*)malloc(sizeof(char) * 50);
		strcpy(tail->u.structname, getStructName(type));
	}
	else {
		tail->kind = 0;
		tail->u.basic = varType;
	}
	//tail->u.array.elem = NULL;
	pre->u.array.elem = tail;
	newNode->type = newNode->type->u.array.elem;
	
}

char* getStructName(Node *type){
	Node *op = type->children[0]->children[1];
	if(op == NULL){
		char *null = (char*)malloc(sizeof(char) * 10);
		strcpy(null, Node_types[indexNum - 1]);	
		return null;
	}
	Node* child = op->children[0];
	char *structname = child->name.content;
	return structname;
}

int getType(Node *p){
	Node *child = p->children[0];
	int type = child->type;
	//TYPE
	if(type == 23){
		if(strcmp(child->name.content, "int") == 0)
			return TYPEOFINT;
		else
			return TYPEOFFLOAT;
	}
	else 
		return TYPEOFSTRUCTURE;
}

int getSize(Type type){
	Type p = type;
	int size = 1;
	while(p->kind == 2){
		size *= p->u.array.size;
		p = p->u.array.elem;
	}
	return size * 4;
}
