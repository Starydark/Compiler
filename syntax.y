%{
	#include "sy.h"	
	#define YYSTYPE Node*
	Node* createNode(int childNum,...);
	char Node_types[48][20]={"RETURN","SEMI","COMMA","ASSIGNOP","PLUS","MINUS",
"STAR","DIV","AND","OR","DOT","NOT","LP","RP","LB","RB","LC","RC",
"RELOP","IF","ELSE","WHILE","STRUCT","TYPE","INT","FLOAT","ID",
"Program","ExtDefList","ExtDef","ExtDecList","Specifier","StructSpecifier",
"OptTag","Tag","VarDec","Fundec","VarList","ParamDec","CompSt","StmtList",
"Stmt","DefList","Def","DecList","Dec","Exp","Args"};
	Node* root;	
	int isError = 0;
	extern int yyerror();
	extern int yylex();
%}
%locations
%token NEWLINE
%token INT
%token FLOAT
%token ID
%token SEMI COMMA DOT
%token TYPE STRUCT
%token LC RC LB RB LP RP
%token IF ELSE WHILE RETURN
%token ASSIGNOP AND OR NOT RELOP PLUS MINUS STAR DIV
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT LP RP LB RB
%%
Program : ExtDefList 	{$$=createNode(1, $1);$$->type = 27; $$->line = @$.first_line; root=(Node *)malloc(sizeof(Node));root=$$;}
	;
ExtDefList : /*empty*/		 {$$=NULL;}
	| ExtDef ExtDefList 	 {$$=createNode(2, $1, $2); $$->type = 28; $$->line = @$.first_line;}
	;
ExtDef :  Specifier ExtDecList SEMI 	{$$=createNode(3, $1, $2, $3); $$->type=29;$$->line=@$.first_line;}
	| Specifier SEMI 		{$$=createNode(2,$1,$2); $$->type=29; $$->line=@$.first_line;}
	| Specifier FunDec CompSt 	{$$=createNode(3,$1,$2,$3);$$->type=29;$$->line=@$.first_line;}
	| Specifier FunDec SEMI		{$$=createNode(3,$1,$2,$3);$$->type=29;$$->line=@$.first_line;}
	| Specifier ExtDecList error    //{printf("ExtDef3\n");}
	| Specifier error		//{printf("ExtDef1\n");}
	| error SEMI			//{printf("ExtDef2\n");}
	;
ExtDecList : VarDec 			{$$=createNode(1,$1); $$->type=30;$$->line=@$.first_line;}
	| VarDec COMMA ExtDecList 	{$$=createNode(3,$1,$2,$3); $$->type=30; $$->line=@$.first_line;}
	//| VarDec error ExtDecList	{printf("ExtDecList1");}
	;
Specifier : TYPE 			{$$=createNode(1,$1); $$->type=31;$$->line=@$.first_line;}
	| StructSpecifier 		{$$=createNode(1,$1); $$->type=31; $$->line=@$.first_line;}
	;
StructSpecifier : STRUCT OptTag LC DefList RC 	{$$=createNode(5, $1,$2,$3,$4,$5);$$->type=32; $$->line=@$.first_line;}
	| STRUCT Tag 				{$$=createNode(2,$1,$2); $$->type=32; $$->line=@$.first_line;}
	| error RC				//{printf("StructSpecifier2\n");}
	;
OptTag :  /*empty*/ 			{$$=NULL;}
	| ID 				{$$=createNode(1,$1);$$->type=33;$$->line=@$.first_line;}
	;
Tag :	  ID 			{$$=createNode(1,$1);$$->type=34; $$->line=@$.first_line;}
	;
VarDec :  ID 			{$$=createNode(1,$1);$$->type=35;$$->line=@$.first_line;}
	| VarDec LB INT RB 	{$$=createNode(4,$1,$2,$3,$4);$$->type=35;$$->line=@$.first_line;}
	;
FunDec :  ID LP VarList RP 	{$$=createNode(4,$1,$2,$3,$4);$$->type=36;$$->line=@$.first_line;}
	| ID LP RP 		{$$=createNode(3,$1,$2,$3);$$->type=36;$$->line=@$.first_line;}
	| ID error RP	{printf("FunDec1\n");}
	;
VarList : ParamDec COMMA VarList {$$=createNode(3,$1,$2,$3);$$->type=37;$$->line=@$.first_line;}
	| ParamDec 		 {$$=createNode(1,$1);$$->type=37;$$->line=@$.first_line;}
	;
ParamDec : Specifier VarDec 	 {$$=createNode(2, $1, $2);$$->type=38;$$->line=@$.first_line;}
	;
CompSt :  LC DefList StmtList RC {$$=createNode(4,$1,$2,$3,$4);$$->type=39;$$->line=@$.first_line;}
	| error DefList StmtList RC /*confilict*/ 	//{printf("CompSt1\n");}	
	;
StmtList : /*empty*/ 		{$$=NULL;}
	| Stmt StmtList 	{$$=createNode(2, $1,$2);$$->type=40;$$->line=@$.first_line;}
	;
Stmt :	  Exp SEMI 				  {$$=createNode(2, $1,$2);$$->type=41;$$->line=@$.first_line;}
	| CompSt 				  {$$=createNode(1,$1);$$->type=41;$$->line=@$.first_line;}
	| RETURN Exp SEMI 			  {$$=createNode(3,$1,$2,$3);$$->type=41;$$->line=@$.first_line;}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=createNode(5,$1,$2,$3,$4,$5);$$->type=41;$$->line=@$.first_line;}
	| IF LP Exp RP Stmt ELSE Stmt 		  {$$=createNode(7,$1,$2,$3,$4,$5,$6,$7);$$->type=41;$$->line=@$.first_line;}
	| WHILE LP Exp RP Stmt 			  {$$=createNode(5,$1,$2,$3,$4,$5);$$->type=41;$$->line=@$.first_line;}
	| Exp error				  {printf("Stmt1\n");}
	| error SEMI				  //{printf("Stmt2\n");}
	| RETURN Exp error SEMI	
	;
