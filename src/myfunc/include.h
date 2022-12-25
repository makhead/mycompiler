#ifndef INCLUDE
#define INCLUDE
#define SYMTABLE_SIZE 107
#define SHIFT_AMOUNT 2
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
enum basicType {TYPE_VOID, TYPE_INT, TYPE_CHAR, TYPE_DOUBLE, TYPE_LONG, TYPE_FLOAT, TYPE_BOOL, ERR};
enum nonterminal {T_program, T_global_list, T_global, T_global_var_stmt, T_func_stmt, T_para_list, T_para, T_local_var_stmt,
                T_local_var_list, T_var_declar, T_stmt_block, T_stmt_list, T_atomic_stmt, T_return_stmt,
                T_iter_stmt, T_cond_stmt,T_else_stmt, T_exp_stmt, T_expr, T_exp_assign, T_exp_L_OR, T_exp_L_AND, T_exp_B_OR,
                T_exp_B_XOR, T_exp_B_AND, T_exp_EQ, T_exp_bool, T_exp_shift, T_exp_additive, T_exp_mul, T_exp_cast,
                T_exp_unary, T_unary_op,T_exp_postfix , T_exp_atomic, T_type,T_type_const,T_type_primary,
                
                T_REAL,T_LSB,T_RSB,T_LMB,T_RMB,T_LLB,T_RLB,T_DOT,T_STAR,T_COMMA,T_COLON,T_L_NOT,
                T_B_NOT,T_INC,T_DEC,T_PLUS_ASSIGN,T_SUB_ASSIGN,T_MUL_ASSIGN,T_DIV_ASSIGN,
                T_MOD_ASSIGN,T_AND_ASSIGN,T_OR_ASSIGN,T_XOR_ASSIGN,T_L_SHIFT_ASSIGN,T_R_SHIFT_ASSIGN,
                T_L_SHIFT,T_R_SHIFT,T_PLUS,T_MINUS,T_DIV,T_MOD,T_GE,T_GT,T_LE,T_LT,T_EQ,T_NEQ,
                T_ASSIGN,T_B_AND,T_L_AND,T_B_OR,T_L_OR,T_B_XOR,T_SEMI,T_ARROW,T_IF,T_ELSE,T_SWITCH,
                T_CASE,T_DEFAULT,T_DO,T_WHILE,T_FOR,T_BREAK,T_CONTINUE,T_CONST,T_INT,T_FLOAT,
                T_DOUBLE,T_CHAR,T_LONG,T_VOID,T_GOTO,T_STRUCT,T_STATIC,T_SIZEOF,T_RETURN,T_NAME,
                T_INTEGER
                 };

typedef enum basicType BasicType;
typedef enum nonterminal NonTerminal;
bool isError;
struct type{
    char* name;
    int ptr;
    bool isConst;
    BasicType varType;  
};

struct ASTnode
{
    NonTerminal name;
    char* value;
	bool isTerminal;
    struct type* nodeType;
    int line;
    struct ASTnode* child;
    struct ASTnode* sibling;
    /***
     * 0: no semantic
     * 1: type semantic(containing type informantion) 
     * 2: int-value semantic(containing int-value informantion)
     * 3: floating-point-value semantic(containing floating-point-value informantion)
     * 4: symbol semantic(containing symbol informantion)
    ***/
    struct S_block
    {
        int semantic_mode;
        union semantic {
            BasicType typeInfo;
            int intValue;
            double doubleValue;
            char* symbolName;
        } semantic_value;
    } S_block;
};

struct symbol{
 //   char* var_name;
    struct type vartype;
    bool isfunc;
    struct funcParaList* paraList;
    int lineno;
    struct symbol* next;
};

struct symTableList{
    struct symbol* table[SYMTABLE_SIZE];
    struct symTableList* next;
};

struct funcParaList{
    struct type* para;
    struct funcParaList* next;
};


struct ASTnode* root;
struct symTableList* symTable;



#endif