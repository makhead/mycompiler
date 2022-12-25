
#ifndef AST
#define AST
#include "include.h"


struct ASTnode* addNode(NonTerminal name,int num,...);

struct ASTnode* newNode(NonTerminal name, char* value, int lineNum);

void traverlTree(struct ASTnode* head, int level,FILE* fp);

void traverlShort(struct ASTnode* head, int level,FILE* fp);

void traverlDebug(struct ASTnode* head, int level);
#endif