DefList : /*empty*/		{$$=NULL;}
	| Def DefList 		{$$=createNode(2,$1,$2);$$->type=42;$$->line=@$.first_line;}
	;
Def :  	  Specifier DecList SEMI 	{$$=createNode(3,$1,$2,$3);$$->type=43;$$->line=@$.first_line;}
	| Specifier error SEMI		//{printf("Def1\n");}
	| error DecList SEMI		//{printf("Def2\n");}
	| Specifier DecList error	//{printf("Def3\n");}	
	;
DecList : Dec 			{$$=createNode(1,$1);$$->type=44;$$->line=@$.first_line;}
	| Dec COMMA DecList 	{$$=createNode(3,$1,$2,$3);$$->type=44;$$->line=@$.first_line;}
	;
Dec :	  VarDec 		{$$=createNode(1,$1);$$->type=45;$$->line=@$.first_line;}
	| VarDec ASSIGNOP Exp 	{$$=createNode(3,$1,$2,$3);$$->type=45;$$->line=@$.first_line;}
	;
Exp : 	  Exp ASSIGNOP Exp 	{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| Exp AND Exp 		{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| Exp OR Exp 		{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| Exp RELOP Exp 	{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| Exp PLUS Exp 		{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| Exp MINUS Exp 	{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| Exp STAR Exp 		{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| Exp DIV Exp 		{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| LP Exp RP 		{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| MINUS Exp 		{$$=createNode(2,$1,$2);$$->type=46;$$->line=@$.first_line;}
	| NOT Exp 		{$$=createNode(2,$1,$2);$$->type=46;$$->line=@$.first_line;}
	| ID LP Args RP 	{$$=createNode(4,$1,$2,$3,$4);$$->type=46;$$->line=@$.first_line;}
	| ID LP RP 		{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| Exp LB Exp RB 	{$$=createNode(4,$1,$2,$3,$4);$$->type=46;$$->line=@$.first_line;}
	| Exp DOT ID 		{$$=createNode(3,$1,$2,$3);$$->type=46;$$->line=@$.first_line;}
	| ID 			{$$=createNode(1,$1);$$->type=46;$$->line=@$.first_line;}
	| INT			{$$=createNode(1,$1);$$->type=46;$$->line=@$.first_line;}
	| FLOAT 		{$$=createNode(1,$1);$$->type=46;$$->line=@$.first_line;}
	| Exp ASSIGNOP error 	{printf("Exp1\n");}
	| Exp AND error 	//{printf("Exp2\n");}
	| Exp OR error 		//{printf("Exp3\n");}
	| Exp RELOP error 	//{printf("Exp4\n");}
	| Exp PLUS error 	//{printf("Exp5\n");}
	| Exp MINUS error 	//{printf("Exp6\n");}
	| Exp STAR error 	//{printf("Exp7\n");}
	| Exp DIV error 	//{printf("Exp8\n");}
	| Exp DOT error 	//{printf("Exp9\n");}
	| NOT error 		//{printf("Exp10\n");}
	| MINUS error		//{printf("Exp11\n");}
	| Exp LB error RB	{printf("Exp12\n");}
	;
Args :    Exp COMMA Args 	{$$=createNode(3,$1,$2,$3);$$->type=47;$$->line=@$.first_line;}
	| Exp 			{$$=createNode(1,$1);$$->type=47;$$->line=@$.first_line;}
	| Exp error Exp		//{printf("Args1\n");}
	| Exp COMMA error	//{printf("Args2\n");}
	| error Exp 		//{printf("Args3\n");}
	;
%%
#include "lex.yy.c"
#include "sy.h"

int yyerror(char *msg)
{
	isError = 1;
	fprintf(stderr,"Error type B at Line %d and Column %d to Column %d: Unexpected \'%s\'\n", yylloc.first_line, yylloc.first_column, yylloc.last_column,  yytext);
}

Node* createNode(int childNum,...){
	Node* currentNode;
	currentNode = (Node *)malloc(sizeof(Node));
	currentNode->children = (TypeNode *)malloc(sizeof(TypeNode)*childNum);
	va_list pArg;
	va_start(pArg, childNum);
	int num = childNum;
	currentNode->childrenNum = childNum;
	int i;
	for(i = 0;i < num; i++){
		currentNode->children[i] = va_arg(pArg, TypeNode);
	}	
	return currentNode;
}

void PreOrder(Node *current, int depth){
	if(current == NULL)
		return;
	int i;
	for(i = 0;i < depth; i++){
		printf(" ");
	}
	if(current->type < 27){
		if(current->type == 23){
			printf("TYPE: %s\n", current->name.content);
		}
		else if(current->type == 24){
			printf("INT: %d\n", current->name.intValue);
			
		}
		else if(current->type == 25){
			printf("FLOAT: %f\n", current->name.floatValue);
		}
		else if(current->type == 26){
			printf("ID: %s\n", current->name.content);
		}
		else {
			printf("%s\n", Node_types[current->type]);
		}
	}
	else {
		if(current != NULL)
			printf("%s (%d)\n", Node_types[current->type], current->line);
		int i;
		for(i = 0;i < current->childrenNum; i++){
			PreOrder(current->children[i], depth+2);
		}
	}
}

void print(){
	PreOrder(root, 0);		
}
