#ifndef _HASH_H
#define _HASH_H

#include "common.h"
#include "ir.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FuncparamList_* FuncparamList;
//extern struct Operand_* Operand;

struct Type_ { 
	enum {BASIC = 0, STRUCTVAR, ARRAY, STRUCTURE, FUNC, STATEMENT} kind;
	union {
		//基本类型
		int basic;
		//数组类型信息包括元素类型与数组大小构成
		struct {
			Type elem;
			int size;		
		} array;
		//结构体类型信息是一个链表
		FieldList structrue;
		//函数
		struct {
			int type;
			char *structName;
			int index;
			FuncparamList param;
		} func;
		char *structname;
	}u;
	
};

struct FieldList_ {
	char *name; //域的名字
	Type type;  //域的类型
	FieldList next;  //下一个域
};

struct FuncparamList_ {
	char *name;	 //形参名字
	Type type;  //形参类型
	FuncparamList next; //下一个形参
};

typedef struct HashNode{
	char *name;
	struct Operand_* operand;
	Type type;
	struct HashNode* next;
} hashnode;

hashnode* hashtable[HASHSIZE];
int linetable[HASHSIZE];
int StatementNum;
int flagNum;
int indexNum;

void hash_create();
void hash_add(hashnode* newnode);
unsigned hash_pjw(char* name);
hashnode* hash_lookup(char* name, int type);
void hash_print();
void hash_printArr(Type arr);

#endif
