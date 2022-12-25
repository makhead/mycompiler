%{
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "myfunc/ASTtree.h"
#include "myfunc/symbol.h"
#include "myfunc/include.h"
#include "myfunc/llvmgen.h"

extern FILE *yyin;
void yyerror(const char *s);
int yylex(void);

int yydebug = 1;
extern bool isError;
extern struct ASTnode* root;
extern struct symTableList* symTable;
//==========================================================================================

%}
%union{
	struct ASTnode* tn;
};

%token<tn> LSB RSB LMB RMB LLB RLB DOT ARROW STAR COMMA COLON L_NOT B_NOT PLUS MINUS DIV MOD GE GT LE LT
%token<tn>EQ NEQ ASSIGN B_AND L_AND B_OR L_OR B_XOR SEMI 
%token<tn>INC DEC
%token<tn>PLUS_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN OR_ASSIGN XOR_ASSIGN L_SHIFT_ASSIGN R_SHIFT_ASSIGN
%token<tn>L_SHIFT R_SHIFT
%token<tn>IF ELSE SWITCH CASE DEFAULT
%token<tn>DO WHILE FOR BREAK CONTINUE
%token<tn>CONST INT FLOAT DOUBLE CHAR LONG
%token<tn>VOID
%token<tn>GOTO STRUCT STATIC SIZEOF RETURN
%token<tn>NAME INTEGER REAL

%type<tn>program global_list global global_var_stmt func_stmt para_list para local_var_stmt
%type<tn>local_var_list var_declar stmt_block stmt_list atomic_stmt return_stmt iter_stmt cond_stmt
%type<tn>else_stmt exp_stmt exp exp_assign exp_L_OR exp_L_AND exp_B_OR
%type<tn>exp_B_XOR exp_B_AND exp_EQ exp_bool exp_shift exp_additive exp_mul exp_cast exp_unary unary_op
%type<tn>exp_postfix exp_atomic type 
%type<tn> type_const
%type<tn>type_primary

%nonassoc THEN
%nonassoc ELSE
%%


program: global_list { printf("program->global_list\n");
						$$=addNode(T_program,1,$1);
						root=$$;
						}
;

global_list: global_list global { printf("global_list->global_list global\n");
								$$=addNode(T_global_list,2,$1,$2);
								}

| global { printf("global_list->global\n");
			$$=addNode(T_global_list,1,$1);
			}
;

global: global_var_stmt { printf("global->global_var_stmt\n");
						$$=addNode(T_global,1,$1);
						}
| func_stmt { printf("global->func_stmt\n");
			$$=addNode(T_global,1,$1);
			}
;

global_var_stmt: type local_var_list SEMI { printf("global_var_stmt->type global_var_list SEMI\n");
											$$=addNode(T_global_var_stmt,3,$1,$2,$3);
											}
;

func_stmt: type NAME LSB para_list RSB stmt_block { printf("func_stmt->type NAME LSB para_list RSB stmt_block\n");
													$$=addNode(T_func_stmt,6,$1,$2,$3,$4,$5,$6);
													}
| type NAME LSB RSB stmt_block { printf("func_stmt->type NAME LSB para_list RSB stmt_block\n");
								$$=addNode(T_func_stmt,5,$1,$2,$3,$4,$5);
								}
;

para_list: para_list COMMA para { printf("para_list->para_list COMMA para\n");
								$$=addNode(T_para_list,3,$1,$2,$3);
								}
| para { printf("para_list->para\n");
		$$=addNode(T_para_list,1,$1);
		}
;

para: type NAME { printf("para->type NAME\n");
				$$=addNode(T_para,2,$1,$2);
				}
;

local_var_stmt: type local_var_list SEMI { printf("local_var_stmt->type local_var_list SEMI\n");
											$$=addNode(T_local_var_stmt,3,$1,$2,$3);
											}
;

local_var_list: local_var_list COMMA var_declar { printf("local_var_list->local_var_list var_declar\n");
												$$=addNode(T_local_var_list,3,$1,$2,$3);
												}
| var_declar { printf("local_var_list->var_declar\n");
			 	$$=addNode(T_local_var_list,1,$1);
			 }
;

var_declar: NAME { printf("var_declar->NAME\n");
					$$=addNode(T_var_declar,1,$1);
					}
