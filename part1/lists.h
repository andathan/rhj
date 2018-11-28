#ifndef _LISTS_H__
#define _LISTS_H__
#include <stdlib.h>
#include <stdio.h>
#define BUFFERSIZE 1024*1024/64


struct ResultNode{
 int32_t buffer[BUFFERSIZE][2];
 int32_t size;
 struct ResultNode *next;
};

typedef struct ResultNode ResultNode ;

struct Result{
  ResultNode *first;
  ResultNode *current;
};

typedef struct Relation Relation;
typedef struct Result Result;
typedef struct ResultNode ResultNode;


/*body of functions in lists.c */
Result *ListInit();
void InsertResult(Result *head,int rowID1,int rowID2);
void PrintResults(Result *head);
void deletelist(Result *head);

#endif
