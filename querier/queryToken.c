/* 
 * queryToken.c
 *
 * queryToken functions as a linked list to hold tokenized queries
 *
 * see queryToken.h for more information.
 *
 * Jack McMahon February 17th, 2022
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/mem.h"

/**************** global types ****************/
typedef struct queryToken {
    char* word;               // pointer to word
    struct queryToken *next;  // link to next queryToken
}   queryToken_t;

/**************** global functions ****************/


/**************** queryToken_new ****************/
queryToken_t* queryToken_new(char* word){

    char* localWord = mem_malloc_assert((strlen(word) + 1),  "local word");
    strcpy(localWord, word);

    queryToken_t* token = mem_malloc(sizeof(queryToken_t));

    if (token == NULL) {
      return NULL;              
    } else {
      // initialize contents of queryToken structure
      token->word = localWord;
      token->next = NULL;
      return token;
    }
}

/**************** queryToken_addNext ****************/
void queryToken_addNext(queryToken_t* token, queryToken_t* nextToken){
    token->next = nextToken;
}

/**************** queryToken_getWord****************/
char* queryToken_getWord(queryToken_t* token){
    return token->word;
}

/**************** queryToken_getNext ****************/
queryToken_t* queryToken_getNext(queryToken_t* token){
    return token->next;
}

/**************** queryToken_print ****************/
void queryToken_print(queryToken_t* token){
    queryToken_t* currentToken = token;
    queryToken_t* nextToken;
    char* word = queryToken_getWord(currentToken);
    if(word != NULL){
      printf("Query: %s", word);
    }
    while((nextToken = queryToken_getNext(currentToken)) != NULL ){
      word = queryToken_getWord(nextToken);
      printf(" %s", word);
      currentToken = nextToken;
    }
    printf("\n");
}

/**************** queryToken_checkSyntax ****************/
bool queryToken_checkSyntax(queryToken_t* token){
    queryToken_t* currentToken = token;
    queryToken_t* nextToken;
    char* word = queryToken_getWord(currentToken);
    bool lastAnd = false;
    bool lastOr = false;

    //check if the first word is 'and' or 'or'
    if((strcmp(word, "and")) == 0 ||(strcmp(word, "or")) == 0 ){
      printf("Error: '%s' cannot be first\n", word);
      return false;
    }
    //loop over the interior words to check if 'and' or 'or'  are adjacent
    while((nextToken = queryToken_getNext(currentToken)) != NULL ){
      word = queryToken_getWord(nextToken);
      if((strcmp(word, "and")) == 0 ){
          if(lastAnd){
              printf("Error: 'and' and 'and' cannot be adjacent\n");
              return false;
          }
          else if(lastOr){
              printf("Error: 'or' and 'and' cannot be adjacent\n");
              return false;
          }
          lastAnd = true;
          lastOr = false;
      }
      else if((strcmp(word, "or")) == 0 ){
          if(lastAnd){
              printf("Error: 'and' and 'or' cannot be adjacent\n");
              return false;
          }
          else if(lastOr){
              printf("Error: 'or' and 'or' cannot be adjacent\n");
              return false;
          }
          lastAnd = true;
          lastOr = false;
      }
      else{
        lastAnd = false;
        lastOr = false;
      }
      currentToken = nextToken;
    } 
    //now check if the last word is 'and' or 'or'
    if((strcmp(word, "and")) == 0 ||(strcmp(word, "or")) == 0 ){
      printf("Error: '%s' cannot be last\n", word);
      return false;
    }
    return true;
}

/**************** queryToken_delete ****************/
void queryToken_delete(queryToken_t* token){

    if (token != NULL) {
        queryToken_t* currentToken = token;
        while(currentToken != NULL) {
            queryToken_t* next = currentToken->next; 
            mem_free(currentToken->word);
            mem_free(currentToken);                  
            currentToken = next;                      
        }
    }
}