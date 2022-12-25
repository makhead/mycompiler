#include "symbol.h"
#include <stdbool.h>
extern struct ASTnode* root;
extern struct symTableList* symTable;

int hash(char* name){
    int l = strlen(name);
    char* p = (char*)malloc(sizeof(char)*l);
    strcpy(p,name);
    if(p==NULL){
        printf("hash error!");
        return 0;
    }
    int sum=0;
    int i=0;
    for(i=0;i<l;i++){
        //printf("%d %d\n",sum,l);
        sum += ((sum << SHIFT_AMOUNT) + p[i]);
        sum %= SYMTABLE_SIZE;
    }
    return sum;
}
/*
void initSym(struct symTableList* symTable){
    symTable = (struct symTableList*)malloc(sizeof(struct symTableList));
    symTable->next=NULL;
}
*/
void initSym(){
    symTable = (struct symTableList*)malloc(sizeof(struct symTableList));
    int i;
    for(i=0;i<SYMTABLE_SIZE;i++)
        symTable->table[i]=0;
    symTable->next=NULL;
    isError=false;
}

void addScope(){
    //printf("addScope\n");
    struct symTableList* p = (struct symTableList*)malloc(sizeof(struct symTableList));
    int i=0;
    for(i=0;i<SYMTABLE_SIZE;i++)
        p->table[i] = 0;
    p->next = symTable;
    symTable = p;
}

void dropScope(){
    //printf("dropScope\n");
    struct symTableList* p = symTable;
    symTable = symTable->next;
    free(p);
}

void addVar(char* name, int lineno, struct type* vartype,bool isfunc,struct funcParaList* paraList,struct symTableList *s){
    int hash_value = hash(name);
    int i=0;
    int l= strlen(name)+1;
    struct symbol* p = (struct symbol*)malloc(sizeof(struct symbol));
    p->lineno = lineno;
    p->isfunc = isfunc;
    p->vartype.isConst = vartype->isConst;
    p->vartype.ptr = vartype->ptr;
    p->vartype.varType = vartype->varType;
    p->paraList = paraList;
    p->vartype.name = (char*)malloc(sizeof(char)*l);
    strcpy(p->vartype.name,name);
    p->next = s->table[hash_value];
    s->table[hash_value] = p;
}

struct symbol* lookup(char* name,struct symTableList* s){
    int hash_value = hash(name);

    struct symbol* p = s->table[hash_value];
   
    while(p!=NULL){
        if(strcmp(p->vartype.name,name)==0){
            return p;
        }
        p = p->next;
    }
    return NULL;
}

struct symbol* lookupAll(char* name){
    int hash_value = hash(name);
    struct symTableList* s = symTable;
    struct symbol* p;
    while(s!=NULL){
        p = lookup(name,s);
        if(p!=NULL)
            return p;
        s = s->next;
    }
    return NULL;
}



struct type* check(struct ASTnode* syntaxTree,struct type* type_para){
    
    struct ASTnode* c;
    struct symbol* t;
    struct type *e = (struct type *)malloc(sizeof(struct type));
    struct type *e1;
    struct type* r;
    struct type* r1;
    struct type* r2;
    struct type* r3;
    
    if(DEBUG0){ printf("start: %s\n",toString(syntaxTree->name));}

