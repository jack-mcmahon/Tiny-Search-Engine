/* 
 * querier.c
 *
 * This is the third module of the TSE for CS50
 * 
 * Querier takes two arguements, a pageDirectory, and an indexFilename
 * 
 * Input Specificiations:
 * The pageDirectory must be readable and must have been created by crawler
 * indexFilename must be readbale and must have been created by Indexer for the same pageDirectory
 * 
 * The querier then reads user input line by line from stdin, for a valid
 * query it will print a list in descending order of webpages from the given directory
 * that match the query words.
 * 
 * Input any valid words of at least three characters with 'or' and 'and'
 * as modifiers.
 * 
 * Written with consultation of CS50 knowledge units and functions provided by libcs50.
 * 
 * Jack McMahon, February 21th 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "../libcs50/mem.h"
#include "../libcs50/webpage.h"
#include "../libcs50/counters.h"
#include "../libcs50/hashtable.h"
#include "queryToken.h"
#include "../common/pagedir.h"
#include "../common/index.h"
#include "../common/word.h"

// function prototypes
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
static void countersZero(void *arg, const int key, const int count);
static char* findURL(char* pageDirectory, int docID);
int fileno(FILE *stream);
static void prompt(void);

/* ********************* main ************************ */
int main(const int argc, char* argv[]){

    char* pageDirectory; //Directory that we will writefiles to for each crawled webpage
    char* indexFilename; //File name for the index file we will write
    index_t* index;      //Index to be loaded from indexFilename
    FILE* fp;            //File pointer to read indexFilename

    //parse arguments to check if they match correct input type, if so assign to variables
    parseArgs(argc, argv, &pageDirectory, &indexFilename);

    //load index from indexFilename
    fp = fopen(indexFilename, "r");
    index_load(&index, fp);
    fclose(fp);

    //take queries from stdin
    takeQueries(pageDirectory, index);
    
    //free memory at the end
    mem_free(pageDirectory);
    mem_free(indexFilename);
    index_delete(index, (void(*)(void*))counters_delete);

    exit(0);
}

/**************** parseArgs ****************/
/* 
 * Input: char** for page directory, char** for indexFilename
 * 
 * Makes sure arguments match the given specifications, if so then assigns them to variables
*/
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename){
    if(argc != 3){
        //throw error if incorrect number of arguments
        fprintf(stderr, "Incorrect number of arguments.\n");
        exit(1);
    }

    *pageDirectory = mem_malloc_assert((strlen(argv[1]) + 1), "pageDirectory");
    strcpy(*pageDirectory, argv[1]);

    if ( ! pagedir_validate(*pageDirectory)) {
        //throw error if can't write in  given page directory
        fprintf(stderr, "pageDirectory could not be validated\n");
        exit(2);
    }

    FILE* fp;
    *indexFilename = mem_malloc_assert((strlen(argv[2]) + 1), "indexFilename");
    strcpy(*indexFilename, argv[2]);

    //check if the index file specified by indexFilename is writeable
    if((fp = fopen(*indexFilename, "r")) != NULL) {
        fclose(fp);
    } else { 
        fprintf(stderr, "Cannot open %s for reading\n", *indexFilename);
        exit(22);
    }

    // return if tests passed
    return;
}

/**************** takeQueries ****************/
/* 
* Input: char* pageDirectory and index_t* index
* 
* Prompts the user and reads query input line by line from stdin. 
* 
* Calls parseQuery to verify the query and handle internal logic.
* Then calls rankResults to print a formatted list in descending order of
* documents in pageDirectory that match the query.
* 
* Returns: nothing
*/
static void takeQueries(char* pageDirectory, index_t* index){

    char* query;        // holds user query

    prompt();
    //read queries line by line until EOF
    while((query = file_readLine(stdin)) != NULL){
        counters_t* resultCounter = counters_new();
        //display prompt if interactive user
        if(parseQuery(&resultCounter, query, index)){
            //if query is valid then return a list of ranked results
            rankResults(resultCounter, pageDirectory);
        }
        //free the query and prompt user for more input
        counters_delete(resultCounter);
        if (query != NULL){
            free(query);
        }
        prompt();
    }
}

