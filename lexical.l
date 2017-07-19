%{
	#include "syntax.tab.h"
	#include "string.h"
	int yycolumn = 1;
	#define YY_USER_ACTION \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;
%}
NEWLINE [\n\r]
WC [ \t]+
TYPE (int)|(float)
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
digit [0-9]
letter_ [a-zA-Z_]
INT [+-]?0|([1-9]{digit}*)
FLOAT [+-]?{digit}+\.{digit}+
FLOATE [+-]?({digit}+\.{digit}*)|({digit}*\.{digit}+)([eE][+-]?{digit}+)
ID {letter_}({letter_}|{digit})*
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >=|<=|>|<|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
%option yylineno
yylineno=1
%%
{NEWLINE}	{yycolumn = 1;}
{WC} 		{}
{STRUCT} 	{yylval=(Node*)malloc(sizeof(Node));yylval->type=22;return STRUCT;}
{RETURN} 	{yylval=(Node*)malloc(sizeof(Node));yylval->type=0;return RETURN;}
{IF}		{yylval=(Node*)malloc(sizeof(Node));yylval->type=19;return IF;}
{ELSE} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=20;return ELSE;}
{WHILE} 	{yylval=(Node*)malloc(sizeof(Node));yylval->type=21;return WHILE;}
{TYPE} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=23;strcpy(yylval->name.content, yytext);return TYPE;}
{ID} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=26;strcpy(yylval->name.content, yytext);return ID;}
{INT} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=24;yylval->name.intValue=atoi(yytext);return INT;}
{FLOAT} 	{yylval=(Node*)malloc(sizeof(Node));yylval->type=25;yylval->name.floatValue=atof(yytext);return FLOAT;}
{FLOATE}	{yylval=(Node*)malloc(sizeof(Node));yylval->type=25;yylval->name.floatValue=atof(yytext);return FLOAT;}
{SEMI} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=1;return SEMI;}
{COMMA}		{yylval=(Node*)malloc(sizeof(Node));yylval->type=2;return COMMA;}
{ASSIGNOP} 	{yylval=(Node*)malloc(sizeof(Node));yylval->type=3;return ASSIGNOP;}
{RELOP}		{yylval=(Node*)malloc(sizeof(Node));yylval->type=18;strcpy(yylval->name.content, yytext);return RELOP;}
{PLUS} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=4;return PLUS;}
{MINUS} 	{yylval=(Node*)malloc(sizeof(Node));yylval->type=5;return MINUS;}
{STAR} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=6;return STAR;}
{DIV} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=7;return DIV;}
{AND} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=8;return AND;}
{OR} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=9;return OR;}
{DOT} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=10;return DOT;}
{NOT} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=11;return NOT;}
{LP}		{yylval=(Node*)malloc(sizeof(Node));yylval->type=12;return LP;}
{RP} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=13;return RP;}
{LB} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=14;return LB;}
{RB} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=15;return RB;}
{LC} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=16;return LC;}
{RC} 		{yylval=(Node*)malloc(sizeof(Node));yylval->type=17;return RC;}
. 		{isError = 1;printf("Error type A at Line %d: Mysterious Characters \'%s\'\n", yylineno, yytext);}
%%
