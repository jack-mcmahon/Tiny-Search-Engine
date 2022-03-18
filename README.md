## Tiny Search Engine

Written by Jack McMahon
February 21th, 2022

## Tiny Search Engine

The Tiny Search Engine is made up of three main modules in addition to the library and helper modules: Crawler, Indexer, and Querier.

Crawler crawls the web and retrieves web pages from a single seed URL. It parses the HTML on each webpage and recursively and extracts and explores each embedded URL to a specified depth. The program was written to only accept and explore URLs corresponding to an internal collection hosted by Dartmouth's servers. Each webpage is downloaded and then written as a file to a directory.

The Indexer is provided with the webpage directory file created by Crawler. It then parses the words in each webpage to build a reverse index linking every word to its frequency in each page. The finished index is then written to an index file.

The Querier accepts user searches from stdin. Each search is formatted and parsed for meaning according to "and" or "or" terms. So for instance someone could search "puppies and kittens or birds," and would receive a list of webpage URLS that either mention puppies and kittens or mention birds. Results are returned to stdout in a formatted list of webpage URLS ranked by best match according word frequency.

## Project Overview

I worked on this project over the month of February when I was enrolled in CS 50: Software Design and Implementation. I wrote each module as well as the underlying index, hashttable, counters, and set data structures.
