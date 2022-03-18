# CS50 TSE Indexer
## Implementation Spec

Written by Jack McMahon, February 12th 2022

This implementation spec for Indexer covers:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan


## Data structures 

We use one main data structure: It is an index data structure that is based on a hastable and builds the index of web pages. It maps froms a word to (docID, #occurences) pairs. The hastable the index is built on stores words as keys and counters as items. The size of the hashtable (slots) when building an inverted index is impossible to determine in advance, so we use 300. When loading an inverted index from file we will be able to know the number of words and can optimize the number of hashtable slots.

## Control flow

The Indexer is implemented in one file `indexer.c`, with four functions.

### main

The `main` function calls `parseArgs` and `indexBuild`, then exits zero.

### parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

* for `pageDirectory`, call `pagedir_validate()`
* for `indexFilename`, check that we can open the file for writing

### indexBuild

This function assembles the index by creating an `index` object, loading each webpage in the pageDictory and sending them to indexPage to be added to the index. It then writes the index to a file.
Pseudocode:

	Create a new 'index' object
	loop over document ID numbers, counting from 1
   		loads a webpage from the document file 'pageDirectory/id'
   		if successful, 
     		pass the webpage and docID to indexPage
	Writes the index to indexFilename

### indexPage

This function is given an index and a webpage to add to that index. It normalizes words from the webpage and add a count of each word over 3 letters to the index.
Pseudocode:

	while there is another word in the webpage
		if that word is at least 3 letters
			call word_normalize()
			look up the word in the index
				if the word is not in the index then add it
			increment the count of that word in the index
		free the word

## Other modules

### index

This index module holds functions to create and modify index data structures. It will store the index information for indexer and help in writing it to a file.

Pseudocode for `index_new`:

	create a new index
	create a new hashtable
	insert hashtable into index
	return index

Pseudocode for `index_insert`:

	Create a new counterset
	if successfully inserts counterset with word as key in hashtable
		return true
Pseudocode for `index_increment`:

	if counterset exists for word
		call counters_add with counterset and docID

Pseudocode for `index_find`:

	create a pointer to a counter set
	assign return value from hashtable_find to counter set
	return counter set

Pseudocode for `index_load`:

	check input isn't null
	create new index
	loop through each line
		insert the word into the index
		loop through each (docID, counter) pair
			while count is greater than zero
				increment the docID counter in index
		free the word

Pseudocode for `index_iterate`:

	check input isn't null
	call hashtable_iterate

Pseudocode for `index_write`:

	check input isn't null
	call hashtable_iterate with index_printLines

Pseudocode for `index_delete`:

	check if index is not null
	call hashtable_delete on the hashtable in index
	free the index


### word

we will write a short function to normalize a word by converting letters to lowercase

Pseudocode for `word_normalize`:

	while there is another letter in the word
		covert letter to lowercase and store in a temp char
		assign the letter as the temp


### pagedir

We will extend the re-usable module `pagedir.c` to add functions to validate crawler-produced directories and to load webpages from files.

Pseudocode for `pagedir_load`:

	if pageDirectory is not null and docID is greate than zero
		combine pageDirectory and docID into a file path
		open the file from that path
		If file is readable
			extract URL
			extract depth
			extract HTML
			initialize webpage with those qualifiers
			return true

Pseudocode for `pagedir_validate(dir)`:

	create a path name for .crawler file
	open .crawler fule
		if .crawler file is readable
			return true

### libcs50

We leverage the modules of libcs50, most notably `file`, `counters`, `hashtable`, and `webpage`.
See that directory for module interfaces.
The `webpage` module allows us to represent pages as `webpage_t` objects. We will use to load information from a pageDirectory and pass it as `webpage_t` objects to indexPage. `file` will also be very useful in loading information from files in pageDirectory and writing/reading index files.

## Function prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `indexer.c` and is not repeated here.

```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename);
static void indexBuild(char* pageDirectory, char* indexFilename);
static void indexPage(index_t* invertedIndex, webpage_t* page, const int docID);
```
### index

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `index.h` and is not repeated here.

```c
static void index_printCounters(void *arg, const int key, const int count);
static void index_printLines(void *arg, const char *key, void *item);
index_t* index_new(const int numslots);
bool index_insert(index_t* index, const char* word);
int index_increment(index_t* index, const char* word, int docID)l
void* index_find(index_t* index, const char* word);
void index_load(index_t** index, FILE* fp);
void index_iterate(index_t *index, void *arg, void (*itemfunc)(void *arg, const 							char *key, void *item));
void index_write(index_t* index, FILE* fp);
void index_delete(index_t* index, void(*itemdelete)(void *item));
```
### word

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `word.h` and is not repeated here.

```c
void word_normalize(char* word);

```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `pagedir.h` and is not repeated here.

```c
bool pagedir_validate(const char* pageDirectory);
bool pagedir_load(webpage_t** page, const char* pageDirectory, const int docID);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.


## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

For unit testing we will use the program, `indextest`. It reads an index file into an `index` data structure and then writes that `index` into a new index file. This allows us to unit test on the `index` module.

### Integration/system testing

We write a script `testing.sh` that invokes the indexer several times, with a variety of command-line arguments and on different pageDirectories.
First, we test the indexer with a series of invalid arguments, including invalid or nonwriteable directories.
Second, we run indexer on a variety of pageDirectories, validating the results with indextest
Third, we will check for any memory leaks or errors by running valgrind on both indexer and indextest.
