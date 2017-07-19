#include "sy.h"
#include "hash.h"
#include "ir.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#ifndef _SYMTAB_H
#define _SYMTAB_H

int FLAG;
//extern char Node_types[48][20];
void traversal(Node *root);
//访问ExtDef子树
void traversal_ExtDef(Node *root);
//访问Def子树
void traversal_Def(Node *root);
	
void trav_ExtDecList(Node *type, Node *name);

void trav_VarDec(Node *type, Node *name);

void newVar(Node *type, Node *name, hashnode* newNode);

int getType(Node *p);

char* getStructName(Node *type);

void trav_SpecSemi(Node *type);

void trav_DefList(Node *deflist, hashnode *newnode);

void trav_Def(Node *def, hashnode* newnode);

void trav_DecList(Node *type, Node *declist, hashnode *newnode);

void trav_Dec(Node *type, Node *dec, hashnode *newnode);

void trav_Func(Node *root);

void trav_FuncDec(Node* funcdec, hashnode* newnode);

void trav_VarList(Node *varList, hashnode* newnode);

void trav_ParamDec(Node *ParamDec, hashnode *newnode);

void trav_CompSt(Node* compst, hashnode *newnode);

void trav_StmtList(Node *stmtlist, hashnode *newnode);

void trav_Stmt(Node *stmt, hashnode *newnode);

void trav_Exp(Node *exp, Operand op);

int getExpType(Node* exp);

int getTypeOfArray(Node *exp);

bool match(Type virtual, Node *exp);

bool ExistField(Node *exp, char *name);

bool left_hand(Node *exp);

void funcstatement(hashnode* newnode, int line);
bool compareFuncparamList(Type func1, Type func2);
bool matchStruct(Node *structVar1, Node *strcutVar2);

int getSize(Type p);
void translate_Cond(Node *exp, Operand label_true, Operand label_false);
void translate_Args(Node *exp, int flag);
void translate_Arr(Node *exp, Operand op);
void translate_else(Node *exp, Operand label_true, Operand label_false);





#endif