/**************** parseQuery ****************/
/* 
* Input: char** resultCounter, char* query, and index_t* index
* 
* First loops over the characters in a query to filter out bad queries that are blank
* or contain invalid characters that are not in the alphabet.
*
* Then calls tokenizeQuery to receive a linked list of normalized and formatted 
* queryTokens which it prints back to stdout and checks for valid syntax.
* 
* Then loops over every word in the query and combines the query results for documents in the index
* into a single counterset based on the 'or' and 'and' syntax. Helper methods for union for 'or' and intersect for 'and 
* iterate of the result and added counterset to combine.
*
* Returns: true if query is valid and result finding is successful, false otherwise
*/
static bool parseQuery(counters_t** resultCounter, char* query, index_t* index){
    char c;                                         //char used to iterate through query
    bool containsLetters = false;                   //bool tracks if query contains letters 
    queryToken_t* firstToken = NULL;                //first word pointer in a linked list of query token structs
    queryToken_t* currentToken = NULL;              //first word pointer in a linked list of query token structs
    queryToken_t* nextToken = NULL;                 //next word pointer in a linked list of query token structs
    char* word;                                     //temp variable loads word from a given queryToken
    int length = strlen(query);                     //length of the query
    counters_t* rightResult = counters_new();       //holds the rightmost running tally of combined counters
    counters_t* tempCounter;                        //temp variable to load counters for a given word
    bool orExists = false;                          //bool keeps track of if we've seen an 'or' yet

    //verify the query contains only letters and spaces
    for(int i =0; i < length; i++){
        c = query[i];
        if(isalpha(c)){
            //if the character is a letter query isn't blank
            containsLetters = true;
        }
        else if(isspace(c)){
            //if the character is a letter then its all good
            continue;
        }
        else{
            //print offending character and return false to takeQueries
            printf("Error: bad character '%c' in query\n", c);
            return false;
        }
    }

    //Check if query is blank
    if(!containsLetters){
        return false;
    }

    //create a linked list of queryTokens for easy parsing
    firstToken = tokenizeQuery(query);
    currentToken = firstToken;
    //print the query back to stdout
    queryToken_print(currentToken);

    //return false and end search if query has invalid syntax
    if (!queryToken_checkSyntax(currentToken)){
        queryToken_delete(currentToken);
        return false;
    }

    //Create the counterset for the first word
    word = queryToken_getWord(currentToken);
    tempCounter = index_find(index, word);
    //add it to the result counterset
    unionSets(resultCounter, tempCounter);

    //loop over each word and either intersect or union countersects 
    while((nextToken = queryToken_getNext(currentToken)) != NULL ){
        //first extract the word from the current queryToken
        currentToken = nextToken;
        word = queryToken_getWord(currentToken);
        if(orExists){
            //if 'or' already exists then we are working with two result countersets and will
            //be intersecting new words with the rightmost result
            if((strcmp(word, "or")) == 0 ){
                //if we reach another 'or', we collapse both result countersets into the left one and start
                //a new one from the right of the 'or'
                unionSets(resultCounter, rightResult);
                nextToken = queryToken_getNext(currentToken);
                currentToken = nextToken;
                word = queryToken_getWord(currentToken);
                tempCounter = index_find(index, word);
                counters_iterate(rightResult, rightResult, &countersZero);
                unionSets(&rightResult, tempCounter);
            }
            else if((strcmp(word, "and")) == 0 ){
                //if the word is 'and' then we skip it and intersect the next word into the result counterset
                nextToken = queryToken_getNext(currentToken);
                currentToken = nextToken;
                word = queryToken_getWord(currentToken);
                tempCounter = index_find(index, word);
                intersectSets(rightResult, tempCounter);
            }
            else{
                //otherwise intersect the word into the result counterset
                tempCounter = index_find(index, word);
                intersectSets(rightResult, tempCounter);
            }
        }
        else{
            //if 'or' does not yet exist then we are working with one result counterset that has been intersected
            //with each addition so far
            if((strcmp(word, "or")) == 0 ){
                //if we reach 'or' we skip it, set aside the result counterset for later, and start a new result 
                //to the right of the 'or'
                orExists = true;
                nextToken = queryToken_getNext(currentToken);
                currentToken = nextToken;
                word = queryToken_getWord(currentToken);
                tempCounter = index_find(index, word);
                unionSets(&rightResult, tempCounter);
            }
            else if((strcmp(word, "and")) == 0 ){
                //if the word is 'and' then we skip it and intersect the next word into the result counterset
                nextToken = queryToken_getNext(currentToken);
                currentToken = nextToken;
                word = queryToken_getWord(currentToken);
                tempCounter = index_find(index, word);
                intersectSets(*resultCounter, tempCounter);
            }
            else{
                //otherwise intersect the word into the result counterset
                tempCounter = index_find(index, word);
                intersectSets(*resultCounter, tempCounter);
            }
        }
    }
    if(orExists){
        //if we've seen an 'or' before then we have two running tallies and need to combine them at the end
        unionSets(resultCounter, rightResult);
    }

    //free queryTokens and the unneeded result counterset
    counters_delete(rightResult);
    queryToken_delete(firstToken);
    
    return true;
}