    if(syntaxTree->name==T_global_var_stmt||syntaxTree->name==T_local_var_stmt){
        c=syntaxTree->child;
        evalType(c->child,e);
        if(DEBUG0) {printType(e);}
        r=check(syntaxTree->child->sibling,e);
        if(DEBUG0) {printf("return : %s\n",toString(syntaxTree->name));}
        if(DEBUG0) printType(r);
        saveType(syntaxTree,r);
        return r;  
    }
    else if(syntaxTree->name==T_local_var_list){
        if(syntaxTree->child->sibling==0){
            r=check(syntaxTree->child,type_para);
            r->isConst=0;
            r->ptr=0;
            r->varType=TYPE_VOID;
            if(DEBUG0) printf("return : %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;
        }
        else{
            r1=check(syntaxTree->child,type_para);
            r2=check(syntaxTree->sibling->sibling,type_para);
            r1->isConst=0;
            r1->ptr=0;
            r1->varType=TYPE_VOID;
            if(DEBUG0) printf("return : %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1;
        }
        
    }
    else if(syntaxTree->name==T_func_stmt){
        c=syntaxTree->child;
        evalType(c->child,e);
        c=c->sibling;
        struct funcParaList* paraList = 0;
        struct funcParaList* last = 0;
        struct funcParaList* q = 0;
        int length=0;
        //insert(c->value,syntaxTree->line,e,true,paraList,1); //look up first then add to sym table      
        // due with para list========================================
        c=c->sibling->sibling;
        addScope();
        if(c->name==T_para_list){
            c=c->child;
            while(c->name==T_para_list){
                e1 = (struct type *)malloc(sizeof(struct type));
                evalType(c->sibling->sibling->child->child,e1);
                insert(c->sibling->sibling->child->sibling->value,syntaxTree->line,e1,0,0,0);
                
                if(paraList==0){
                    last = (struct funcParaList*)malloc(sizeof(struct funcParaList));
                    paraList = last;
                    length = strlen(c->sibling->sibling->child->sibling->value);
                    last->para = e1;
                    last->para->name = (char*)malloc(sizeof(char)*length);
                    strcpy(last->para->name,c->sibling->sibling->child->sibling->value);
                    last->next = 0;
                }
                else{
                    q = (struct funcParaList*)malloc(sizeof(struct funcParaList));
                    length = strlen(c->sibling->sibling->child->sibling->value);
                    q->para = e1;
                    q->para->name = (char*)malloc(sizeof(char)*length);
                    q->next=0;
                    strcpy(q->para->name,c->sibling->sibling->child->sibling->value);
                    last->next = q;
                    last = last->next;
                }    
                c=c->child;
            }
            e1 = (struct type *)malloc(sizeof(struct type));
            evalType(c->child->child,e1);
            insert(c->child->sibling->value,syntaxTree->line,e1,0,0,0);

            if(paraList==0){
                last = (struct funcParaList*)malloc(sizeof(struct funcParaList));
                paraList = last;
                length = strlen(c->child->sibling->value);
                last->para = e1;
                last->para->name = (char*)malloc(sizeof(char)*length);                    strcpy(last->para->name,c->sibling->sibling->child->sibling->value);
                strcpy(last->para->name,c->child->sibling->value);
                last->next = 0;
            }
            else{
                q = (struct funcParaList*)malloc(sizeof(struct funcParaList));
                length = strlen(c->child->sibling->value);
                q->para = e1;
                q->para->name = (char*)malloc(sizeof(char)*length);
                q->next=0;
                strcpy(q->para->name,c->child->sibling->value);
                last->next = q;
                last = last->next;
            }
            insert(syntaxTree->child->sibling->value,syntaxTree->line,e,true,paraList,1);
            r=check(syntaxTree->child->sibling->sibling->sibling->sibling->sibling,NULL);
            
        }
        else {
            insert(syntaxTree->child->sibling->value,syntaxTree->line,e,true,paraList,1);
            r=check(syntaxTree->child->sibling->sibling->sibling->sibling,NULL);
        }       
        dropScope();
        if(typeEqual(e,r)){
            r->varType=TYPE_VOID;
            r->isConst=0;
            r->ptr=0;
        }
        else{     
            isError = true;       
            printf("line: %d , func %s return value error\n",syntaxTree->line,syntaxTree->child->sibling->value);
            r->isConst=0;
            r->ptr=0;
            r->varType=TYPE_VOID;       
        }
        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
        saveType(syntaxTree,r);
        return r;         
        //========================================
    }
    else if(syntaxTree->name==T_iter_stmt){
        c=syntaxTree->child;
        if(c->name==T_FOR){
            if(c->sibling->sibling->name==T_expr){
                r=check(c->sibling->sibling,NULL);
                r1=check(c->sibling->sibling->sibling->sibling,NULL);
                r2=check(c->sibling->sibling->sibling->sibling->sibling->sibling,NULL);
                addScope();
                r3=check(c->sibling->sibling->sibling->sibling->sibling->sibling->sibling->sibling,NULL);
                dropScope();
            }
            else{
                addScope();
                r=check(c->sibling->sibling,NULL);
                r1=check(c->sibling->sibling->sibling,NULL);
                r2=check(c->sibling->sibling->sibling->sibling->sibling,NULL);               
                r3=check(c->sibling->sibling->sibling->sibling->sibling->sibling->sibling,NULL);
                dropScope();
            }
            
            
            if(r1->varType!=TYPE_BOOL){
                isError = true;
                printf("line: %d , type %s cannot be for stmt end condition\n",syntaxTree->line,typeToString(r1->varType));
                r1->varType=ERR;
            }
            else{
                r1->isConst=0;
                r1->ptr=0;
                r1->varType=TYPE_VOID;          
            }
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);  
            return r1;    
        }
        else if(c->name==T_WHILE||c->name==T_DO){
            while(c!=0){
                if(c->name==T_expr){
                    r1=check(c,NULL);
                }
                else if(c->name==T_stmt_block){
                    addScope();                   
                    r2=check(c,NULL);       
                    dropScope();
                }
                c=c->sibling;
            }
            if(r1->varType!=TYPE_BOOL&&r1->varType!=TYPE_INT){
                isError=true;
                printf("line:%d , type %s cannot be while stmt cond\n",syntaxTree->line,(r1->varType));
                r1->varType=ERR;
            }
            else{
                r1->isConst=0;
                r1->ptr=0;
                r1->varType=TYPE_VOID;
            }
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);  
            return r1;    
        }
        
    }
    else if(syntaxTree->name==T_cond_stmt){
        c=syntaxTree->child;       
        while(c!=0){
            if(c->name==T_expr){
                r1 = check(c,NULL);
            }
            else if(c->name==T_else_stmt){
                if(c->child->sibling==0){ // single if
                    addScope();
                    r2 = check(c->child,NULL);
                    dropScope();
                }
                else{
                    addScope();
                    r2 = check(c->child,NULL);
                    dropScope();
                    addScope();
                    r2 = check(c->child->sibling->sibling,NULL);
                    dropScope();
                }
                
            }
            c=c->sibling;
        } 
        if(r1->varType!=TYPE_BOOL&&r1->varType!=TYPE_INT){
            isError=true;
            printf("line: %d, type %s cannot be if stmt cond\n",syntaxTree->line,typeToString(r1->varType));
            r1->varType=ERR;
        }
        else{
            r1->varType=TYPE_VOID;
        }
        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
        saveType(syntaxTree,r1);  
        return r1; 
    }
    else if(syntaxTree->name==T_NAME){
        e = (struct type*)malloc(sizeof(struct type));
        t=lookupAll(syntaxTree->value);
        if(t==0){
            isError=true;
            printf("line: %d ,variable %s is undeclared\n",syntaxTree->line,syntaxTree->value);             
            e->isConst = 0;
            e->ptr = 0;
            e->varType = ERR;   
            e->name = 0;
        }
        else{
            e->isConst = t->vartype.isConst;
            e->ptr = t->vartype.ptr;
            e->varType = t->vartype.varType;
            int temp = strlen(t->vartype.name);
            e->name = (char*)malloc(sizeof(char)*temp);
            strcpy(e->name,t->vartype.name);
        }
        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
        saveType(syntaxTree,e);  
        return e;
    }
    else if(syntaxTree->name==T_var_declar){
        if(syntaxTree->child->sibling==0){
            insert(syntaxTree->child->value,syntaxTree->line,type_para,0,0,0);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(type_para);
            saveType(syntaxTree,type_para);
            return type_para;  
        }                
        else if(syntaxTree->child->sibling->name==T_ASSIGN){
            insert(syntaxTree->child->value,syntaxTree->line,type_para,0,0,0);
            r=check(syntaxTree->child->sibling->sibling,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(typeEqual(type_para,r)){
                if(DEBUG0) printType(r);
                saveType(syntaxTree,r);
                return r;
            }           
            else{
                isError=true;
                if(DEBUG0) printType(r);
                printf("line: %d, variable %s :type %s cannot be assigned by type %s\n",syntaxTree->line,syntaxTree->child->value,typeToString(type_para->varType),typeToString(r->varType));
                r->varType=ERR;
                saveType(syntaxTree,r);
                return r;
            }                  
        }
        else if(syntaxTree->child->sibling->sibling->sibling->sibling==0){
            r=(struct type*)malloc(sizeof(struct type));
            r->isConst=type_para->isConst;
            r->ptr=type_para->ptr+1;
            r->varType=type_para->varType;
            insert(syntaxTree->child->value,syntaxTree->line,r,0,0,0);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;  
        }                
    }
    else if(syntaxTree->name==T_INTEGER){
        e = (struct type*)malloc(sizeof(struct type));
        e->isConst = 1;
        e->ptr = 0;
        e->varType = TYPE_INT;
        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
        saveType(syntaxTree,e);
        return e;
    }
    else if(syntaxTree->name==T_REAL){
        e = (struct type*)malloc(sizeof(struct type));
        e->isConst = 0;
        e->ptr = 0;
        e->varType = TYPE_DOUBLE;
        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
        saveType(syntaxTree,e);
        return e;
    }
    else if(syntaxTree->name==T_exp_atomic){
        if(syntaxTree->child->name==T_LSB){
            r = check(syntaxTree->child->sibling,NULL); 
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;      
        }            
        else{
            r = check(syntaxTree->child,NULL); 
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;      
        } 
    }
    else if(syntaxTree->name==T_exp_postfix){
        if(syntaxTree->child->name==T_exp_postfix){
            if(syntaxTree->child->sibling->name==T_LMB){
                r1 = check(syntaxTree->child,NULL);
                if(r1->varType==ERR){
                    if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                    if(DEBUG0) printType(r1);
                    saveType(syntaxTree,r1);
                    return r1;
                }                  
                else if(r1->ptr<=0){
                    isError=true;
                    printf("line: %d, type %s cannot be referenced\n",syntaxTree->line,typeToString(r1->varType));
                    r1->varType = ERR;
                }
                else{
                    r1->ptr--;
                }
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            else if(syntaxTree->child->sibling->name==T_LSB){
                if(syntaxTree->child->sibling->sibling->name==T_RSB){
                    r1 = check(syntaxTree->child,NULL);
                    if(r1->varType==ERR){
                        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                        if(DEBUG0) printType(r1);
                        saveType(syntaxTree,r1);
                        return r1;
                    }  
                    t =lookupAll(r1->name);
                    e = (struct type*)malloc(sizeof(struct type));
                    e->isConst = t->vartype.isConst;
                    e->ptr = t->vartype.ptr;
                    e->varType = t->vartype.varType;
                    if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                    if(DEBUG0) printType(e);
                    saveType(syntaxTree,e);
                    return e;
                }
                else{
                    r1 = check(syntaxTree->child,NULL);
                    if(r1->varType==ERR){
                        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                        if(DEBUG0) printType(r1);
                        saveType(syntaxTree,r1);
                        return r1;
                    }  
                    t =lookupAll(r1->name);
                    e = (struct type*)malloc(sizeof(struct type));
                    e->isConst = t->vartype.isConst;
                    e->ptr = t->vartype.ptr;
                    e->varType = t->vartype.varType;
                    if(!checkFuncPara(t->paraList,syntaxTree->child->sibling->sibling)){    
                        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                        if(DEBUG0) printType(e);
                        printf("line: %d func para not match !\n",syntaxTree->line);
                        saveType(syntaxTree,e);
                        return e;
                    }
                    else{
                        e->varType=ERR;
                        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                        if(DEBUG0) printType(e);
                        saveType(syntaxTree,e);
                        return e;
                    }
                    
                }
                
            }
            else{
                r = check(syntaxTree->child,NULL);
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r);
                saveType(syntaxTree,r);
                return r;
            } 
        }             
        else{
            r = check(syntaxTree->child,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;
        } 
    }
    else if(syntaxTree->name==T_exp_unary){
        if(syntaxTree->child->name==T_exp_postfix){
            r1 = check(syntaxTree->child,NULL);

            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1;
        }
        else if(syntaxTree->child->child->name==T_B_AND){
            r1 = check(syntaxTree->child->sibling,NULL);
            if(r1->varType==ERR){
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            else if(r1->isConst){
                isError=true;
                printf("line: %d, type const %s cannot be got address\n",syntaxTree->line,typeToString(r1->varType));
                r1->varType=ERR;
            }
            else
                r1->ptr++;
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1;
        }
        else if(syntaxTree->child->child->name==T_L_NOT){
            r1 = check(syntaxTree->child->sibling,NULL);
            if(r1->varType==ERR){
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            r1->varType=TYPE_BOOL;
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1;
        }
        else if(syntaxTree->child->child->name==T_STAR){
            r1 = check(syntaxTree->child->sibling,NULL);
            if(r1->varType==ERR){
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            else if(r1->ptr<=0){
                isError=true;
                printf("line: %d, type %s cannot be got address\n",syntaxTree->line,typeToString(r1->varType));
                r1->varType=ERR;
            }
            else
                r1->ptr--;
            if(DEBUG0) printf("return: %s\n",syntaxTree->name);
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1;
        }
        else{
            r=check(syntaxTree->child->sibling,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;

        } 
    }
    else if(syntaxTree->name==T_exp_cast){
        if(syntaxTree->child->name==T_LSB){
            e = (struct type*)malloc(sizeof(struct type));
            evalType(syntaxTree->child->sibling,e);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(e);
            saveType(syntaxTree,e);
            return e;
        }
        else{
            r=check(syntaxTree->child,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            saveType(syntaxTree,r);
            return r;
        }
    }
    else if(syntaxTree->name==T_exp_mul||syntaxTree->name==T_exp_additive){
        //printf("%s\n","cal");
        if(syntaxTree->child->sibling==0){
            r=check(syntaxTree->child,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;
        }
        else{
            r1 = check(syntaxTree->child,NULL);
            if(r1->ptr!=0||r1->varType==TYPE_BOOL||r1->varType==TYPE_CHAR||r1->varType==TYPE_VOID){
                isError=true;
                printf("line: %d, type error, type %s cannot be used in additive calculation!\n",syntaxTree->line,typeToString(r1->varType));
                r1->varType=ERR;
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            r2 = check(syntaxTree->child->sibling->sibling,NULL);
            if(r2->ptr!=0||r2->varType==TYPE_BOOL||r2->varType==TYPE_CHAR||r2->varType==TYPE_VOID){
                isError=true;
                printf("line: %d ,type error, type %s cannot be used in additive calculation!\n",syntaxTree->line,typeToString(r2->varType));
                r1->varType=ERR;
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            if(r1->varType==TYPE_INT && r2->varType==TYPE_INT)
                r1->varType=TYPE_INT;
            else    
                r1->varType=TYPE_DOUBLE;
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1;
        }
    }
    else if(syntaxTree->name==T_exp_shift||syntaxTree->name==T_exp_B_AND||
            syntaxTree->name==T_exp_B_XOR||syntaxTree->name==T_exp_B_OR){
        //printf("%s\n","logic cal");
        if(syntaxTree->child->sibling==0){
            r=check(syntaxTree->child,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;
        }
        else{
            r1 = check(syntaxTree->child,NULL);
            if(r1->ptr!=0){
                isError=true;
                printf("line: %d, type error, %s pointer cannot be used in bit calculation!\n",syntaxTree->line,typeToString(r1->varType));
                r1->varType=ERR;
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            else if(r1->varType!=TYPE_INT){
                isError=true;
                printf("line: %d, type error, type %s cannot be used in bit calculation!\n",syntaxTree->line,typeToString(r1->varType));
                r1->varType=ERR;
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            r2 = check(syntaxTree->child->sibling->sibling,NULL);
            if(r2->ptr!=0){
                printf("line: %d, type error, %s pointer cannot be used in bit calculation!\n",syntaxTree->line,typeToString(r2->varType));
                isError=true;
                r1->varType=ERR;            
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            else if(r2->varType!=TYPE_INT){
                printf("line: %d, type error, type %s cannot be used in bit calculation!\n",syntaxTree->line,typeToString(r1->varType));
                isError=true;
                r1->varType=ERR;            
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1;
        }
    }   
    else if(syntaxTree->name==T_exp_bool||syntaxTree->name==T_exp_EQ||
            syntaxTree->name==T_exp_L_OR||syntaxTree->name==T_exp_L_AND){
        //printf("%s\n","binary logic");
        if(syntaxTree->child->sibling==0){
            r=check(syntaxTree->child,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;
        }
        else{
            r1 = check(syntaxTree->child,NULL);
            if(r1->ptr!=0||r1->varType==TYPE_VOID){
                isError=0;
                r1->varType=ERR;
                printf("line: %d, type error, void exp cannot be used in compare calculation!\n",syntaxTree->line);
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            r2 = check(syntaxTree->child->sibling->sibling,NULL);
            if(r2->ptr!=0||r1->varType==TYPE_VOID){
                isError=true;
                r1->varType=ERR;
                printf("line: %d, type error, void exp cannot be used in compare calculation!\n",syntaxTree->line);
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            r1->varType=TYPE_BOOL;
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1;
        }
    }
    else if(syntaxTree->name==T_exp_assign){
        if(syntaxTree->child->sibling==0){
             r=check(syntaxTree->child,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r);
            saveType(syntaxTree,r);
            return r;
        }
        else{
            r1 = check(syntaxTree->child,NULL);
            if(r1->varType==ERR){
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r1);
                saveType(syntaxTree,r1);
                return r1;
            }
            r2 = check(syntaxTree->child->sibling->sibling,NULL);
            if(r2->varType==ERR){
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                if(DEBUG0) printType(r2);
                saveType(syntaxTree,r2);
                return r2;
            }
            if(syntaxTree->child->sibling->name==T_PLUS_ASSIGN||syntaxTree->child->sibling->name==T_PLUS_ASSIGN||
               syntaxTree->child->sibling->name==T_MUL_ASSIGN||syntaxTree->child->sibling->name==T_DIV_ASSIGN){
                if((r1->ptr!=0&&r2->ptr!=0)&&(r1->varType!=TYPE_INT &&r2->varType!=TYPE_INT)&&(r1->varType!=TYPE_DOUBLE &&r2->varType!=TYPE_DOUBLE)){
                    printf("line: %d, ASSIGN TYPE ERR: %s\n cannot do Arithmetic calculation",syntaxTree->line,typeToString(r1->varType));
                    isError=true;
                    r1->varType=ERR;
                    if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                    if(DEBUG0) printType(r1);
                    saveType(syntaxTree,r1);
                    return r1;
                }                 
            }
            else if(syntaxTree->child->sibling->name==T_MOD_ASSIGN||syntaxTree->child->sibling->name==T_AND_ASSIGN ||
                    syntaxTree->child->sibling->name==T_OR_ASSIGN|| syntaxTree->child->sibling->name==T_XOR_ASSIGN ||
                    syntaxTree->child->sibling->name==T_L_SHIFT_ASSIGN|| syntaxTree->child->sibling->name==T_R_SHIFT_ASSIGN){
                if((r1->ptr!=0&&r2->ptr!=0)&&r1->varType!=TYPE_INT&&r2->varType!=TYPE_INT){
                    printf("line: %d, ASSIGN TYPE ERR: %s cannot be mod\n",syntaxTree->line,typeToString(r1->varType));
                    isError=true;
                    r1->varType=ERR;
                    if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                    if(DEBUG0) printType(r1);
                    saveType(syntaxTree,r1);
                    return r1;
                }  
                             
            }
            else if(syntaxTree->child->sibling->name=T_ASSIGN){
                if(typeEqual(r1,r2)){
                    if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                    if(DEBUG0) printType(r1);
                    if(DEBUG0) printType(r2);
                    saveType(syntaxTree,r1);
                    return r1;
                }
                else{
                    
                    isError=true;  
                    printf("line: %d, ASSIGN TYPE ERR: %s cannot be assign by %s\n",syntaxTree->line,typeToString(r1->varType),typeToString(r2->varType));
                    r1->varType=ERR;
                    if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
                    if(DEBUG0) printType(r1);
                    if(DEBUG0) printType(r2);  
                    saveType(syntaxTree,r1);                
                    return r1;
                }             
            }
        }
    }
    else if(syntaxTree->name==T_return_stmt){
        r=check(syntaxTree->child->sibling,NULL);
        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
        if(DEBUG0) printType(r);
        saveType(syntaxTree,r);
        return r;      
    }
    else if(syntaxTree->name==T_exp_stmt){
        //printf("%s\n","binary logic");
        if(syntaxTree->child->sibling==0){
            e = (struct type*)malloc(sizeof(struct type));
            e->isConst = 0;
            e->name = 0;
            e->ptr = 0;
            e->varType = TYPE_VOID;
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(e);
            saveType(syntaxTree,e);
            return e;
        }
        else{
            r1 = check(syntaxTree->child,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1;
        }
    }
    else if(syntaxTree->name==T_stmt_block){
        if(syntaxTree->child->sibling==0){
            r1 = check(syntaxTree->child,NULL); 
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1; 
        }
        else{
            r1 = check(syntaxTree->child->sibling,NULL); 
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r1);
            saveType(syntaxTree,r1);
            return r1; 
        }
        
    }
    else if(syntaxTree->name==T_stmt_list){
        if(syntaxTree->child->sibling==0){
            r2 = check(syntaxTree->child,NULL);
            if(DEBUG0){printf("return: %s\n",toString(syntaxTree->name));}
            if(DEBUG0) printType(r2);
            saveType(syntaxTree,r2);
            return r2; 
        }
        else{
            r1 = check(syntaxTree->child,NULL);
            
            r2 = check(syntaxTree->child->sibling,NULL);
            //if(r1->varType==ERR) {r2->varType=ERR;}
            if(DEBUG0){ printf("return: %s\n",toString(syntaxTree->name));}
            if(DEBUG0) printType(r2);
            saveType(syntaxTree,r2);
            return r2;
            
        } 
        
    }
    else if(syntaxTree->name==T_expr){
        if(syntaxTree->child->sibling==0){
            r2 = check(syntaxTree->child,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r2);
            saveType(syntaxTree,r2);
            return r2; 
        }
        else{
            r1 = check(syntaxTree->child,NULL);
            if(r1->varType==ERR){
                if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name)); 
                if(DEBUG0) printType(r2);
                saveType(syntaxTree,r1); 
                return r1;
            }
            r2 = check(syntaxTree->child->sibling->sibling,NULL);
            if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name)); 
            if(DEBUG0) printType(r2); 
            saveType(syntaxTree,r2);
            return r2;
            
        } 
        
    }
    else if(syntaxTree->name==T_atomic_stmt){
        r2 = check(syntaxTree->child,NULL);
        if(DEBUG0) printf("return: %s\n",toString(syntaxTree->name));
        if(DEBUG0) printType(r2);
        saveType(syntaxTree,r2);
        return r2;
    }
    else{
        if(syntaxTree->child!=0){
            r1=check(syntaxTree->child,NULL);
        }            
        if(syntaxTree->sibling!=0){
            r2=check(syntaxTree->sibling,NULL);
            if(DEBUG0) printf("return: %s, ELSE r2\n",toString(syntaxTree->name));
            if(DEBUG0) printType(r2);
            saveType(syntaxTree,r2);
            return r2;  
        }         
        if(DEBUG0) printf("return: %s, ELSE r1\n",toString(syntaxTree->name));
        if(DEBUG0) printType(r1);
        saveType(syntaxTree,r1);
        return r1;   
    }
}

void evalType(struct ASTnode* syntaxTree,struct type* t){
    struct ASTnode* p = syntaxTree;
    int count=0;
    while(p->name==T_type){
        // cal level of ptr
        count++;
        p=p->child;
    }
    t->ptr=count;
    // cal is const
    p=p->child;
    t->name=0;
    if(p->name==T_CONST){
        t->isConst = true;
        p=p->sibling->child;
    }
    else{
        t->isConst = false;
        p=p->child;
    }
    // cal type
    if(p->name==T_INT)
        t->varType = TYPE_INT;
    else if(p->name==T_FLOAT)
        t->varType = TYPE_FLOAT;
    else if(p->name==T_DOUBLE)
        t->varType = TYPE_DOUBLE;
    else if(p->name==T_CHAR)
        t->varType = TYPE_CHAR;
    else if(p->name==T_LONG)
        t->varType = TYPE_LONG;
    else{ // customize type
        t->varType = TYPE_INT;
    }
    //printf("%d %d %d\n",t->isconst,t->ptr,t->varType);
}

void insert(char* name,int lineno, struct type* vartype,bool isfunc,struct funcParaList* paraList,int upperScope){
    struct symbol* t = lookup(name,symTable->next);
    struct symTableList *s = symTable;
    int i=0;
    if(t !=0){
        printf("line: %d, redeclaration of variable: %s\n",lineno,name);
    }
    else{
        for(i=0;i<upperScope;i++)
            s=s->next;
        addVar(name,lineno,vartype,isfunc,paraList,s);
    }
}

void SymCheck(struct ASTnode* syntaxTree){
    initSym();
    printf("start symbol check...\n");
    check(syntaxTree,NULL);
    printf("finish symbol check!\n");
    if(DEBUG0) printf("isError :%d\n",isError);
    return;
}



bool typeEqual(struct type* a, struct type* b){
    if(a->varType==ERR)
        return false;
    else if(b->varType==ERR)
        return false;
    else if(a->ptr != b->ptr)
        return false;
    else if(a->isConst)
        return false;
    else if(a->varType==TYPE_DOUBLE && b->varType==TYPE_INT)
        return true;
    else if(a->varType!=b->varType)
        return false;
    return true;
}

void printType(struct type* b){
    char t[10];
    switch(b->varType){
        case TYPE_BOOL:
            strcpy(t,"bool");
            break;
        case TYPE_CHAR:
            strcpy(t,"char");
            break;
        case TYPE_DOUBLE:
            strcpy(t,"double");
            break;
        case TYPE_FLOAT:
            strcpy(t,"flaot");
            break;
        case TYPE_INT:
            strcpy(t,"int");
            break;
        case TYPE_LONG:
            strcpy(t,"long");
            break;
        case TYPE_VOID:
            strcpy(t,"void");
            break;
        case ERR:
            strcpy(t,"err");
            break;
        default:
            strcpy(t,"bug");
            break;
    }
    if(b==0)
        printf("null ptr\n");
    else
        printf("ptr=%d isConst=%d type=%s\n",b->ptr,b->isConst,t);
}

char* typeToString(BasicType t){
    switch(t){
        case TYPE_BOOL:
            return "bool";
        case TYPE_CHAR:
            return "char";
        case TYPE_DOUBLE:
            return "double";
        case TYPE_FLOAT:
            return "flaot";
        case TYPE_INT:
            return "int";
        case TYPE_LONG:
            return "long";
        case TYPE_VOID:
            return "void";
        case ERR:
            return "err";
        default:
            return "bug";
    }
}

void printTable(){
    
    int i=0;
    struct symbol** p = symTable->table;
    struct symbol* t=0;
    struct type* para=0;
    struct funcParaList* q=0;
    for(i=0;i<SYMTABLE_SIZE;i++){
        t=p[i];
        if(t==0)
            continue;
        printf("%s ",t->vartype.name);
        printType(&(t->vartype));
        if(t->isfunc){           
            q = t->paraList;
            if(q==NULL)
                printf("parameter: void\n");
            while(q!=NULL){
                para = q->para;
                printf("%s ",para->name);
                printType(para);
                q=q->next;
            }
        }
            
                
    }
    
}

void saveType(struct ASTnode* node,struct type* nodeType){
    node->nodeType = (struct type*)malloc(sizeof(struct type));
    node->nodeType->isConst=nodeType->isConst;
    node->nodeType->name=nodeType->name;
    node->nodeType->ptr=nodeType->ptr;
    node->nodeType->varType=nodeType->varType;
    
    return;
}

bool checkFuncPara(struct funcParaList* paraList,struct ASTnode* tree){
    struct ASTnode* c = tree;
    struct funcParaList* p = paraList;
    struct type* r;
    c=c->child;
    while(c->name==T_expr){
        r = check(c->sibling->sibling,NULL);
        if(DEBUG0) printf("in check Func\n");
        if(DEBUG0) printType(r);
        if(DEBUG0) printType(p->para);
        if(!typeEqual(r,p->para)){
            r->varType=ERR;
            saveType(tree,r);
            return false;
        }
        p=p->next;
        c=c->child;
    }
    r = check(c,NULL);
    if(DEBUG0) printf("in check Func\n");
    if(DEBUG0) printType(r);
    if(DEBUG0) printType(p->para);
    if(!typeEqual(r,p->para)){
        r->varType=ERR;
        saveType(tree,r);
        return false;
    }
    return true;
}


char* toString(NonTerminal name){
    switch (name)
    {
    case T_program:
        return "program";
    case T_global_list:
        return "global_list";
    case T_global:
        return "global";
    case T_global_var_stmt:
        return "global_var_stmt";
    case T_func_stmt:
        return "func_stmt";
    case T_para_list:
        return "para_list";
    case T_para:
        return "para";
    case T_local_var_stmt:
        return "local_var_stmt";
    case T_local_var_list:
        return "local_var_list";
    case T_var_declar:
        return "var_declar";
    case T_stmt_block:
        return "stmt_block";
    case T_stmt_list:
        return "stmt_list";
    case T_atomic_stmt:
        return "atomic_stmt";
    case T_return_stmt:
        return "return_stmt";
    case T_iter_stmt:
        return "iter_stmt";
    case T_cond_stmt:
        return "cond_stmt";
    case T_else_stmt:
        return "else_stmt";
    case T_exp_stmt:
        return "exp_stmt";
    case T_expr:
        return "expr";
    case T_exp_assign:
        return "exp_assign";
    case T_exp_L_OR:
        return "exp_L_OR";
    case T_exp_L_AND:
        return "exp_L_AND";
    case T_exp_B_OR:
        return "exp_B_OR";
    case T_exp_B_XOR:
        return "exp_B_XOR";
    case T_exp_B_AND:
        return "exp_B_AND";
    case T_exp_EQ:
        return "exp_EQ";
    case T_exp_bool:
        return "exp_bool";
    case T_exp_shift:
        return "exp_shift";
    case T_exp_additive:
        return "exp_additive";
    case T_exp_mul:
        return "exp_mul";
    case T_exp_cast:
        return "exp_cast";
    case T_exp_unary:
        return "exp_unary";
    case T_unary_op:
        return "unary_op";
    case T_exp_postfix:
        return "exp_postfix";
    case T_exp_atomic:
        return "exp_atomic";
    case T_type:
        return "type";
    case T_type_const:
        return "type_const";
    case T_type_primary:
        return "type_primary";
    case T_REAL:
        return "REAL";
    case T_LSB:
        return "LSB";
    case T_RSB:
        return "RSB";
    case T_LMB:
        return "LMB";
    case T_RMB:
        return "RMB";
    case T_LLB:
        return "LLB";
    case T_RLB:
        return "RLB";
    case T_DOT:
        return "DOT";
    case T_STAR:
        return "STAR";
    case T_COMMA:
        return "COMMA";
    case T_COLON:
        return "COLON";
    case T_L_NOT:
        return "L_NOT";
    case T_B_NOT:
        return "B_NOT";
    case T_INC:
        return "INC";
    case T_DEC:
        return "DEC";
    case T_PLUS_ASSIGN:
        return "PLUS_ASSIGN";
    case T_SUB_ASSIGN:
        return "SUB_ASSIGN";
    case T_MUL_ASSIGN:
        return "MUL_ASSIGN";
    case T_DIV_ASSIGN:
        return "DIV_ASSIGN";
    case T_MOD_ASSIGN:
        return "MOD_ASSIGN";
    case T_AND_ASSIGN:
        return "AND_ASSIGN";
    case T_OR_ASSIGN:
        return "OR_ASSIGN";
    case T_XOR_ASSIGN:
        return "XOR_ASSIGN";
    case T_L_SHIFT_ASSIGN:
        return "L_SHIFT_ASSIGN";
    case T_R_SHIFT_ASSIGN:
        return "R_SHIFT_ASSIGN";
    case T_L_SHIFT:
        return "L_SHIFT";
    case T_R_SHIFT:
        return "R_SHIFT";
    case T_PLUS:
        return "PLUS";
    case T_MINUS:
        return "MINUS";
    case T_DIV:
        return "DIV";
    case T_MOD:
        return "MOD";
    case T_GE:
        return "GE";
    case T_GT:
        return "GT";
    case T_LE:
        return "LE";
    case T_LT:
        return "LT";
    case T_EQ:
        return "EQ";
    case T_NEQ:
        return "NEQ";
    case T_ASSIGN:
        return "ASSIGN";
    case T_B_AND:
        return "B_AND";
    case T_L_AND:
        return "L_AND";
    case T_B_OR:
        return "B_OR";
    case T_L_OR:
        return "L_OR";
    case T_B_XOR:
        return "B_XOR";
    case T_SEMI:
        return "SEMI";
    case T_ARROW:
        return "ARROW";
    case T_IF:
        return "IF";
    case T_ELSE:
        return "ELSE";
    case T_SWITCH:
        return "SWITCH";
    case T_CASE:
        return "CASE";
    case T_DEFAULT:
        return "DEFAULT";
    case T_DO:
        return "DO";
    case T_WHILE:
        return "WHILE";
    case T_FOR:
        return "FOR";
    case T_BREAK:
        return "BREAK";
    case T_CONTINUE:
        return "CONTINUE";
    case T_CONST:
        return "CONST";
    case T_INT:
        return "INT";
    case T_FLOAT:
        return "FLOAT";
    case T_DOUBLE:
        return "DOUBLE";
    case T_CHAR:
        return "CHAR";
    case T_LONG:
        return "LONG";
    case T_VOID:
        return "VOID";
    case T_GOTO:
        return "GOTO";
    case T_STRUCT:
        return "STRUCT";
    case T_STATIC:
        return "STATIC";
    case T_SIZEOF:
        return "SIZEOF";
    case T_RETURN:
        return "RETURN";
    case T_NAME:
        return "NAME";
    case T_INTEGER:
        return "INTEGER";
    default:
        return "bug";
    }
}