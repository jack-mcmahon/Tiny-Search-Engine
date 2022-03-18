/* 
 * indextest.c
 *
 * This is a tester for index. 
 * It takes two arguments:
 *          char*  oldIndexFilename
 *          char*  newIndexFilename
 * 
 * Indextest reads the old index file into an index data structure.
 * It then writes the index data structure into a new index file that should be identical to the old one
 * 
 * To compare results use /cs50-dev/shared/tse/indexcmp oldIndexFilename newIndexFilename
 * 
 * Jack McMahon, February 14th 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/counters.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include "../common/index.h"

/* ********************* main ************************ */
int main(const int argc, char* argv[]){

    index_t* invertedIndex = NULL;
    FILE* fp;

    if(argc != 3){
        //throw error if incorrect number of arguments
        fprintf(stderr, "Incorrect number of arguments.\n");
        exit(1);
    }

    //load old index into an inverted index data type
    fp = fopen(argv[1], "r");
    index_load(&invertedIndex, fp);
    fclose(fp);

    //load old index into an inverted index data type
    fp = fopen(argv[2], "w");
    index_write(invertedIndex, fp);
    fclose(fp);
    index_delete(invertedIndex, (void(*)(void*))counters_delete);

    exit(0);

}
