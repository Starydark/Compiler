#include <stdio.h>
#include "hash.h"
#include "symtab.h"

extern int yyrestart();
extern int yyparse();
extern int isError;
extern void print();
extern Node *root;

int main(int argc, char* argv[])
{
    if(argc <= 1)
	return 1;
    FILE *f = fopen(argv[1], "r");
    if(!f) {
        perror(argv[1]);
	return 1;
    }
    yyrestart(f);
    yyparse();
    //if(isError == 0)
	//print();
	
    hash_create();
	FLAG = 0;
	flagNum = 0;	
	StatementNum = 0;
	indexNum = 0;
    traversal(root);
	
	//printf("%d\n", flagNum);	
    if(flagNum != 0){
		for(int i = 0;i < StatementNum; i++){
			if(linetable[i] != -1){
				printf("Error type 18 at Line %d: Undefined function.\n", linetable[i]);
			}
		}
	}	
	//printf("%d\n", argc);
	FILE *fp;
	if(argc == 3){
		fp = fopen(argv[2], "w+");
	}
	if(fp){
		clean_temp_var();
		ir_print(fp);
    }
    //hash_print();
    return 0;    
}


