#ifndef SYM
#define SYM
#include "include.h"
#define DEBUG0 0
extern bool isError;
void initSym();
void addScope();

void dropScope();

extern int hash(char* name);

void addVar(char* name, int lineno, struct type* vartype,bool isfunc,struct funcParaList* paraList,struct symTableList *s);
struct type* check(struct ASTnode* syntaxTree,struct type* type_para);
void SymCheck(struct ASTnode* syntaxTree);
struct symbol* lookup(char* name,struct symTableList* s);
struct symbol* lookupAll(char* name);
void evalType(struct ASTnode* syntaxTree,struct type* t);
void insert(char* name,int lineno, struct type* vartype,bool isfunc,struct funcParaList* paraList,int upperScope);
bool typeEqual(struct type* a, struct type* b);
void printType(struct type* b);
void printTable();
char* typeToString(BasicType t);
void saveType(struct ASTnode* node,struct type* nodeType);
char* toString(NonTerminal name);
bool checkFuncPara(struct funcParaList* paraList,struct ASTnode* tree);
#endif
//======================================================================