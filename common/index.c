/* 
 * index.c
 *
 * An index is a hashtable of countersets.
 *
 * see index.h for more information.
 *
 * Jack McMahon, February 14th, 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "../libcs50/mem.h"
#include "../libcs50/counters.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/file.h"

/**************** global types ****************/
typedef struct index {
    hashtable_t *ht;       
} index_t;

/**************** local functions ****************/

/**************** index_printCounters ****************/
/* 
 * Input: FILE* as arg, docID as key, int for count.
 * Prints each (DocID, count) pair in the counterset for a word in index
 * Passed to counters_iterate by index_printLines.
 * Matches the output format specified for an index file.
 */
static void index_printCounters(void *arg, const int key, const int count){
    FILE* fp = arg;     //assigns arg as a file pointer
    int docID = key;    //assigns key as docID

    //prints each (docID, count) pair
    fprintf(fp, "%d %d ", docID, count);
}

/**************** index_printLines ****************/
/* 
 * Input: FILE* as arg, char* as key, counters_t* as item.
 *
 * This function is meant to be passed to hashtable_iterate by index_write.
 * Prints the key word for each line, then calls counters_iterate to print the rest.
 * Matches the output format specified for an index file.
 *
*/
static void index_printLines(void *arg, const char *key, void *item){
    FILE* fp = arg;                 //assigns arg as a file pointer
    const char* word = key;         //assigns key as word
    counters_t* counterset = item;  //assigns item  as counterset_t*

    //prints the word to start each line
    fprintf(fp, "%s ", word);

    //calls counters iterate with index_printCounters to print the rest of the line
    counters_iterate(counterset, fp, index_printCounters);

    fprintf(fp, "\n");
}

/**************** functions ****************/

/**************** index_new ****************/
/* See index.h for usage documentation */
index_t* index_new(const int numslots){
    //build index
    index_t* index = mem_malloc_assert(sizeof(index_t), "index");

    //build hashtable for index
    hashtable_t* ht = mem_assert(hashtable_new(numslots), "index");
    index->ht = ht;

    return index;
}

/**************** index_insert ****************/
/* See index.h for usage documentation */
bool index_insert(index_t* index, const char* word){

    //create a counter set for the given word
    counters_t* counterset = mem_assert(counters_new(), "index");

    //add a counter for that word to the index;
    return hashtable_insert(index->ht, word, counterset);
}

/**************** index_increment ****************/
/* See index.h for usage documentation */
int index_increment(index_t* index, const char* word, int docID){
    counters_t* counterset;     //pointer to counterset in hashtable for gievn word
    int count;                  //stores count of docID for a word in index

    counterset =  hashtable_find(index->ht, word);
    //check if counterset exists
    mem_assert(counterset, "counterset to increment");
    count = counters_add(counterset, docID);
    return count;
}

/**************** index_find ****************/
/* See index.h for usage documentation */
void* index_find(index_t* index, const char* word){
    counters_t* result; //stores result counterset
    result = hashtable_find(index->ht, word);
    return result;
}

/**************** index_load ****************/
/* See index.h for usage documentation */
void index_load(index_t** index, FILE* fp){
    //assert index and fp are not null
    mem_assert(index, "index");
    mem_assert(fp, "file");

    int slots;                  //number of slots in index
    char* word;                 //word values for index
    int docID;                  //docID values for index
    int count;                  //count values for index

    //initialize the inverted index to have the same number of slots as lines in the index file
    slots = file_numLines(fp);
    *index = mem_assert(index_new(slots), "inverted index\n");

    //read word at the start of the line
    while((word = file_readWord(fp)) != NULL){
        //insert word into the index
        index_insert(*index, word);
        //advance along the line two numbers at a time until end
        while(fscanf(fp,"%d %d ", &docID, &count) == 2){
            while(count > 0){
                //increment index for docID until it reaches count
                index_increment(*index, word, docID);
                count--;
            }
        }
        mem_free(word);
    }
}

/**************** index_iterate ****************/
/* See index.h for usage documentation */
void index_iterate(index_t *index, void *arg, void (*itemfunc)(void *arg, const char *key, void *item)){
    mem_assert(index, "index");
    mem_assert(arg, "arg");
    hashtable_iterate(index->ht, arg, itemfunc);
}

/**************** index_write ****************/
/* See index.h for usage documentation */
void index_write(index_t* index, FILE* fp){
    mem_assert(index, "index");
    mem_assert(fp, "file");
    index_iterate(index, fp, index_printLines);
}

/**************** index_delete ****************/
/* See index.h for usage documentation */
void index_delete(index_t* index, void(*itemdelete)(void *item)){
    mem_assert(index, "index");
    hashtable_delete(index->ht, itemdelete);
    mem_free(index);
}

