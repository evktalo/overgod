CC=gcc
CFLAGS=-O2 -march=native -w
LDFLAGS=-lm -lalleg

SOURCE=$(wildcard *.c)
DEPENDENCY=$(wildcard *.h) Makefile

all: overgod

debug: CFLAGS += -DDEBUG -g
debug: overgod

overgod: $(SOURCE) $(DEPENDENCY)
	$(CC) $(SOURCE) $(CFLAGS) $(LDFLAGS) -o overgod
