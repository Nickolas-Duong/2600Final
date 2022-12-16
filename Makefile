CC=gcc

ifeq ($(OS), Windows_NT)
	ARFLAGS=-rc
	DELETE=del
	EXT=.exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($UNAME_S, Darwin)
		ARFLAGS=rcs
		DELETE = rm -f
		EXT = .out
	endif
endif

all: src/main.c
	$(CC) -o bin/main$(EXT) src/main.c -lpaho-mqtt3c

compile: main

main: src/main.c 
	$(CC) -c src/main.c -o obj/main.o

delete: main$(EXT)
	$(DELETE) bin/main$(EXT)