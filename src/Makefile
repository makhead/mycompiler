action:proj

.PHONY:proj
proj:
	flex lex.l
	bison -d -v yacc.y
	clang -g -c myfunc/ASTtree.c myfunc/symbol.c myfunc/llvmgen.c
	clang -g -o $@ lex.yy.c yacc.tab.c ASTtree.o symbol.o llvmgen.o

.PHONY:clean
clean:
	rm -f *.o
	rm -f lex.yy.c yacc.tab.c yacc.tab.h
	@echo cleaned