| NAME ASSIGN exp_L_OR { printf("var_declar->NAME ASSIGN exp_assign\n");
					$$=addNode(T_var_declar,3,$1,$2,$3);
					}
| NAME LMB INTEGER RMB { printf("var_declar->NAME LMB INTEGER RMB\n");
					$$=addNode(T_var_declar,4,$1,$2,$3,$4);
					}
;

stmt_block: LLB stmt_list RLB {printf("stmt_block->LLB stmt_list RLB\n");
								$$=addNode(T_stmt_block,3,$1,$2,$3);
								}
| atomic_stmt {printf("stmt_block->atomic_stmt\n");
				$$=addNode(T_stmt_block,1,$1);
				}
;

stmt_list: stmt_list atomic_stmt { printf("stmt_list->stmt_list atomic_stmt\n");
								 	$$=addNode(T_stmt_list,2,$1,$2);
								 }
| atomic_stmt { printf("stmt_list->atomic_stmt\n"); 
				$$=addNode(T_stmt_list,1,$1);
				}
;

atomic_stmt: exp_stmt	{ printf("atomic_stmt->exp_stmt\n");
						$$=addNode(T_atomic_stmt,1,$1);
						}
| iter_stmt	{ printf("atomic_stmt->iter_stmt\n");
			$$=addNode(T_atomic_stmt,1,$1);
			}
| cond_stmt { printf("atomic_stmt->cond_stmt\n");
			$$=addNode(T_atomic_stmt,1,$1);
			}
| local_var_stmt { printf("atomic_stmt->local_var_stmt\n");
					$$=addNode(T_atomic_stmt,1,$1);
					}
| return_stmt { printf("atomic_stmt->return_stmt\n");
				$$=addNode(T_atomic_stmt,1,$1);
				}
;

return_stmt: RETURN exp_stmt { printf("return_stmt->RETURN\n");
								$$=addNode(T_return_stmt,2,$1,$2);
								}
;

iter_stmt:	FOR LSB exp SEMI exp SEMI exp RSB stmt_block {
				printf("iter_stmt->FOR LSB exp SEMI exp SEMI exp RSB stmt_block\n");
				$$=addNode(T_iter_stmt,9,$1,$2,$3,$4,$5,$6,$7,$8,$9);
			}
| FOR LSB local_var_stmt exp SEMI exp RSB stmt_block {
				printf("iter_stmt->FOR LSB exp SEMI exp SEMI exp RSB stmt_block\n");
				$$=addNode(T_iter_stmt,8,$1,$2,$3,$4,$5,$6,$7,$8);
			}
| WHILE LSB exp RSB stmt_block { printf("iter_stmt->WHILE LSB exp RSB stmt_block\n"); 
								$$=addNode(T_iter_stmt,5,$1,$2,$3,$4,$5);
								}
| DO stmt_block WHILE LSB exp RSB SEMI { printf("iter_stmt->DO stmt_block WHILE LSB exp RSB SEMI\n");
										$$=addNode(T_iter_stmt,7,$1,$2,$3,$4,$5,$6,$7);
										}
;

cond_stmt:	IF LSB exp RSB else_stmt {
				printf("cond_stmt->IF LSB exp RSB else_stmt\n");
				$$=addNode(T_cond_stmt,5,$1,$2,$3,$4,$5);
			}
;

else_stmt:	stmt_block %prec THEN { printf("else_stmt->stmt_block\n");
						$$=addNode(T_else_stmt,1,$1);
						}
| stmt_block ELSE stmt_block { printf("else_stmt->stmt_block ELSE stmt_block\n");
					$$=addNode(T_else_stmt,3,$1,$2,$3);
					}
;

exp_stmt: SEMI {printf("exp_stmt->SEMI\n");
				$$=addNode(T_exp_stmt,1,$1);
				}
| exp SEMI {printf("exp_stmt->exp SEMI\n");
			$$=addNode(T_exp_stmt,2,$1,$2);
			}
;

exp: exp COMMA exp_assign {printf("exp->exp COMMA exp_assign\n");
							$$=addNode(T_expr,3,$1,$2,$3);
							}
| exp_assign {printf("exp->exp_assign\n");
				$$=addNode(T_expr,1,$1);
				}
;

