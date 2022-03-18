# CS50 TSE Querier
## Implementation Spec

Written by Jack McMahon, February 21th 2022

This implementation spec for querier covers:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan


## Data structures 

We use three main data structures: 

The first is an index data structure that is based on a hastable and builds the index of web pages. It maps froms a word to (docID, #occurences) pairs. The hastable the index is built on stores words as keys and counters as items. 

The second is a queryToken data structure. This data structure is in the form of a linked listm where each node stores a word and a pointer to the next node. The queryToken linked list is used for validating syntax and parsing a query to build a counterset of results

The third main data structure is the counterset, 

## Control flow

The Querier is implemented in one file `querier.c`, with eight main functions.

### main

The `main` function calls `parseArgs` and `takeQueries`, then exits zero.

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

* for `pageDirectory`, call `pagedir_validate()`
* for `indexFilename`, check that we can open the file for reading

### takeQueries

This functions takes queries line by line from stdin and passes them to parseQuery and the rankResults;
Pseudocode:

	prints prompt
	Reads a query per line from stdin
		creates a results counter 
        calls parseQuery
        if query was valid
            call rankResults

### parseQuery

This function parses a query for syntax, tokenizes it, and assembles a results counter
Pseudocode:

	while i = 0 < querylength
        if any character is invalid
            return false
    call tokenizeQuery
    check syntax by calling method
    create result counterset from first node
    while there are more query tokens
        while the word is not "or"
            call intersectSets
        while there are more words
            set aside the first counterset
            while the word is not "or" or NULL
                intersectSets each word on a new counterset
            call unionSets on both countersets
    if the end is not or
        call unionsets on both countersets
    return resultsCounter  

### tokenizeQuery

This function creates a linked list of query Tokens from a query
Pseudocode:

	for each character in the query
		splice out white spice and isolate each word
		create a new query token for each word
		link to the next query token
	return a pointer to the first token

### intersectSets

This function is called when we see an and operatior or no operator, find the min count for two words.
Pseudocode:

	create array of both counters
	pass to counters_iterate
		for each counter in result
		find the min of both counters for that key
		set the min to be that sum

### unionSets

This function is called when we see an or operatior, find the sum count for two words, combining two countersets.
Pseudocode:

	if added counter is not null
		create array of both counters
		pass to counters_iterate
			for each counter in result
			find the sum of both counters for that key
			set the result to be that sum

### rankResults

Iterates through result counterset and prints the highest, setting it then to zero to get a list indescedning order.
Pseudocode:

	iterate to count number of documents
	print header with count
	while there are more documents
		call counters_iterate to find the highest ranked
		find URL for that docID
		print 
		set docID to zero count
	if no best
		print "no matching docuements"

## Other modules

### queryToken

This queryToken module holds functions to create and modify queryToken data structures.

Pseudocode for `queryToken_new`:

	create a local copy of the word
	create a new querytoken
	insert word into querytoken
	return querytoken

Pseudocode for `queryToken_addNext`:

	add a point from token to next token

Pseudocode for `queryToken_getWord`:

	return token->word

Pseudocode for `queryToken_getNext`:

	return pointer from token to next

Pseudocode for `queryToken_print`:

	while the next word is not null
		current token gets next
		print formatted word from token

Pseudocode for `queryToken_checkSyntax`:

	if the first word is 'or' or 'and'
		return false
	while there are more words
		if word is 'and' or 'or'
			if last was 'and' or 'or'
				return false
			set last to true
		else
			set last to false
	if the last word is 'or' or 'and'
		return false
	return true

Pseudocode for `queryToken_delete`:

	while the next word is not null
		current token gets next
		delete stored word
		save pointer to next
		delete current
## Function prototypes

### querier

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `querier.c` and is not repeated here.

```c
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename);
static void takeQueries(char* pageDirectory, index_t* index);
static bool parseQuery(counters_t** resultCounter, char* query, index_t* index);
static queryToken_t* tokenizeQuery(char* query);
static void intersectSets(counters_t* resultCounter, counters_t* addedCounter);
static void intersectHelper(void *arg, const int key, const int count);
static void unionSets(counters_t** resultCounter, counters_t* addedCounter);
static void unionHelper(void *arg, const int key, const int count);
static void rankResults(counters_t* resultCounter, char* pageDirectory);
static void countResults(void *arg, const int key, const int count);
static void printResults(void *arg, const int key, const int count);
static char* findURL(char* pageDirectory, int docID);
int fileno(FILE *stream);
static void prompt(void);
```
### queryToken

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `queryToken.h` and is not repeated here.

```c
typedef struct queryToken queryToken_t
queryToken_t* queryToken_new(char* word);
void queryToken_addNext(queryToken_t* token, queryToken_t* nextToken);
char* queryToken_getWord(queryToken_t* token);
queryToken_t* queryToken_getNext(queryToken_t* token);
void queryToken_print(queryToken_t* token);
bool queryToken_checkSyntax(queryToken_t* token);
void queryToken_delete(queryToken_t* token);
```


## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.

## Testing plan

Here is an implementation-specific testing plan.

### Integration/system testing

We write a script `testing.sh` that invokes the querier several times, with a variety of command-line arguments and on different pageDirectories and indexes.

First, we test the querier with a series of invalid arguments, including invalid directories.

Second, we run querier with valid arguments but invalid queries, testing character parsing and syntax

Third, we will run some basic queries, checking to see if words are normalized and properly spaced when printed back. We will try different orderings and combinagtions of 'or' and 'and.'

Fourth, we will check valgrind on two query searches.

Lastly, we will do rigorous regression testing with fuzzquery, getting several random queries we can compare against different runs of querier.
