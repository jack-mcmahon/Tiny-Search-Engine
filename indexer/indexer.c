/* 
 * indexer.c
 *
 * This is the second module of the TSE for CS50
 * Written with consultation of CS50 knowledge pages and using methods from libcs50
 * 
 * Indexer takes two arguements, a pageDirectory, and an indexFilename
 * 
 * It will read webpages from the pagedirecting, counting the occurences of words or three
 * or more letters in each, and creating an index data structure to store that information.
 * It then writes the index to the file specified in the arguments.
 * 
 * Input Specificiations:
 * The pageDirectory must be readable and must have been created by crawler
 * indexFilename must point to a writeable file
 * 
 * Written with consultation of CS50 knowledge units and functions provided by libcs50.
 * 
 * Jack McMahon, February 14th 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/mem.h"
#include "../libcs50/webpage.h"
#include "../libcs50/counters.h"
#include "../libcs50/hashtable.h"
#include "../common/pagedir.h"
#include "../common/index.h"
#include "../common/word.h"

// function prototypes
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename);
static void indexBuild(char* pageDirectory, char* indexFilename);
static void indexPage(index_t* index, webpage_t* page, const int docID);

/* ********************* main ************************ */
int main(const int argc, char* argv[]){

    char* pageDirectory; //Directory that we will writefiles to for each crawled webpage
    char* indexFilename; //File name for the index file we will write

    //parse arguments to check if they match correct input type, if so assign to variables
    parseArgs(argc, argv, &pageDirectory, &indexFilename);

    //crawl pages and add them to pageDirectory
    indexBuild(pageDirectory, indexFilename);
    
    //free memory at the end
    mem_free(pageDirectory);
    mem_free(indexFilename);
    exit(0);
}

/**************** parseArgs ****************/
/* 
* Input: char* for page directory, char* for indexFilename
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
    if((fp = fopen(*indexFilename, "w")) != NULL) {
        fclose(fp);
    } else { 
        fprintf(stderr, "Cannot open %s for writing\n", *indexFilename);
        exit(22);
    }

    // return if tests passed
    return;
}

/**************** indexBuild ****************/
/* 
* Input: char* for page directory
* 
* Creates an index structure, loops through webpage files in pageDIrectory
* Calls indexPage to count words in each webpage file and add them to the index
* prints the index to a file specified by indexFilename.
 *
* Returns: nothing
*/
static void indexBuild(char* pageDirectory, char* indexFilename)
{
    index_t* invertedIndex;     //holds inverted index of words in the pageDirectory
    const int SLOTS = 300;      //number of index slots
    int docID = 1;              //increments docID
    webpage_t* current = NULL;         //stores current webpage
    FILE* fp;                   //stores the webpage to be sent to pagedir_load

    invertedIndex = mem_assert(index_new(SLOTS), "inverted index\n");

    //load each page in pageDirectory by incrementing docID
    while(true){
        if(docID == 1){
            if(pagedir_load(&current, pageDirectory, docID)){
                indexPage(invertedIndex, current, docID);
                docID++;
            }
            else{
                //unrecoverable error if cant open the first file to read
                fprintf(stderr, "Cannot open first file in %s for reading\n", pageDirectory);
                exit(11);
            }
        }
        else if(pagedir_load(&current, pageDirectory, docID)){
            indexPage(invertedIndex, current, docID);
            docID++;
        }
        else{
            //we've reached the last webpage file in pageDirectory
            break;
        }
    }
    fp = fopen(indexFilename, "w");

    //print the inverted index to indexFilename
    index_write(invertedIndex, fp);

    //delete the inverted index
    index_delete(invertedIndex, (void(*)(void*))counters_delete);

    fclose(fp);
    
}

/**************** indexPage ****************/
/* 
* Input: index_t* for index, webpage_t* for webpage to be added to the index, int for docID
* 
* Scans a webpage file for words which are counted using an index data structure
* 
* Returns: nothing
*/
static void indexPage(index_t* invertedIndex, webpage_t* page, const int docID)
{
    mem_assert(invertedIndex, "index");
    mem_assert(page, "webpage in indexpage");
    int pos = 0; //tracks position of word for weppage_getNextWord
    char* result; //stores normalized word

    //loops through each word in the webpage
    while ((result = webpage_getNextWord(page, &pos)) != NULL) {
        //normalize the word by converting to lowercase
        word_normalize(result);
        if(strlen(result) > 2){
            if (index_find(invertedIndex, result) == NULL){
                 //if the word is not in the inverted index then add it
                index_insert(invertedIndex, result);
            }
        // increment the count of the word for the given docID
        index_increment(invertedIndex, result, docID);
        }
        mem_free(result);
    }
    webpage_delete(page);
}