# CS50 TSE Querier
## Design Spec

Written by Jack McMahon, February 21th, 2022.

According to the querier requirements  spec, the TSE Querier is a standalone program that reads the index file produced by the TSE Indexer, and page files produced by the TSE Crawler, and answers search queries submitted via stdin

### User interface

The querier's command-line interface with the user must always have two arguments:

```
querier pageDirectory indexFilename
```

Where pageDictory is the pathname of a directory produced by the Crawler and indexFilename is the pathname of an index file produced by the Indexer.

After validating the command-line arguments, the user interface is through stdin, where Querier takes one query per line until EOF. 

### Inputs and outputs

**Input**: We take input initially specifying the pageDirectory and indexFilename through the command-line. Afterwards, each query is taken through stdin. A query  consists of words to search for and the qualifiers, "or" and "and".

**Output**: We print to stdout the formatted query and a list in descending order of the documents that satisfy the query along with their score and URL.

### Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules;
 2. *takeQueries*, which accepts queries from stdin
 3. *parseQuery*, which parses a query word by word and calls methods to build a counterset for matches
 4. *tokenizeQuery*, which standardizes a query
 5. *intersectSet*, which finds the intersect of two countersets prompted by "and"
 6. *unionSets*, which finds the union of two countersets prompted by "or"
 7. *rankResults*, which prints a set of documents in descending order by score
 8. *findURL*, which finds the URL for a given docID


And some helper modules that provide data structures:

 1. *index*, a module providing the data structure to represent the in-memory index, and functions to read and write index files;
 2. *counters*, a module providing the data structure to represent webpages, and to scan a webpage for words;
 3. *word*, a module providing a function to normalize a word.
 4. *queryToken* a module to represent queries as a linked list

### Pseudo code for logic/algorithmic flow

The querier will run as follows:

    parse the command line, validate parameters, initialize other modules, and load the index.
    call takeQueries

where *takeQueries:*

    Reads a query per line from stdin
        calls parseQuery
        if query was valid
            call rankResults
     

where *parseQuery:*

    verifies query contains only letters and spaces
    if so
        calls tokenizeQuery
        if query structure is valid
            Initialize resultsCounter
            call index_find on first word
            while the word is not "or"
                call intersectSets
            while there are more words
                set aside the first counterset
                while the word is not "or" or NULL
                    intersectSets each word on a new counterset
                call unionSets on both countersets
                return resultsCounter          
     

where *rankResults:*

    while resultsCounter has non zero counters
        find the max counter
        print entry
        call findURL on docID
        print URL
        set max counter to zero

### Major data structures

The key data structure is the *counters*, mapping from *docID* to *score* for a given search query.

Another important data structure is the *queryToken*, a linked list holding each discrete word in a query. A collection of methods for queryToken allow us to easily parse through the query, printing or verifying syntax.

We will also build an *index* from the document specified by indexFilename. An *index* is a *hashtable* keyed by *word* and storing *counters* as items. The *counters* in an index is keyed by *docID* and stores a count of the number of occurrences of that word in the document with that ID. 

### Testing plan

Testing will occur through checking edge cases, checking bad inputs, different permuations of word and formatting.

We will also use myfuzzquery to randomize tests. Then for regrerssion testing we will compare the output of two query results that should be identical.
