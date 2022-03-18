#!/bin/bash
#
# Bash testing script for indexer.c
#
# Created by Jack McMahon February 14th, 2022

myvalgrind='valgrind --leak-check=full --show-leak-kinds=all'

#Incorrect Arguments********************************

#1: Test with incorrectly no arguments
./indexer

#2: Test with incorrectly one argument
./indexer ../tse/output/wikipedia-1

#3: Test with incorrectly three arguments
./indexer ../tse/output/wikipedia-1 ../index-data/test3.index 5

#4:  Test with invalid pageDirectory(non-existant path)
./indexer ../nonexistant-path/tse/output/wikipedia-1 ../index-data/test4.index

#5: Test with invalid pageDirectory(not a crawler directory)
./indexer ../tse/output/indexer ../index-data/test5.index

#6: Test with invalid indexFile(nonexistant path)
./indexer ../tse/output/wikipedia-1 ../non-existant-path/index-data/test6.index

#7: Test with invalid indexFile(read-only directory)
./indexer ../tse/output/wikipedia-1 ../index-data/read-only/test7.index

#8: Test with invalid indexFile(existing, read-only file)
./indexer ../tse/output/wikipedia-1 ../index-data/test8.index

#Test with indexTest validation****************************

#1: Test with letters-2, indextest, then compare
./indexer ../tse/output/letters-2 ../index-data/letters-2.index

./indextest ../index-data/letters-2.index ../index-data/letters-2-indextest.index

../tse/indexcmp ../index-data/letters-2.index ../index-data/letters-2-indextest.index

#2: Test with letters-3, indextest, then compare
./indexer ../tse/output/letters-3 ../index-data/letters-3.index

./indextest ../index-data/letters-3.index ../index-data/letters-3-indextest.index

../tse/indexcmp ../index-data/letters-3.index ../index-data/letters-3-indextest.index

#3: Test with toscrape-1, indextest, then compare
./indexer ../tse/output/toscrape-1 ../index-data/toscrape-1.index

./indextest ../index-data/toscrape-1.index ../index-data/toscrape-1-indextest.index

../tse/indexcmp ../index-data/toscrape-1.index ../index-data/toscrape-1-indextest.index

#4: Test with wikipedia-1, indextest, then compare
./indexer ../tse/output/wikipedia-1 ../index-data/wikipedia-1.index

./indextest ../index-data/wikipedia-1.index  ../index-data/wikipedia-1-indextest.index

../tse/indexcmp ../index-data/wikipedia-1.index  ../index-data/wikipedia-1-indextest.index

#Test with valgrind***********************

#1: Test with valgrind on indexer for letters10
$myvalgrind ./indexer ../tse/output/letters-10 ../index-data/letters-10.index

#2: Test with valgrind on indextest for toscrape-2.index
$myvalgrind ./indextest ../tse/output/toscrape-2.index ../index-data/toscrape-2-indextest.index

../tse/indexcmp ../tse/output/toscrape-2.index ../index-data/toscrape-2-indextest.index
