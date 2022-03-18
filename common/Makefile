# Makefile for Common
#
# Jack McMahon, February 14th, 2022

OBJS = pagedir.o index.o word.o
LIB = common.a
L = ../libCS50

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS) -I$L
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

pagedir.o: pagedir.h
index.o: index.h
word.o: word.h

.PHONY: clean

clean:
	rm -f $(LIB) *~ *.o
	rm -f core
