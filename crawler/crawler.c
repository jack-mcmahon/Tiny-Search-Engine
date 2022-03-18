/* 
 * crawler.c
 *
 * This is the first module of the TSE for CS50
 * Written with consultation of CS50 knowledge pages and using methods from libcs50
 * 
 * Crawler takes three arguements, a seed URL, page Directory, and depth
 * 
 * It will crawl from the seed URL to linked URLS going as for as the depth argument specifies.
 * It will then save the contents of each crawled webpage to a file in the page Directory.
 * 
 * Input Specificiations:
 * The Seed URL must be an internal URL.
 * depth must be between 0 and 10(inclusive).
 * The page directory must be writeable and also must not contain
 * any files whose name is an integer
 * 
 * Crawler also pauses for one second between page fetches to avoid overloading the webservser
 * 
 * Jack McMahon, February 5th 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/mem.h"
#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../common/pagedir.h"


// function prototypes
static void parseArgs(const int argc, char* argv[], char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

/* ********************* main ************************ */
int main(const int argc, char* argv[]){


    char* seedURL; //URL of the seed webpage
    char* pageDirectory; //Directory that we will writefiles to for each crawled webpage
    int maxDepth = 0; //integer for the maximum depth our program will search 

    //parse arguments to check if they match correct input type, if so assign to variables
    parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

    //crawl pages and add them to pageDirectory
    crawl(seedURL, pageDirectory, maxDepth);
    
    //free memory at the end
    mem_free(pageDirectory);

    exit(0);
}

/**************** parseArgs ****************/
/* 
* Input: char* for seed URL, char* for page directory, int for max depth
 * 
 * Makes sure arguments match the given specifications, if so then assigns them to variables
*/
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth) {
    if(argc != 4){
        //throw error if incorrect number of arguments
        fprintf(stderr, "Incorrect number of arguments.\n");
        exit(1);
    }

    char* normalizedURL; // stores normalized URL
    *seedURL = mem_malloc(strlen(argv[1]) + 1);
    //check if URL can be normalized
    if((normalizedURL = normalizeURL(argv[1])) == NULL){
        fprintf(stderr, "Failed to normalize URL\n");
        exit(17);
    }

    strcpy(*seedURL, normalizedURL);
    mem_free(normalizedURL);

    if( ! isInternalURL(*seedURL)){
        //throw error if seed URL is not internal
        fprintf(stderr, "Seed URL is not an internal URL.\n");
        exit(2);
    }

    *pageDirectory = mem_malloc_assert((strlen(argv[2]) + 1), "pageDirectory");
    strcpy(*pageDirectory, argv[2]);
    
    if ( ! pagedir_init(*pageDirectory)) {
        //throw error if can't write in  given page directory
        fprintf(stderr, "Failed to construct .crawler file to %s.\n", *pageDirectory);
        exit(3);
    }

    int n = atoi(argv[3]);
    if (n > 10 || n < 0){
        //throw error max depth is out of bounds
        fprintf(stderr, "Max Depth is out of range.\n");
        exit(4);
    } else {
        *maxDepth = n;
    }
    
    // return if tests passed
    return;
}

/**************** Crawl ****************/
/* 
* Input: char* for seed URL, char* for page directory, int for max depth
* 
* Finds linked URLs for a given webpage and adds them to the bag and tashatble if not at max depth
*/
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
    hashtable_t* pagesSeen; //hashtable that holds pages seen
    bag_t* pagesToCrawl;    //bag that holds pages to crawl
    webpage_t* current;     //webpage_t to hold the current webpage
    const int SLOTS = 200;  // num slots for hashtable
    int i = 1;              //counter for  docIDs
    
    //create hashtable of pages seen and add the seed URL
    pagesSeen = mem_assert(hashtable_new(SLOTS), "pagesSeen new\n");
    
    if ( ! (hashtable_insert(pagesSeen, seedURL, ""))) {
        fprintf(stderr, "error\n");
        exit(12);
    }
    
    //create a bag of unexplored pages and add a webpage structure for the seednode
    pagesToCrawl = mem_assert(bag_new(), "bag new\n");
    current = mem_assert(webpage_new(seedURL, 0, NULL), "webpage new\n");
    bag_insert(pagesToCrawl, current);
    
    //loop through all pages reachable from the seed up to the max depth
    //fetch HTML and save each page to the directory if successful
    while((current = bag_extract(pagesToCrawl)) != NULL) {
        if (webpage_fetch(current)) {
            #ifdef VERBOSE
              printf("%d, Fetched, %s\n", webpage_getDepth(current), webpage_getURL(current));
            #endif
            pagedir_save(current, pageDirectory, i);
            i++;
            //if webpage is not at max depth scan for connected pages
            if  (webpage_getDepth(current) < maxDepth) {
                #ifdef VERBOSE
                     printf("%d, Scanning, %s\n", webpage_getDepth(current), webpage_getURL(current));
                #endif
                pageScan(current, pagesToCrawl, pagesSeen);
            }
        }
        //free webpage from the bag
        webpage_delete(current);
    }
    //free data structures
    hashtable_delete(pagesSeen, NULL);
    bag_delete(pagesToCrawl, webpage_delete);
}

/**************** pageScan ****************/
/* 
 * Input: webpage_t to scan, bag_t of pages tp crawl, hashtable_t of pages seen
 * 
 * Finds linked URLs for a given webpage and adds them to the bag and tashatble if not at max depth
*/
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
    int pos = 0; //tracks position of URL for weppage_getNextURL
    char* result; //stores the current URL
    char* temp; //stores the tmp URL
    int depth; //stores page depth
    webpage_t* current; // stores webpage to be added
    while ((temp = webpage_getNextURL(page, &pos)) != NULL) {
        result = normalizeURL(temp);
        //find depth of the page to be added
        depth = webpage_getDepth(page);
        #ifdef VERBOSE
              printf("%d, Found, %s\n", depth, result);
        #endif
        mem_free(temp);
        if(isInternalURL(result)) {
            if(hashtable_insert(pagesSeen, result, "")) {
                //create new webpage_t for the found URL ands add to the bag of pages to crawl
                current = webpage_new(result, depth + 1, NULL);
                bag_insert(pagesToCrawl, current);
                #ifdef VERBOSE
                    printf("%d, Added, %s\n", depth, result);
                #endif
            } else {
                // free if duplicate
                #ifdef VERBOSE
                    printf("%d, IgnDupl, %s\n", depth, result);
                #endif
                mem_free(result);
            }
        } else {
            // free if external
            #ifdef VERBOSE
              printf("%d, IgnExtrn, %s\n", depth, result);
            #endif
            mem_free(result); 
        }
    }
    
}

