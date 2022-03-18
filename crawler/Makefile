# Makefile for Crawler
#
# Jack McMahon, February 7th, 2022

OBJS = crawler.o
C = ../common
L = ../libcs50
LLIBS = $C/common.a $L/libcs50.a
PROG = crawler


# uncomment the following to turn on verbose output logging
# LOGGING=-DVERBOSE

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) $(LOGGING) -I$C -I$L
CC = gcc
MAKE = make

$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

crawler.o: crawler.c

.PHONY: test clean

test: $(PROG)
	bash -v testing.sh

clean:
	rm -f *~ *.o
	rm -f $(PROG)
	rm -f crawler.o
	rm -f core
	rm -f testing.out