exp_assign: exp_unary ASSIGN exp_assign {printf("exp_assign->exp_unary ASSIGN exp_assign\n");
										$$=addNode(T_exp_assign,3,$1,$2,$3);
										}
| exp_unary PLUS_ASSIGN exp_assign {printf("exp_assign->exp_unary PLUS_ASSIGN exp_assign\n");
									$$=addNode(T_exp_assign,3,$1,$2,$3);
									}
| exp_unary SUB_ASSIGN exp_assign {printf("exp_assign->exp_unary SUB_ASSIGN exp_assign\n");
									$$=addNode(T_exp_assign,3,$1,$2,$3);
									}
| exp_unary MUL_ASSIGN exp_assign {printf("exp_assign->exp_unary MUL_ASSIGN exp_assign\n");
									$$=addNode(T_exp_assign,3,$1,$2,$3);
									}
| exp_unary DIV_ASSIGN exp_assign {printf("exp_assign->exp_unary DIV_ASSIGN exp_assign\n");
									$$=addNode(T_exp_assign,3,$1,$2,$3);
									}
| exp_unary MOD_ASSIGN exp_assign {printf("exp_assign->exp_unary MOD_ASSIGN exp_assign\n");
									$$=addNode(T_exp_assign,3,$1,$2,$3);
									}
| exp_unary AND_ASSIGN exp_assign {printf("exp_assign->exp_unary AND_ASSIGN exp_assign\n");
									$$=addNode(T_exp_assign,3,$1,$2,$3);
									}
| exp_unary OR_ASSIGN exp_assign {printf("exp_assign->exp_unary OR_ASSIGN exp_assign\n");
									$$=addNode(T_exp_assign,3,$1,$2,$3);
									}
| exp_unary XOR_ASSIGN exp_assign {printf("exp_assign->exp_unary XOR_ASSIGN exp_assign\n");
									$$=addNode(T_exp_assign,3,$1,$2,$3);
									}
| exp_unary L_SHIFT_ASSIGN exp_assign {printf("exp_assign->exp_unary L_SHIFT_ASSIGN exp_assign\n");
										$$=addNode(T_exp_assign,3,$1,$2,$3);
										}
| exp_unary R_SHIFT_ASSIGN exp_assign {printf("exp_assign->exp_unary R_SHIFT_ASSIGN exp_assign\n");
										$$=addNode(T_exp_assign,3,$1,$2,$3);
										}
| exp_L_OR {printf("exp_assign->exp_L_OR\n");
			$$=addNode(T_exp_assign,1,$1);
			}
; 

exp_L_OR: exp_L_OR L_OR exp_L_AND {printf("exp_L_OR->exp_L_OR L_OR exp_L_AN\n");
									$$=addNode(T_exp_L_OR,3,$1,$2,$3);
									}
| exp_L_AND {printf("exp_L_OR->exp_L_AND\n");
			$$=addNode(T_exp_L_OR,1,$1);
			}
;

exp_L_AND: exp_L_AND L_AND exp_B_OR {printf("exp_L_AND->exp_L_AND L_AND exp_B_OR\n");
									$$=addNode(T_exp_L_AND,3,$1,$2,$3);
									}
| exp_B_OR {printf("exp_L_AND->exp_B_OR\n");
			$$=addNode(T_exp_L_AND,1,$1);
			}
;

exp_B_OR: exp_B_OR B_OR exp_B_XOR {printf("exp_B_OR->exp_B_OR B_OR exp_B_XOR\n");
									$$=addNode(T_exp_B_OR,3,$1,$2,$3);
									}
| exp_B_XOR {printf("exp_B_OR->exp_B_XOR\n");
			$$=addNode(T_exp_B_OR,1,$1);
			}
;

exp_B_XOR: exp_B_XOR B_XOR exp_B_AND {printf("exp_B_XOR->exp_B_XOR B_XOR exp_B_AND\n");
										$$=addNode(T_exp_B_XOR,3,$1,$2,$3);
										}
| exp_B_AND {printf("exp_B_OR->exp_B_AND\n");
			$$=addNode(T_exp_B_XOR,1,$1);
			}
;

exp_B_AND: exp_B_AND B_AND exp_EQ {printf("exp_B_AND->exp_B_AND B_AND exp_EQ\n");
									$$=addNode(T_exp_B_AND,3,$1,$2,$3);
									}
