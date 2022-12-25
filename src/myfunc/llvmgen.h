#ifndef LLVMGEN_H
#define LLVMGEN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include.h"
#include "symbol.h"

#define MAX_VREG_NUM 10000
#define MAX_FUNC_ARGS 10
#define MAX_SECTIONS 1000
#define STACK_SIZE 1000
#define MAX_NONT_NUM 10

enum action {Load, Store};

union exp_value {
    int constant;
    char symbol[30];
};

extern void llvmgen_init();
extern void llvmTraverse(struct ASTnode* node);
extern void FuncBegin(struct symbol* func);
extern void FuncEnd();
extern void VarDeclar(NonTerminal cur_type, char* cur_sym);
extern void start_scope(union exp_value res, unsigned char mode);
extern void enter_while();
extern void else_scope();
extern int end_scope(int is_else, int is_iter);
extern void refine_label();
extern void refine_else_label();
extern void rw_var(union exp_value* target, enum action a,
            union exp_value* res_to_store, int mode);
extern void write_return(union exp_value* res, int mode);
extern void write_call(union exp_value* res, int* index, struct symbol* func,
                int cur_op, int cnt);
extern void examine_unary_exp(union exp_value* res, NonTerminal operators,
                    union exp_value opa, unsigned char mode);
extern void examine_binary_exp(union exp_value* res, NonTerminal operators,
                    union exp_value opa, union exp_value opb,
                    unsigned char mode);
extern void expandExpNode(struct ASTnode* node, struct ASTnode** op_stack,
                            int* parent, int* preced);
extern void doExpand(struct ASTnode* node, struct ASTnode** op_stack, int* preced);
extern union exp_value calculateExp(struct ASTnode** op_stack, int* parent, int preced);
extern void llvmgen_finalize();

#endif