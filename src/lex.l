%{
#include<string.h>
#include "yacc.tab.h" 
#include <stdlib.h>
#include "myfunc/ASTtree.h"

int line=1;
void yyerror(const char *s);
//struct ASTnode* newNode(char* name, char* value);

%}

DEFINE_decl "#define"|"#DEFINE"
INCLUDE_decl "#include"

INTEGER [0-9]+
SIGNEDINT [-+]?{INTEGER}
REAL {SIGNEDINT}("."{INTEGER})([eE]{SIGNEDINT})?
NAME [a-zA-Z_][_a-zA-Z0-9]*
C_BLOCK_COMMENT \/\*([^*]|[\n]|\*+([^*/]|[\n]))*\*+\/
C_LINE_COMMENT \/[/]+.*

%%
{REAL} { printf("REAL\n"); 
		yylval.tn =  newNode(T_REAL,yytext,line);
		return REAL;
	}
"(" {printf("LSB\n");
	yylval.tn=newNode(T_LSB,"(",line);
	return LSB;
	}
")" {printf("RSB\n");
	yylval.tn=newNode(T_RSB,")",line);
	return RSB;
	}
"[" {printf("LMB\n");
	yylval.tn=newNode(T_LMB,"[",line);
	return LMB;
	}
"]" {printf("RMB\n");
	yylval.tn=newNode(T_RMB,"]",line);
	return RMB;
	}
"{" {printf("LLB\n");
	yylval.tn=newNode(T_LLB,"{",line);
	return LLB;
	}
"}" {printf("RLB\n");
	yylval.tn=newNode(T_RLB,"}",line);
	return RLB;
	}
"." {printf("DOT\n");
	yylval.tn=newNode(T_DOT,".",line);
	return DOT;
	}
"*" {printf("STAR\n");
	yylval.tn=newNode(T_STAR,"*",line);
	return STAR;
	}
"," {printf("COMMA\n");
	yylval.tn=newNode(T_COMMA,",",line);
	return COMMA;
	}
":" {printf("COLON\n");
	yylval.tn=newNode(T_COLON,":",line);
	return COLON;
	}
"!" {printf("L_NOT\n");
	yylval.tn=newNode(T_L_NOT,"!",line);
	return L_NOT;
	}
"~" {printf("B_NOT\n");
	yylval.tn=newNode(T_B_NOT,"~",line);
	return B_NOT;
	}
"++" {printf("INC\n");
	yylval.tn=newNode(T_INC,"++",line);
	return INC;
	}
"--" {printf("INC\n");
	yylval.tn=newNode(T_DEC,"--",line);
	return DEC;
	}
"+=" {printf("PLUS_ASSIGN\n");
	yylval.tn=newNode(T_PLUS_ASSIGN,"+=",line);
	return PLUS_ASSIGN;
	}
"-=" {printf("SUB_ASSIGN\n");
	yylval.tn=newNode(T_SUB_ASSIGN,"-=",line);
	return SUB_ASSIGN;
	}
"*=" {printf("MUL_ASSIGN\n");
	yylval.tn=newNode(T_MUL_ASSIGN,"*=",line);
	return MUL_ASSIGN;
	}
"/=" {printf("DIV_ASSIGN\n");
	yylval.tn=newNode(T_DIV_ASSIGN,"/=",line);
	return DIV_ASSIGN;
	}
"%=" {printf("MOD_ASSIGN\n");
	yylval.tn=newNode(T_MOD_ASSIGN,"%=",line);
	return MOD_ASSIGN;
	}
"&=" {printf("AND_ASSIGN\n");
	yylval.tn=newNode(T_AND_ASSIGN,"&=",line);
	return AND_ASSIGN;
	}
"|=" {printf("OR_ASSIGN\n");
	yylval.tn=newNode(T_OR_ASSIGN,"|=",line);
	return OR_ASSIGN;
	}
"^=" {printf("XOR_ASSIGN\n");
	yylval.tn=newNode(T_XOR_ASSIGN,"^=",line);
	return XOR_ASSIGN;
	}
"<<=" {printf("L_SHIFT_ASSIGN\n");
	yylval.tn=newNode(T_L_SHIFT_ASSIGN,"<<=",line);
	return L_SHIFT_ASSIGN;
	}
">>=" {printf("R_SHIFT_ASSIGN\n");
	yylval.tn=newNode(T_R_SHIFT_ASSIGN,">>=",line);
	return R_SHIFT_ASSIGN;
	}
"<<" {printf("L_SHIFT\n");
	yylval.tn=newNode(T_L_SHIFT,"<<",line);
	return L_SHIFT;
	}
">>" {printf("R_SHIFT\n");
	yylval.tn=newNode(T_R_SHIFT,">>",line);
	return R_SHIFT;
	}
"+" {printf("PLUS\n");
	yylval.tn=newNode(T_PLUS,"+",line);
	return PLUS;
	}
"-" {printf("MINUS\n");
	yylval.tn=newNode(T_MINUS,"-",line);
	return MINUS;
	}
