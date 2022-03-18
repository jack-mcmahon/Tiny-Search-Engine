/* 
 * word.h
 *
 * Contains a single method to normalize a word by converting it to all lowercase.
 * 
 * Jack McMahon, February 14th 2022
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

/**************** word_normalize ****************/
/* 
 * Caller provides:
 *     char* for the word
 * 
 * We convert all letters to lowercase
 * 
 * We return: 
 *     nothing
 */
void word_normalize(char* word);