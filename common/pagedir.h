/* 
 * pagedir.h
 *
 * These are methods for initializing, validating, and loading a pageDirectory
 * To be used by TSE modules Crawler, Indexer, and Querier
 * 
 * Jack McMahon, February 14th 2022
 */


#ifndef __PAGEDIR_H
#define __PAGEDIR_H


#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"

/**************** pagedir_validate ****************/
/* 
 * Caller provides:
 *      char* for pageDirectory to validate
 * 
 * We validate the pageDirectory by checking if it contains a .crawler file
 * 
 * We return: 
 *      true if the pageDirectory contains a .crawler file
 *      false if it does not contain a .crawler file
 */
bool pagedir_validate(const char* pageDirectory);

/**************** pagedir_init ****************/
/* 
 * Caller provides:
 *     char* for the pageDirectory
 * 
 * We construct the pathname for the .crawler in pageDirectory
 * 
 * We return: 
 *     true if successful in opening .crawler file for writing
 *     false if unsuccessful
 */
bool pagedir_init(const char* pageDirectory);


/**************** pagedir_save ****************/
/* 
 * Caller provides:
 *      webpage_t* for webpage infornation 
 *      char* for pageDirectory to save webpage information to as a file
 *      const int to label file with webpage information
 * 
 * We construct the pathname for the page fil in pageDirectory
 * Open that file for writing and print webpage URL, depth and HTML then close the file
 * 
 * We return: nothing
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**************** pagedir_load ****************/
/* 
 * Caller provides:
 *      webpage_t* that will be given loaded webpage inform ation
 *      char* for pageDirectory to load webpage file information from
 *      const int to identify the webpage file to be loaded from pageDIrectory
 * 
 * We construct find the pathname to the sought after webpage file by scanning pageirectory and docID
 * We read the webpage file into the webpage_t data structure
 * We return: true if successful creation of webpage_t, false if otheriwse
 */
bool pagedir_load(webpage_t** page, const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H