"/" {printf("DIV\n");
	yylval.tn=newNode(T_DIV,"/",line);
	return DIV;
	}
"%" {printf("MOD\n");
	yylval.tn=newNode(T_MOD,"%",line);
	return MOD;
	}
">=" {printf("GE\n");
	yylval.tn=newNode(T_GE,">=",line);
	return GE;
	}
">" {printf("GT\n");
	yylval.tn=newNode(T_GT,">",line);
	return GT;
	}
"<=" {printf("LE\n");
	yylval.tn=newNode(T_LE,"<=",line);
	return LE;
	}
"<" {printf("LT\n");
	yylval.tn=newNode(T_LT,"<",line);
	return LT;
	}
"==" {printf("EQ\n");
	yylval.tn=newNode(T_EQ,"==",line);
	return EQ;
	}
"!=" {printf("NEQ\n");
	yylval.tn=newNode(T_NEQ,"!=",line);
	return NEQ;
	}
"=" {printf("ASSIGN\n");
	yylval.tn=newNode(T_ASSIGN,"=",line);
	return ASSIGN;
	}
"&" {printf("B_AND\n");
	yylval.tn=newNode(T_B_AND,"&",line);
	return B_AND;
	}
"&&" {printf("L_AND\n");
	yylval.tn=newNode(T_L_AND,"&&",line);
	return L_AND;
	}
"|" {printf("B_OR\n");
	yylval.tn=newNode(T_B_OR,"|",line);
	return B_OR;
	}
"||" {printf("L_OR\n");
	yylval.tn=newNode(T_L_OR,"||",line);
	return L_OR;
	}
"^" {printf("B_XOR\n");
	yylval.tn=newNode(T_B_XOR,"^",line);
	return B_XOR;
	}
";" {printf("SEMI\n");
	yylval.tn=newNode(T_SEMI,";",line);
	return SEMI;
	}
"->" {printf("ARROW\n");
	yylval.tn=newNode(T_ARROW,"->",line);
	return ARROW;
	}
"if" {printf("IF\n");
	yylval.tn=newNode(T_IF,"IF",line);
	return IF;
	}
"else" {printf("ELSE\n");
	yylval.tn=newNode(T_ELSE,"ELSE",line);
	return ELSE;
	}
"switch" {printf("SWITCH\n");
	yylval.tn=newNode(T_SWITCH,"SWITCH",line);
	return SWITCH;
	}
"case" {printf("CASE\n");
	yylval.tn=newNode(T_CASE,"CASE",line);
	return CASE;
	}
"default" {printf("DEFAULT\n");
	yylval.tn=newNode(T_DEFAULT,"DEFAULT",line);
	return DEFAULT;
	}
"do" {printf("DO\n");
	yylval.tn=newNode(T_DO,"DO",line);
	return DO;
	}
"while" {printf("WHILE\n");
	yylval.tn=newNode(T_WHILE,"WHILE",line);
	return WHILE;
	}
"for" {printf("FOR\n");
	yylval.tn=newNode(T_FOR,"FOR",line);
	return FOR;
	} 
"break" {printf("BREAK\n");
	yylval.tn=newNode(T_BREAK,"BREAK",line);
	return BREAK;
	}
"continue" {printf("CONTINUE\n");
	yylval.tn=newNode(T_CONTINUE,"CONTINUE",line);
	return CONTINUE;
	}
"const" {printf("CONST\n");
	yylval.tn=newNode(T_CONST,"CONST",line);
	return CONST;
	}
"int" {printf("INT\n");
	yylval.tn=newNode(T_INT,"INT",line);
	return INT;
	}
"flaot" {printf("FLOAT\n");
	yylval.tn=newNode(T_FLOAT,"FLOAT",line);
	return FLOAT;
	}
"double" {printf("DOUBLE\n");
	yylval.tn=newNode(T_DOUBLE,"DOUBLE",line);
	return DOUBLE;
	}
"char" {printf("CHAR\n");
	yylval.tn=newNode(T_CHAR,"CHAR",line);
	return CHAR;
	}
"long" {printf("LONG\n");
	yylval.tn=newNode(T_LONG,"LONG",line);
	return LONG;
	}
"void" {printf("VOID\n");
	yylval.tn=newNode(T_VOID,"VOID",line);
	return VOID;
	}
"goto" {printf("GOTO\n");
	yylval.tn=newNode(T_GOTO,"GOTO",line);
	return GOTO;
	}
"struct" {printf("STRUCT\n");
	yylval.tn=newNode(T_STRUCT,"STRUCT",line);
	return STRUCT;
	}
"static" {printf("STATIC\n");
	yylval.tn=newNode(T_STATIC,"STATIC",line);
	return STATIC;
	}
"sizeof" {printf("SIZEOF\n");
	yylval.tn=newNode(T_SIZEOF,"SIZEOF",line);
	return SIZEOF;
	}