| exp_EQ {printf("exp_B_AND->exp_EQ\n");
			$$=addNode(T_exp_B_AND,1,$1);
			}
;

exp_EQ: exp_EQ EQ exp_bool {printf("exp_EQ->exp_EQ EQ exp_bool\n");
							$$=addNode(T_exp_EQ,3,$1,$2,$3);
							}
| exp_EQ NEQ exp_bool {printf("exp_EQ->exp_EQ NEQ exp_bool\n");
						$$=addNode(T_exp_EQ,3,$1,$2,$3);
						}
| exp_bool {printf("exp_EQ->exp_bool\n");
			$$=addNode(T_exp_EQ,1,$1);
			}
;

exp_bool: exp_bool GT exp_shift {printf("exp_bool->exp_bool GT exp_shift\n");
								$$=addNode(T_exp_bool,3,$1,$2,$3);
								}
| exp_bool GE exp_shift {printf("exp_bool->exp_bool GE exp_shift\n");
						$$=addNode(T_exp_bool,3,$1,$2,$3);
						}
| exp_bool LT exp_shift {printf("exp_bool->exp_bool LT exp_shift\n");
						$$=addNode(T_exp_bool,3,$1,$2,$3);
						}
| exp_bool LE exp_shift {printf("exp_bool->exp_bool LE exp_shift\n");
						$$=addNode(T_exp_bool,3,$1,$2,$3);
						}
| exp_shift {printf("exp_bool->exp_shift\n");
			$$=addNode(T_exp_bool,1,$1);
			}
;

exp_shift: exp_shift L_SHIFT exp_additive {printf("exp_shift->exp_bool L_SHIFT exp_additive\n");
											$$=addNode(T_exp_shift,3,$1,$2,$3);
											}
| exp_shift R_SHIFT exp_additive {printf("exp_shift->exp_bool R_SHIFT exp_additive\n");
									$$=addNode(T_exp_shift,3,$1,$2,$3);
									}
| exp_additive {printf("exp_shift->exp_additive\n");
				$$=addNode(T_exp_shift,1,$1);
				}
;

exp_additive: exp_additive PLUS exp_mul {printf("exp_additive->exp_additive + exp_mul\n");
										$$=addNode(T_exp_additive,3,$1,$2,$3);
										}
| exp_additive MINUS exp_mul {printf("exp_additive->exp_additive - exp_mul\n");
								$$=addNode(T_exp_additive,3,$1,$2,$3);
								}
| exp_mul {printf("exp_additive->exp_mul\n");
			$$=addNode(T_exp_additive,1,$1);
			}
;


exp_mul: exp_mul STAR exp_cast {printf("exp_mul-> exp_mul * exp_cast\n");
								$$=addNode(T_exp_mul,3,$1,$2,$3);
								}
| exp_mul DIV exp_cast {printf("exp_mul->exp_mul / exp_cast\n");
						$$=addNode(T_exp_mul,3,$1,$2,$3);
						}
| exp_mul MOD exp_cast {printf("exp_mul->exp_mul %% exp_cast\n");
						$$=addNode(T_exp_mul,3,$1,$2,$3);
						}
| exp_cast {printf("exp_mul->exp_cast\n");
			$$=addNode(T_exp_mul,1,$1);
			}
; 


exp_cast: LSB type RSB exp_unary {printf("exp_cast->LSB type RSB exp_unary\n");
									$$=addNode(T_exp_postfix,4,$1,$2,$3,$4);
									}
| exp_unary {printf("exp_cast->exp_unary\n");
			$$=addNode(T_exp_cast,1,$1);
			}
;

exp_unary: unary_op exp_unary {printf("exp_unary->unary_op exp_unary\n");
								$$=addNode(T_exp_unary,2,$1,$2);
								}
| exp_postfix {printf("exp_unary->exp_postfix\n");
				$$=addNode(T_exp_unary,1,$1);
				}
;

unary_op: PLUS {printf("unary_op->+\n");
				$$=addNode(T_unary_op,1,$1);
				}
| MINUS {printf("unary_op->-\n");
		$$=addNode(T_unary_op,1,$1);
		}
| B_AND {printf("unary_op->&\n");
		$$=addNode(T_unary_op,1,$1);
		}
| L_NOT {printf("unary_op->!\n");
		$$=addNode(T_unary_op,1,$1);
		}
