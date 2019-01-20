#ifndef _INBETWEEN_H__
#define _INBETWEEN_H__
#include "rhj.h"
#include <stdint.h>
#define BUFFERSIZE 1024*1024/64

typedef struct Relation Relation;

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
  int joined;
};

typedef struct inbet_list inbet_list;


struct inbetween_results{
  int **inbetween;          /*array of pointers to lists*/
  int *joined;
  int num_lists;            /*number of lists*/
  int **joined_pairs;
};

typedef struct inbetween_results inbetween_results;

inbet_node *InitInbetNode();
inbet_list *InitInbetList();
inbetween_results *InitInbetResults(int n);
void InsertInbetList(inbet_list *list,int key);
Relation *BuildKeyRelation(inbet_list *list);
inbetween_results *UpdateInbetList2(inbetween_results *inb_results,inbet_list *results,int rel_id);
void FreeInbetList(inbetween_results *list);
int GetValue(inbet_list *list,int pos);
inbetween_results *UpdateInbetList(inbetween_results *inb_results,inbet_list *result1,inbet_list *result2,int rel1,int rel2);
void PrintInbetList(inbet_list *list);
#endif