static void countersZero(void *arg, const int key, const int count){
    if (arg != NULL){
        counters_t* rightResult = arg;
        counters_set(rightResult, key, 0);
    }
}

/**************** tokenizeQuery ****************/
/* 
* Input: char* query
* 
* Loops over the characters in a query, picks out words and adds them as new queryTokens in a linked list.
*
* Returns: pointer to the first queryToken in the linked list.
*/
static queryToken_t* tokenizeQuery(char* query){
    queryToken_t* firstWord;                                                     //holds a pointer to the first queryToken we can return
    queryToken_t* currentWord;                                                      //holds a pointer to the current
    queryToken_t* nextWord;                                                          //holds a pointer to the next queryToken
    char length = strlen(query);                                                  //length of query
    char* word = mem_malloc_assert(((length + 1)*sizeof(char)), "tokenWord");     //stores the word to add to make a new queryToken
    char charString[2];                                                           //allows us to strcat and strcpy with a single character
    charString[1] = '\0';                                                        //allows us to strcat and strcpy with a single character
    char c;                                                                      //temp for current character
    bool isInWord = false;                                                       //whether we're reading characters in a word
    bool isFirstWord = true;                                                     //whether it is the first word we're adding

    //iterate through the query and create a char* for each continous
    //string of letters to then create a queryToken_t for each
    for(int i =0; i < length; i++){
        c = query[i];
        charString[0] = c;

        if(isalpha(c) && !isInWord){
            //if this is the first letter after white space, start a new word
            strcpy(word, charString);
            isInWord = true;
        }
        else if(isalpha(c)){
            //if this letter comes after other letters, add to existing word
            strcat(word, charString);
        }
        else if (isInWord){
            word_normalize(word);
            //if this is the first space after letters, end word and tokenize
            isInWord = false;
            if(isFirstWord){
                //special condition so we can keep a pointer to the first word
                isFirstWord = false;
                firstWord = queryToken_new(word);
                currentWord = firstWord;
            }
            else{
                //create a new queryToken and add a link from the last one
                nextWord = queryToken_new(word);
                queryToken_addNext(currentWord, nextWord);
                currentWord = nextWord;
            }
            strcpy(word, "");
        }
        else{
            //continue to pass through white space
            continue;
        }
    }
    if(strlen(word) != 0){
        word_normalize(word);
        if(isFirstWord){
            //special condition so we can keep a pointer to the first word
            firstWord = queryToken_new(word);
        }
        else{
            //create a new queryToken and add a link from the last one
            nextWord = queryToken_new(word);
            queryToken_addNext(currentWord, nextWord);
            currentWord = nextWord;
        }
    }
    mem_free(word);
    return firstWord;
}

/**************** intersectSets ****************/
/* 
* Input: two countersets
* 
* iterates over the result counterset and sets the count for each key as the min of the two countersets for that key.
*
* Returns: nothing
*/
static void intersectSets(counters_t* resultCounter, counters_t* addedCounter){

    counters_t** twoCounters = mem_malloc_assert((sizeof(counters_t*)*2), "twocounters");
    
    twoCounters[0] = resultCounter;
    twoCounters[1] = addedCounter;
    counters_iterate(twoCounters[0], twoCounters, intersectHelper);

    mem_free(twoCounters);


}

