#!/bin/bash
#
# Bash testing script for crawler.c
#
# For verbose output logging uncomment #Logging in crawler Makefile
#
# Created by Jack McMahon February 5th, 2022

myvalgrind='valgrind --leak-check=full --show-leak-kinds=all'

# Test with incorrect number of arguments
./crawler

# Test with bad URL
mkdir ../data/test2
./crawler https://www.nytimes.com ../data/test2 2

# Test with bad directory
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/test3 2

# Test with out of bounds depth
mkdir ../data/letters3
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters3 15

# Test with toscrape on depth 0
mkdir ../data/toscrape0
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape0 0

# Test with toscrape on depth 1
mkdir ../data/toscrape1
$myvalgrind ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/index.html ../data/toscrape1 1

# Test with letters on depth 1
mkdir ../data/letters1
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters1 1

# Test with letters on depth 10
mkdir ../data/letters10
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters10 10

# Test with wikipedia on depth 0
mkdir ../data/wikipedia0
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia0 0

# Test with wikipedia on depth 1
mkdir ../data/wikipedia1
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/index.html ../data/wikipedia1 1
