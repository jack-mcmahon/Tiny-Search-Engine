#!/bin/bash
#
# Bash testing script for querier.c
#
# Created by Jack McMahon February 21th, 2022

myvalgrind='valgrind --leak-check=full --show-leak-kinds=all'

#********************Incorrect Arguments********************************

#1: Test with incorrectly no arguments

./querier

#2: Test with incorrectly one argument

./querier output/wikipedia-1

#3: Test with incorrectly three arguments

./querier output/wikipedia-1 output/wikipedia-1.index 5

#4:  Test with invalid pageDirectory(non-existant path)

./querier nonexistant-path/tse/output/wikipedia-1 output/wikipedia-1.index

#5: Test with invalid pageDirectory(not a crawler directory)

./querier output/indexer output/wikipedia-1.index

#6: Test with invalid indexFile(nonexistant path)

./querier output/wikipedia-1 non-existant-path/index-data/test6.index

#***************Testing Basic Query Inputs****************************

#1: Test with different invalid queries

./querier output/toscrape-1 output/toscrape-1.index < testInputs/input1

#2: Test with different spacing, capitalization, and basic or/and combinations.

./querier output/toscrape-2 output/toscrape-2.index < testInputs/input2

#***************Testing Valgrind****************************

#1: Test with valgrind 1

$myvalgrind echo superscript or hop or fame |  ./querier output/wikipedia-2 output/wikipedia-2.index

#1: Test with valgrind 2

$myvalgrind echo poetry or this and that translation  |   ./querier output/wikipedia-2 output/wikipedia-2.index

#*************** Regression Testing with myfuzzquery ****************************

#1: Test with myfuzzquery 1

./myfuzzquery output/toscrape-1.index 10 0 | ./querier output/toscrape-1 output/toscrape-1.index  | tee regressiontesting/first-trial-toscrape-1

#2: Test with myfuzzquery 2
./myfuzzquery output/toscrape-2.index 10 0 | ./querier output/toscrape-2 output/toscrape-2.index  | tee regressiontesting/first-trial-toscrape-2

#3: Test with myfuzzquery 3

./myfuzzquery output/toscrape-1.index 10 0 | ./querier output/toscrape-1 output/toscrape-1.index  | tee regressiontesting/second-trial-toscrape-1

#4: Test with myfuzzquery 4
./myfuzzquery output/toscrape-2.index 10 0 | ./querier output/toscrape-2 output/toscrape-2.index  | tee regressiontesting/second-trial-toscrape-2

#compare:

cmp -l regressiontesting/first-trial-toscrape-1 regressiontesting/second-trial-toscrape-1

cmp -l regressiontesting/first-trial-toscrape-2 regressiontesting/second-trial-toscrape-2
