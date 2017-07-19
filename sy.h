#include <stdio.h>
#include <stdarg.h>
#ifndef _SY_H
#define _SY_H

typedef	union {
	char content[50];
	int intValue;
	float floatValue;
}Name;
typedef struct node{
	int type;
	Name name;		
	int line;
	int childrenNum;
	struct node **children;
}Node;
typedef Node* TypeNode;

extern char Node_types[48][20];
//遍历访问语法树
Node* createNode(int childNum,...);
void PreOrder(Node* current, int depth);

#endif
