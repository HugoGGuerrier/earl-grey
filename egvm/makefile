CC=gcc
CFLAGS=-W -Wall -O3
LDFLAGS=
EXEC=out/egvm

SRC=src/main.c src/machine.c src/utils.c src/executer.c src/debug_executer.c
LIB=${SRC:src%.c=include%.h}
OBJ=${SRC:src%.c=obj%.o}

all: obj out $(EXEC)

$(EXEC):$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

obj/%.o:src/%.c include/%.h
	$(CC) -o $@ -c $< -I include $(CFLAGS)

obj:
	mkdir obj/

out:
	mkdir out/

clean:
	rm -rf obj/*

purge: clean
	rm -f $(EXEC)