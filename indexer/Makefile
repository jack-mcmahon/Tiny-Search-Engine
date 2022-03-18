# Makefile for Indexer
#
# Jack McMahon, February 14th, 2022

OBJS = indexer.o
C = ../common
L = ../libcs50
LLIBS = $C/common.a $L/libcs50.a
PROG = indexer
TESTER = indextest


CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -I$C -I$L
CC = gcc
MAKE = make

.PHONY: test clean all

all: $(PROG) $(TESTER)

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

$(TESTER): indextest.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

indexer.o: indexer.c

test: $(PROG) $(TESTER)
	bash -v testing.sh

clean:
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f $(TESTER)
	rm -f indexer.o
	rm -f indextest.o
	rm -f core
	rm -f testing.out