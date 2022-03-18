/* 
 * queryToken.h
 *
 * queryToken functions as a linked list to hold tokenized queries
 *
 *
 * Jack McMahon February 17th, 2022
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/mem.h"

/**************** global types ****************/
typedef struct queryToken queryToken_t;

/**************** global functions ****************/

/**************** queryToken_new ****************/
/* Create a new queryToken that is created with a passed char* word
 *
 * We return:
 *   pointer to a new queryToken
 * Caller is responsible for:
 *   freeing the passed word since we make a local copy
 */
queryToken_t* queryToken_new(char* word);

/**************** queryToken_addNext ****************/
/* Create a link between two queryTokens
 * We set:
 * token->next = nextToken
 * We return:
 *   nothing
 * 
 */
void queryToken_addNext(queryToken_t* token, queryToken_t* nextToken);


/**************** queryToken_getWord****************/
/* We return:
 *   char* word stored in a specified queryToken
 */
char* queryToken_getWord(queryToken_t* token);

/**************** queryToken_getNext ****************/
/* We return:
 *   pointer to the queryToken stored in token->next
 */
queryToken_t* queryToken_getNext(queryToken_t* token);

/**************** queryToken_print ****************/
/* Iterate through the linked list and print each stored word with single spaces between
 * Caller Provides:
 *          head queryToken of the linked list
 */
void queryToken_print(queryToken_t* token);

/**************** queryToken_checkSyntax ****************/
/* Iterate through the linked list and verify syntax matches specifications
 * Caller Provides:
 *          head queryToken of the linked list
 * We return:
 *         true if syntax is correct, false if otherwise
 */
bool queryToken_checkSyntax(queryToken_t* token);

/**************** queryToken_delete ****************/
/* Iterate through the linked list and delete each queryToken and char* word
 * Caller Provides:
 *          head queryToken of the linked list
 * We return:
 *         nothing
 */
void queryToken_delete(queryToken_t* token);