/**************** intersectHelper ****************/
/* 
* called by counters_iterate to set each key in result as the min of both counters for that key
*/
static void intersectHelper(void *arg, const int key, const int count){

    int minCount;
    counters_t** twoCounters = arg;

    if(count < counters_get(twoCounters[1], key)){
        minCount = count;
    }
    else{
        minCount = counters_get(twoCounters[1], key);
    }

    counters_set(twoCounters[0], key, minCount);

}

/**************** unionSets ****************/
/* 
* Input: two countersets
* 
* iterates over the added counterset and sets the count in the result 
* for each key as the sum of the two countersets for that key.
*
* Returns: nothing
*/
static void unionSets(counters_t** resultCounter, counters_t* addedCounter){

    if(addedCounter != NULL) {
        counters_t** twoCounters = mem_malloc_assert((sizeof(counters_t*)*2), "twocounters");
        twoCounters[0] = *resultCounter;
        twoCounters[1] = addedCounter;
        counters_iterate(twoCounters[1], twoCounters, unionHelper);
        mem_free(twoCounters);
    }

}

/**************** unionHelper ****************/
/* 
* called by counters_iterate to set each key in result as the sum of both counters for that key
*/
static void unionHelper(void *arg, const int key, const int count){
    
    counters_t** twoCounters = arg;
    int sumCount = count + counters_get(twoCounters[0], key);
    counters_set(twoCounters[0], key, sumCount);

}

/**************** rankResults ****************/
/* 
* Input: result counterset and char* for pageDirectory path
* 
* Prints the count, docID, and url from pageDirectory for each document match in the result counterset
*
* Returns: nothing
*/
static void rankResults(counters_t* resultCounter, char* pageDirectory){
    int numDocuments = 0;                       //counter for number of documents matching query
    int best[] = {-1, 0};                       //int array to track the current highest ranking docID 
    char* url;                                  //stores url

    //iterate to tally number of documents
    counters_iterate(resultCounter, &numDocuments, countResults);

    //header with number of documents before our output
    printf("Matches (%d) documents (ranked):\n", numDocuments);

    //continiously find the best docID and then set it to zero to get the full ranked list in descedning order
    for(int i = 0; i < numDocuments; i++){
        best[0] = -1;
        best[1] = 0;
        counters_iterate(resultCounter, best, printResults);
        url = findURL(pageDirectory, best[0]);

        printf("score %5d doc %5d: %s\n", best[1], best[0], url);
        counters_set(resultCounter, best[0], 0);
        mem_free(url);
    }
    if(best[0] < 0){
        printf("No documents match\n");
    }
    printf("-----------------------------------------------\n");
}

/**************** countResults ****************/
/* 
* called by counters_iterate to count total result documents
*/
static void countResults(void *arg, const int key, const int count){
    int* sum = arg;

    if (sum != NULL && count > 0)
    (*sum)++;
}

/**************** printResults ****************/
/* 
* called by counters_iterate to find the highest scoring docID to print
*/
static void printResults(void *arg, const int key, const int count){
    int* best = arg;

    if(best[1] < count){
        best[0] = key;
        best[1] = count;
    }
}

/**************** findURL ****************/
/* 
* Input: pageDirectory and docID
*
* Finds and returns the url in pagedirector for the given document path
*/
static char* findURL(char* pageDirectory, int docID){
    FILE* fp;           //file to open webpage file
    char* url;          //holds url from line 1 of webpage file

    //first make docID into a char*
    int idLength = 10; 
    char* pageID = mem_malloc(idLength*sizeof(char)); 
    sprintf(pageID,"%d", docID);

    //now create and malloc a char* for the pathname to the file
    int length = strlen(pageDirectory) + idLength;
    char* pathName = mem_malloc(length*sizeof(char)+1);

    //copy the directory and id into the pathname
    strcpy(pathName, pageDirectory);
    strcat(pathName, "/");
    strcat(pathName, pageID);

    //check if file is readable
    if((fp = fopen(pathName, "r")) == NULL) {
        mem_free(pageID);
        mem_free(pathName);
        return NULL;
    }
    else{
        //scan the webpage file and extract url
        url = file_readLine(fp);
        mem_free(pathName);
        mem_free(pageID);
        fclose(fp);
        return url;
    }
}

/**************** prompt ****************/
static void prompt(void)
{
  // print a prompt iff stdin is a tty (terminal)
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}