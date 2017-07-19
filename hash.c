#include "hash.h"
#include "common.h"
void hash_create() {
	int i;
	for(i = 0; i < HASHSIZE; i++)
		hashtable[i]= NULL;   
}
void hash_add(hashnode* newnode) {
	unsigned int index = hash_pjw(newnode->name);
	//if(hash_lookup(index, hashnode* newnode)){
	//	return;
	//}
	//printf("hash_add_start\n");
	if(hashtable[index] == NULL){
		//printf("2\n");
		hashtable[index] = newnode;	
		//printf("%s, %d\n", newnode->name, index);
		//printf("3\n");	
	}
	else {
		//printf("3\n");
		//printf("%s\t%s\n", newnode->name, hashtable[index]->name);
		newnode->next = hashtable[index];
		hashtable[index] = newnode;
		//hashtable[index]->next = newnode;
	}
	//printf("hash_add_end\n");
}

unsigned int hash_pjw(char* name){
	unsigned int val = 0, i;
	for(; *name; ++name) {
		val = (val << 2) + *name;
		if(i = val & ~ HASHSIZE - 1)
			val = (val ^ (i >> 12)) & HASHSIZE - 1;	
	}	
	return val;
}

hashnode* hash_lookup(char *name, int type){
	unsigned int index = hash_pjw(name);
	if(hashtable[index] == NULL)
		return NULL;	
	else {
		hashnode *p = hashtable[index];
		while(p != NULL){
			if(strcmp(name, p->name) == 0) {
				if(type == 0 && p->type->kind < 4){
					return p;
				}				
				else if(type == 4 && p->type->kind == 4){
					return p;
				}
				else if(type == 5 && p->type->kind == 5){
					return p;
				}
			}
			p = p->next;
		}
	}
	return NULL;
}

void hash_print(){
	printf("hash_print_start\n");
	int i;	
	for(i = 0;i < HASHSIZE; i++){
		hashnode *p = hashtable[i];	
		while(p != NULL){
			printf("------------------\n");
			printf("名字%s\n", p->name);
			
			Type q = p->type;
			if(q->kind != 0 && q->kind != 1) {
				if(q->kind == 2){ //全局数组
					printf("数组%d\n", q->kind);
					hash_printArr(q);
					printf("数组结束\n");
				}					
				else if(q->kind == 3){ //结构体
					printf("结构体域\n");
					
					FieldList field = q->u.structrue;

					while(field != NULL) {
					
						printf("名字%s\t", field->name);
						Type filedtype = field->type;
					
						hash_printArr(filedtype);

						field = field->next;
					}
					printf("结构体域结束\n");
				}
				else if(q->kind == 4) { //函数
					printf("函数\n");
					printf("返回值类型: %d\n", q->u.func.type);
					
					FuncparamList param = q->u.func.param;
					while(param != NULL) {
						printf("形参名字：%s\n", param->name);
						Type paramType = param->type;
						hash_printArr(paramType);
						
						param = param->next;
					}
					
					//todo: Compst
					printf("函数结束\n");
				}	
			}
			else{
				if(q->kind == 1)
					printf("结构体变量: %s\n", q->u.structname);
				else if(q->u.basic == TYPEOFINT)
					printf("类型: int\n");                  
				else if(q->u.basic == TYPEOFFLOAT)
					printf("类型: float\n");

				//printf("普通变量%d\n普通变量结束\n", q->kind);
			}
			printf("------------------\n");
			p = p->next;		
		}
	}
	printf("hash_print_end\n");
}

void hash_printArr(Type arr){
	while(arr->kind != 0 && arr->kind != 1){
		printf("大小%d\t", arr->u.array.size);
		arr = arr->u.array.elem;
	}
	if(arr->kind == 1)
		printf("结构体变量: %s\n", arr->u.structname);
	if(arr->u.basic == TYPEOFINT)
		printf("类型: int\n");
	else if(arr->u.basic == TYPEOFFLOAT)
		printf("类型: float\n"); 
}