| B_NOT {printf("unary_op->~\n");
		$$=addNode(T_unary_op,1,$1);
		}
| STAR {printf("unary_op->*\n");
		$$=addNode(T_unary_op,1,$1);
		}
| INC {printf("unary_op->++\n");
		$$=addNode(T_unary_op,1,$1);
		}
| DEC {printf("unary_op->--\n");
		$$=addNode(T_unary_op,1,$1);
		}
;

exp_postfix: exp_postfix LMB exp RMB {printf("exp_postfix->exp_postfix LMB exp RMB\n");
										$$=addNode(T_exp_postfix,3,$1,$2,$3);
										}
| exp_postfix LSB RSB {printf("exp_postfix->exp_postfix LSB RSB\n");
						$$=addNode(T_exp_postfix,3,$1,$2,$3);
						}
| exp_postfix LSB exp RSB {printf("exp_postfix->exp_postfix LSB exp RSB\n");
						$$=addNode(T_exp_postfix,4,$1,$2,$3,$4);
						}
| exp_postfix DOT NAME {printf("exp_postfix->exp_postfix DOT NAME\n");
						$$=addNode(T_exp_postfix,3,$1,$2,$3);
						}
| exp_postfix ARROW NAME {printf("exp_postfix->exp_postfix ARROW NAME\n");
						$$=addNode(T_exp_postfix,3,$1,$2,$3);
						}
| exp_postfix INC {$$=$1+1;printf("exp_postfix->exp_postfix INC\n");
						$$=addNode(T_exp_postfix,2,$1,$2);
						}
| exp_postfix DEC {$$=$1-1;printf("exp_postfix->exp_postfix DEC\n");
						$$=addNode(T_exp_postfix,2,$1,$2);
						}
| exp_atomic {printf("exp_postfix->exp_atomic\n");
						$$=addNode(T_exp_postfix,1,$1);
						}
;

exp_atomic: LSB exp RSB {printf("exp_atomic->LSB exp RSB\n");
						$$=addNode(T_exp_atomic,3,$1,$2,$3);
						}
| INTEGER {printf("exp_atomic->INTEGER\n");
						$$=addNode(T_exp_atomic,1,$1);
						}
| REAL {printf("exp_atomic->REAL\n");
						$$=addNode(T_exp_atomic,1,$1);
						}
| NAME {printf("exp_atomic->NAME\n");
						$$=addNode(T_exp_atomic,1,$1);
						}
;



type: type STAR {printf("type->type STAR\n");
				$$=addNode(T_type,2,$1,$2);
				}
| type_const {printf("type->type_const\n");
				$$=addNode(T_type,1,$1);}
;

type_const: CONST type_primary {printf("type_const->CONST type_primary\n");
								$$ = addNode(T_type_const,2,$1,$2);
								}
| type_primary {printf("type_const->type_primary\n");
				$$ = addNode(T_type_const,1,$1);
				}
;

type_primary: VOID {printf("type_primary->VOID\n");
					$$=addNode(T_type_primary,1,$1);
					}
| INT {printf("type_primary->INT\n");
		$$=addNode(T_type_primary,1,$1);
		}
| FLOAT {printf("type_primary->FLOAT\n");
		$$=addNode(T_type_primary,1,$1);
		}
| CHAR {printf("type_primary->CHAR\n");
		$$=addNode(T_type_primary,1,$1);
		}
| LONG {printf("type_primary->LONG\n");
		$$=addNode(T_type_primary,1,$1);
		}
| DOUBLE {printf("type_primary->DOUBLE\n");
			$$=addNode(T_type_primary,1,$1);
			}
;

%%

void yyerror(const char *s)
{
	printf("Syntax Error\n");
}

int main(int argc, char **argv)
{
	yyin = fopen("testdata.c", "r");
//	return yyparse();
//	yylex();
//	return 0;
	yyparse();
	FILE *fp;
	fp=fopen("ASTtree.txt","w+");
	//traverlShort(root,0,fp);
	/* traverlTree(root,0,fp); */
	SymCheck(root);
	printTable();
	traverlTree(root,0,fp);
	/*
	if(!isError)
		traverlDebug(root,0);
	*/
	printf("finish print sym table\n");
	llvmgen_init();
	llvmTraverse(root);
	llvmgen_finalize();
}












//========================================================================================
