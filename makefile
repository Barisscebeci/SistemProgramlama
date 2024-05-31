CC = gcc
CFLAGS = -I "./include"

all: program lib/libfdr.a

program: main.o fields.o dllist.o jval.o jrb.o
	$(CC) $(CFLAGS) ./obj/fields.o ./obj/main.o ./obj/dllist.o ./obj/jval.o ./obj/jrb.o -o ./bin/main

main.o:
	$(CC) $(CFLAGS) -c ./src/main.c -o ./obj/main.o

fields.o:
	$(CC) $(CFLAGS) -c ./src/fields.c -o ./obj/fields.o

jval.o:
	$(CC) $(CFLAGS) -c ./src/jval.c -o ./obj/jval.o

jrb.o:
	$(CC) $(CFLAGS) -c ./src/jrb.c -o ./obj/jrb.o

dllist.o: jval.o
	$(CC) $(CFLAGS) -c ./src/dllist.c -o ./obj/dllist.o

lib/libfdr.a: obj/jval.o obj/dllist.o obj/fields.o obj/jrb.o
	ar ru lib/libfdr.a obj/jval.o obj/jrb.o obj/dllist.o obj/fields.o
	ranlib lib/libfdr.a

clean:
	rm -rf o ./obj/ lib/* ./bin/*

run:
	.\bin\main

.PHONY: all clean run