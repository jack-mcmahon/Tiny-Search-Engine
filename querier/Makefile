# Makefile for Indexer
#
# Jack McMahon, February 14th, 2022

OBJS = queryToken.o querier.o
C = ../common
L = ../libcs50
LLIBS = $C/common.a $L/libcs50.a
PROG = querier


CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -I$C -I$L
CC = gcc
MAKE = make

.PHONY: test clean all

all: $(PROG) myfuzzquery

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

myfuzzquery: myfuzzquery.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

myfuzzquery.o: myfuzzquery.c
queryToken.o: queryToken.h
querier.o: querier.c

test: $(PROG)
	bash -v testing.sh

clean:
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f querier.o
	rm -f queryToken.o
	rm -f fuzzquery.o
	rm -f core
	rm -f testing.out