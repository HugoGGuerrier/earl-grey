CC=gcc
YACC_C=bison
FLEX_C=lex

CFLAGS=-W -Wall -O3
LDFLAGS=-lm -ll
EXEC=out/egcc

SRC=src/lex.yy.c src/parser.tab.c src/main.c src/ast.c src/ast_printer.c src/compiler.c src/utils.c
LIB=${SRC:src%.c=include%.h}
OBJ=${SRC:src%.c=obj%.o}

all: obj out $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

obj/%.o: src/%.c include/%.h
	$(CC) -o $@ -c $< -I include $(CFLAGS)

obj/lex.yy.o: src/lex.yy.c include/parser.tab.h
	$(CC) -o $@ -c $< -I include $(CFLAGS)

src/lex.yy.c: src/parser/lexer.lex
	${FLEX_C} -o $@ $<

src/parser.tab.c include/parser.tab.h: src/parser/parser.y
	${YACC_C} -d $<
	mv parser.tab.c src/parser.tab.c
	mv parser.tab.h include/parser.tab.h

obj:
	mkdir obj/

out:
	mkdir out/

clean:
	rm -rf obj/*

purge: clean
	rm -f $(EXEC)
	rm -f include/parser.tab.h
	rm -f src/parser.tab.c
	rm -f src/lex.yy.c