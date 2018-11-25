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


struct inbet_node{           /*struct to store inbetween relations*/
  int rowIDS[BUFFERSIZE];        /*array of rowIDS*/
  int num_tuples;               /*number of tuples in array*/
  struct inbet_node *next;          /*pointer to the next node */
};
typedef struct inbet_node inbet_node;

struct inbet_list{
  inbet_node *head;       /*pointer to the head of the list*/
  inbet_node *current;    /*pointer to the current node of the list*/
  int total_tuples;       /*total tuples in list*/
};
typedef struct inbet_list inbet_list;

struct inbetween_results{
  inbet_list **inbet_lists; /*array of pointers to lists*/
  int num_lists;                  /*number of lists*/
};

typedef struct inbetween_results inbetween_results;


Relation *BuildRelation(inbet_list *list,Relation *init_relation);
inbetween_results *InitInbetResults(int n);
inbet_node *InitInbetNode();
void UpdateInbetList(inbet_list *list,int key);
void ReInitInbetList(inbet_list *list);


/*body of functions in lists.c */
Result *ListInit();
void InsertResult(Result *head,int rowID1,int rowID2);
void PrintResults(Result *head);
void deletelist(Result *head);

#endif
