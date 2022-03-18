/* 
 * index.h
 *
 * An index is a hashtable of countersets. It is used by the Indexer module for TSE
 * 
 * The index stores a hashtable of of sets. 
 *      Each set key is a char* for a word.
 *      Each set points to a counterset
 *      Each counterset contains (docID, count) pairs.
 * 
 * index contains methods to create, modify, load, write, and iterate indexes.
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
typedef struct index index_t;  // opaque to users of the module

/**************** functions ****************/

/**************** index_new ****************/
/* Create a new (empty) index.
 *
 * Caller provides:
 *   number of slots to be used for the index (must be > 0).
 * We return:
 *   pointer to the new index; return NULL if error.
 * We guarantee:
 *   index is initialized empty.
 * Caller is responsible for:
 *   later calling index_delete.
 */
index_t* index_new(const int numslots);

/**************** index_insert ****************/
/* Insert word, identified by key (string).
 *
 * Caller provides:
 *   valid pointer to index, valid string for key;
 * We return:
 *   false if key exists in index, any parameter is NULL, or error;
 *   true iff new item was inserted.
 * Notes:
 *   The key string is copied for use by the index; that is, the module
 *   is responsible for allocating memory for a copy of the key string, and
 *   later deallocating that memory; thus, the caller is free to re-use or 
 *   deallocate its key string after this call.  
 *   You must first add a word using index_insert before incrementing docID counters
 */
bool index_insert(index_t* index, const char* word);

/**************** index_increment ****************/
/* Increment counter for a docID for a given word
 *
 * Caller provides:
 *   valid pointer to index, valid string for key, valid in for docID
 * We return:
 *   The count of docID for word after insertion 
 */
int index_increment(index_t* index, const char* word, int docID);

/**************** index_find ****************/
/* Return the counterset associated with the given word.
 *
 * Caller provides:
 *   valid pointer to index, valid string for word.
 * We return:
 *   pointer to the counters_t corresponding to the given word, if found;
 *   NULL if index is NULL, word is NULL, or word is not found.
 * Notes:
 *   the index is unchanged by this operation.
 *   You must first add a word using index_insert before incrementing docID counters
 */
void* index_find(index_t* index, const char* word);

/**************** index_load ****************/
/* Load an index from a file into a given index_t struct
 *
 * Caller provides:
 *   valid double pointer to index, valid opened file.
 * We load the index from the file into the passed index pointer.
 * We return nothing.
 *
 */
void index_load(index_t** index, FILE* fp);

/**************** index_iterate ****************/
/* iterate over the whole table;
 * 
 * Caller provides:
 *   valid pointer to index, 
 *   void *arg
 *   itemfunc
 * We call itemfunc on each entry in the index.
 *  
 * Note: the index and its contents are not changed by this function.
 */
void index_iterate(index_t *index, void *arg, void (*itemfunc)(void *arg, const char *key, void *item));

/**************** index_write ****************/
/* write the whole table; provide the output file and func to print each item.
 * 
 * Caller provides:
 *   valid pointer to index, 
 *   FILE open for writing,
 * We print:
 * one line per index slot in FILE, start with word then listing (key,item) pairs in that slot.
 * Note:
 *   the index and its contents are not changed by this function,
 */
void index_write(index_t* index, FILE* fp);

/**************** index_delete ****************/
/* Delete index, calling a delete function on each item.
 *
 * Caller provides:
 *   valid index pointer,
 *   valid pointer to function that handles one item (may be NULL).
 * We do:
 *   if index==NULL, throw error
 *   otherwise, unless itemfunc==NULL, call the itemfunc on each item.
 *   free all the key strings, and the index itself.
 * Notes:
 *   We free the strings that represent key for each item, because 
 *   this module allocated that memory in index_insert.
 *   We handle much of this by calling hashtable_delete.
 */
void index_delete(index_t* index, void (*itemdelete)(void* item));