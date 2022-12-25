#include "ASTtree.h"
#include "symbol.h"
struct ASTnode* newNode(NonTerminal name, char* value, int lineNum){
    struct ASTnode* p = (struct ASTnode*)malloc(sizeof(struct ASTnode));
    p->child=0;
    p->sibling=0;
	p->line=lineNum;
	p->name = name;
	p->isTerminal = true;
	p->nodeType=0;
	int l=strlen(value);
    p->value = (char*)malloc(sizeof(char)*l+1);
    strcpy(p->value,value);
    return p;
}


struct ASTnode* addNode(NonTerminal name,int num,...){
    // add to sibling
    va_list valist;
    struct ASTnode* t;
    va_start(valist,num);
    struct ASTnode* p = (struct ASTnode*)malloc(sizeof(struct ASTnode));
    p->child=0;
    p->sibling=0;
    p->name=name;
	p->isTerminal = false;
	p->nodeType=0;
	printf("%s\n",toString(name));
    int i=0;
    if(num>0){
        t = (struct ASTnode*)va_arg(valist,struct ASTnode*);
        p->child = t;
		p->line= t->line;
        for(i=1;i<num;i++){
            t->sibling = (struct ASTnode*)va_arg(valist,struct ASTnode*);
            t=t->sibling;
			p->line= t->line;
        }       
    }
    va_end(valist);
    return p;
}



void traverlTree(struct ASTnode* head, int level,FILE* fp){
	int i;	
	for(i=0;i<level;i++){
		fprintf(fp,"--");
		printf("--");	
	}
	int nextLevel = level;
	if(head->isTerminal){
		
		fprintf(fp,"%s , line: %d\n",toString(head->name),head->line);
		printf("%s , line: %d\n",toString(head->name),head->line);
		/*
		fprintf(fp,"%s\n",toString(head->name));
		printf("%s\n",toString(head->name));
		*/
		if(head->sibling!=0)
			traverlTree(head->sibling,level,fp);
	}		
	else{		
		/*
		fprintf(fp,"%s\n",toString(head->name));
		printf("%s\n",toString(head->name));
		*/
		
		fprintf(fp,"%s , line: %d\n",toString(head->name),head->line);
		printf("%s , line: %d\n",toString(head->name),head->line);
		
		if(head->child!=0)
			traverlTree(head->child,level+1,fp);
		if(head->sibling!=0)
			traverlTree(head->sibling,level,fp);		
	}
}


void traverlShort(struct ASTnode* head, int level,FILE* fp){
	int i;
	int nextLevel = level;
	if(head->isTerminal){
		for(i=0;i<level;i++){
			fprintf(fp,"--");
			printf("--");	
		}
		fprintf(fp,"%s\n",toString(head->name));
		printf("%s\n",toString(head->name));
		if(head->sibling!=0)
			traverlShort(head->sibling,level,fp);
	}		
	else{
		if(head->sibling!=0||level==0){
			for(i=0;i<level;i++){
				fprintf(fp,"--");
				printf("--");	
			}
			fprintf(fp,"%s\n",toString(head->name));
		printf("%s\n",toString(head->name));
			nextLevel+=1;
		}
		if(head->child!=0)
			traverlShort(head->child,nextLevel,fp);
		if(head->sibling!=0)
			traverlShort(head->sibling,level,fp);
	}
}

void traverlDebug(struct ASTnode* head, int level){
	int i;	
	for(i=0;i<level;i++){

		printf("--");	
	}
	int nextLevel = level;
	if(head->isTerminal){
		if(head->nodeType==0)
			printf("%s , type: %s\n",toString(head->name),"not travel");
		else
			printf("%s , type:%s ptr=%d isConst=%d\n",toString(head->name),typeToString(head->nodeType->varType),head->nodeType->ptr,head->nodeType->isConst);
		if(head->sibling!=0)
			traverlDebug(head->sibling,level);
	}		
	else{		
		if(head->nodeType==0)
			printf("%s , type: %s\n",toString(head->name),"not travel");
		else
			printf("%s , type:%s ptr=%d isConst=%d\n",toString(head->name),typeToString(head->nodeType->varType),head->nodeType->ptr,head->nodeType->isConst);
		if(head->child!=0)
			traverlDebug(head->child,level+1);
		if(head->sibling!=0)
			traverlDebug(head->sibling,level);		
	}
/*
	int i;
	for(i=0;i<level;i++){
		printf("--");	
	}
	printf("%s   ",toString(head->name));
	
	if(head->nodeType!=0)
		printType(head->nodeType);
	else
		printf("not travelled\n");
	
	if(head->child!=0)
		traverlDebug(head->child,level+1);
	if(head->sibling!=0)
		traverlDebug(head->sibling,level+1);
		*/
}