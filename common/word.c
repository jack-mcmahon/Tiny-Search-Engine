/* 
 * word.c
 *
 * Contains a single method to normalize a word by converting it to all lowercase.
 * See word.h for usage documentation
 * 
 * Jack McMahon, February 14th 2022
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/**************** word_normalize ****************/
/* See word.h for usage documentation */
void word_normalize(char* word){

    for(int i = 0; i < strlen(word); i++){
        char temp = tolower(word[i]);
        word[i] = temp;
    }
}