flex lex.l
bison -d -v yacc.y
clang -c myfunc/ASTtree.c myfunc/symbol.c myfunc/llvmgen.c
clang -o proj lex.yy.c yacc.tab.c ASTtree.o symbol.o llvmgen.o