"return" {printf("RETURN\n");
	yylval.tn=newNode(T_RETURN,"RETURN",line);
	return RETURN;
	}
{C_BLOCK_COMMENT} {;}
{C_LINE_COMMENT} {;}
{NAME} {printf("NAME: %s\n",yytext);
	yylval.tn=newNode(T_NAME,yytext,line);
	return NAME;
	}
{INTEGER} { printf("INTEGER\n");
	yylval.tn=newNode(T_INTEGER,yytext,line);
	 return INTEGER;
	 }
[\n] {line++;}
[ \t]+ {;}
. {printf("Error Unrecognized character, %d\n",line);}
 /*
"(" {printf("LSB ");}
")" {printf("RSB ");}
"[" {printf("LMB ");}
"]" {printf("RMB ");}
"{" {printf("LLB ");}
"}" {printf("RLB ");}
"." {printf("DOT ");}
"*" {printf("STAR ");}
"," {printf("COMMA ");}
":" {printf("COLON ");}
"!" {printf("NOT ");}
"+" {printf("PLUS ");}
"++" {printf("INC ");}
"-" {printf("MINUS ");}
"--" {printf("DEC ");}
"/" {printf("DIV ");}
"%" {printf("MOD ");}
">=" {printf("GE ");}
">" {printf("GT ");}
"<=" {printf("LE ");}
"<" {printf("LT ");}
"==" {printf("EQ ");}
"!=" {printf("NEQ ");}
"=" {printf("ASSIGN ");}
"&" {printf("B_AND ");}
"&&" {printf("L_AND ");}
"|" {printf("B_OR ");}
"||" {printf("L_OR ");}
"^" {printf("B_XOR ");}
";" {printf("; ");}
"->" {printf("-> ");}
"NULL"|"null" {yylval = 0;}

"if" {printf("IF ");}
"else" {printf("ELSE ");}
"switch" {printf("SWITCH ");}
"case" {printf("CASE ");}
"default" {printf("DEFAULT ");}

"do" {printf("DO ");}
"while" {printf("WHILE ");}
"for" {printf("FOR ");} 
"break" {printf("BREAK ");}
"continue" {printf("CONTINUE ");}


"const" {printf("CONST ");}
"int" {printf("INT ");}
"flaot" {printf("FLOAT ");}
"double" {printf("DOUBLE ");}
"char" {printf("CHAR ");}
"long" {printf("LONG ");}

"void" {printf("VOID ");}

"goto" {printf("GOTO ");}
"struct" {printf("STRUCT ");}
"static" {printf("STATIC ");}
"sizeof" {printf("SIZEOF ");}
"return" {printf("RETURN ");}


{C_BLOCK_COMMENT} {printf("COMMENT ");}
{C_LINE_COMMENT} {printf("COMMENT ");}
{NAME} {printf("NAME ");}
{INTEGER} {printf("integer ");}
{REAL} {printf("real number ");}
[\n] {line++;printf("\n");}
[ \t]+ {;}
. {printf("Error Unrecognized character, %d\n",line);}
 */

 
 /*
"(" {return LSB;}
")" {return RSB;}
"[" {return LMB;}
"]" {return RMB;}
"{" {return LLB;}
"}" {return RLB;}
"." {return DOT;}
"*" {return STAR;}
"," {return COMMA;}
":" {return COLON;}
"!" {return NOT;}
"+" {return PLUS;}
"-" {return MINUS;}
"/" {return DIV;}
"%" {return MOD;}
">=" {return GE;}
">" {return GT;}
"<=" {return LE;}
"<" {return LT;}
"==" {return EQ;}
"!=" {return NEQ;}
"=" {return ASSIGN;}
"&" {return B_AND;}
"&&" {return L_AND;}
"|" {return B_OR;}
"||" {return L_OR;}
"^" {return B_XOR;}
";" {return SEMI;}
"->" {return ARROW;}
"NULL"|"null" {yylval = 0;}

"if" {return IF;}
"else" {return ELSE;}
"switch" {return SWITCH;}
"case" {return CASE;}
"default" {return DEFAULT;}

"do" {return DO;}
"while" {return WHILE;}
"for" {return FOR;} 
"break" {return BREAK;}
"continue" {return CONTINUE;}


"const" {return CONST;}
"int" {return INT;}
"flaot" {return FLOAT;}
"double" {return DOUBLE;}
"char" {return CHAR;}
"long" {return LONG;}

"void" {return VOID;}

"goto" {return GOTO;}
"struct" {return STRUCT;}
"static" {return STATIC;}
"sizeof" {return SIZEOF;}
"return" {return RETURN;}

{C_BLOCK_COMMENT} {;}
{C_LINE_COMMENT} {;}
{NAME} {return NAME;}
{INTEGER} {return INTEGER;}
{REAL} {return REAL;}
[\n] {line++;}
[ \t]+ {;}
. {printf("Error Unrecognized character, %d\n",line);}
 */






 




%%


int yywrap()
{
	return 1;
}

