/* 
 * pagedir.c
 *
 * These are methods for initializing, validating, and loading a pageDirectory
 * To be used by TSE modules Crawler, Indexer, and Querier
 * 
 * See pagedir.h for usage documentation
 *
 * Jack McMahon, February 14th 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "../libcs50/mem.h"
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"


/**************** pagedir_validate ****************/
/* See pagedir.h for usage documentation */
bool pagedir_validate(const char* pageDirectory){

    const int CRAWLERLENGTH = 12;                                                            // length of ./crawler
    char* pathname = mem_malloc_assert(strlen(pageDirectory) + CRAWLERLENGTH, "pathname2");   //holds path name
    FILE* fp;                                                                                //file to write in directory
    //copy /.crawler onto the directory name to create a pathname
    strcpy(pathname, pageDirectory);
    strcat(pathname, "/.crawler");

    //check if the .crawler file exists
    if((fp = fopen(pathname, "r")) != NULL) {
        fclose(fp);
    } else { 
        fprintf(stderr, "No .crawler file found in pageDirectory\n");
        mem_free(pathname);
        return false;
    }

    //now write pathname of pageDirectory/1
    strcpy(pathname, pageDirectory);
    strcat(pathname, "/1");

    //check if pageDirectory/1 file exists
     if((fp = fopen(pathname, "r")) != NULL) {
        mem_free(pathname);
        fclose(fp);
        return true;
    } else { 
        fprintf(stderr, "No readable /1 file found in pageDirectory\n");
        mem_free(pathname);
        return false;
    }
}

/**************** pagedir_init ****************/
/* See pagedir.h for usage documentation */
bool pagedir_init(const char* pageDirectory)
{
    const int CRAWLERLENGTH = 12;                                                            // length of ./crawler
    char* pathname = mem_malloc_assert(strlen(pageDirectory) + CRAWLERLENGTH, "pathname");   //holds path name
    FILE* out;                                                                              //file to write in directory
    //copy /.crawler onto the directory name to create a pathname
    strcpy(pathname, pageDirectory);
    strcat(pathname, "/.crawler");


    //check if the .crawler file is writeable
    if((out = fopen(pathname, "w")) != NULL) {
        mem_free(pathname);
        fclose(out);
        return true;
    } else { 
        fprintf(stderr, "Cannot open .crawler file for writing\n");
        mem_free(pathname);
        return false;
    }
    
}

/**************** pagedir_save ****************/
/* See pagedir.h for usage documentation */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
    //check if webpage or directory are null
    if(page == NULL || pageDirectory == NULL){
        fprintf(stderr, "Null values sent to pagedir_save \n");
        exit(5);
    }
    else{
        FILE* out;
        //first make docID into a char*
        int idLength = 10; 
        char* pageName = mem_malloc(idLength*sizeof(char)); 
        sprintf(pageName,"%d", docID);

        //now create and malloc a char* for the pathname to the file
        int length = strlen(pageDirectory) + idLength;
        char* pathName = mem_malloc(length*sizeof(char)+1);

        //copy the directory and id into the pathname
        strcpy(pathName, pageDirectory);
        strcat(pathName, "/");
        strcat(pathName, pageName);

        //check if file is writeable
        if((out = fopen(pathName, "w")) == NULL) {
        fprintf(stderr, "Cannot write file %s \n", pathName);
        exit(6);
        }

        char* url = webpage_getURL(page); // retrieve webpage URL
        int depth = webpage_getDepth(page); // retrieve webpage depth
        char* html = webpage_getHTML(page); // retrieve webpage contents

        //print webpage URL, depth, and contents to file in directory
        fprintf(out, "%s\n%d\n%s", url, depth, html);

        mem_free(pageName);
        mem_free(pathName);
        fclose(out);
    }
}

/**************** pagedir_load ****************/
/* See pagedir.h for usage documentation */
bool pagedir_load(webpage_t** page, const char* pageDirectory, const int docID)
{
    FILE* fp;           //file to open webpage file
    char* url;          //holds url from line 1 of webpage file
    char* tempDepth;    //holds temporary tepth of webpage
    int depth;          //holds depth from line 2 of webpage file
    char* html;         //holds html from after line 2 of webpage file

     //check if directory is null or docID less than one
    if(pageDirectory == NULL || docID < 1){
    return false;
    }
    else{
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
            return false;
        }
        else{
            //scan the webpage file and extract url, depth and html
            url = file_readLine(fp);
            tempDepth = file_readLine(fp);
            depth = atoi(tempDepth);
            html = file_readFile(fp);

            //assign url, depth, and html to page passed from caller
            *page = mem_assert(webpage_new(url, depth, html), "webpage load new\n");
            mem_free(pageID);
            mem_free(pathName);
            mem_free(tempDepth);
            fclose(fp);
            return true;
        }
    }
}