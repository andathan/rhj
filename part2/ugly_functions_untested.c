#include "rhj.h"
#include <stdio.h>
#include <stdlib.h>
#include "lists.h"

Relation *BuildRelation(inbet_list *list,Relation *init_relation){
  /* input: list of inbetween results , initial Relation
      output : a Relation with inbetween tuples*/
  int i,c=0;
  Relation *new_relation = (Relation *)malloc(sizeof(Relation));
  new_relation->tuples = malloc(sizeof(tuple)*list->total_tuples);
  new_relation->num_tuples = list->total_tuples;

  inbet_node *current = list->head;

  while(current!=NULL){ //for every node in list
    for(i=0;i<current->num_tuples;i++){  //for every rowID in node
      new_relation->tuples[c].key = current->rowIDS[i];
      new_relation->tuples[c].payload = init_relation[new_relation->tuples[c].key].payload;
      c++;
    }
    current = current->next;
  }
  return new_relation;
}


inbetween_results *InitInbetResults(int n){
  /*initializes the struct of inbtween results*/
  int i;
  inbetween_results *results = malloc(sizeof(inbetween_results));
  results->num_lists=n;
  results->inbet_lists = malloc(sizeof(inbet_list *)*results->num_lists);
  for(i=0;i<results->num_lists;i++){
    results->inbet_lists[i] = ( inbet_list * ) malloc(sizeof(inbet_list));
    results->inbet_lists[i]->head=NULL;
    results->inbet_lists[i]->current=NULL;
    results->inbet_lists[i]->total_tuples=0;
  }
  return results;
}

inbet_node *InitInbetNode(){
  inbet_node *node = malloc(sizeof(inbet_node));
  node->num_tuples=0;
  node->next=NULL;
  return node;
}

void UpdateInbetList(inbet_list *list,int key){
  if(list == NULL) return;
  if(list->head==NULL){     /*list is empty*/
    list->head = InitInbetNode();
    list->current = list->head;
  }else if(list->current->num_tuples==BUFFERSIZE-1){ /*current buffer is full*/
    list->current->next = InitInbetNode();
    list->current = list->current->next;
  }
  list->current->rowIDS[list->current->num_tuples]=key;
  list->current->num_tuples++;
  list->total_tuples++;
}


void ReInitInbetList(inbet_list *list){
  if(list==NULL || list->head==NULL) return;
  inbet_node *current = list->head;
  inbet_node *temp = list->head;
  while(current!=NULL){
    temp = current;
    current = current->next;
    free(temp);
  }
  list->total_tuples = 0;
  list->head=NULL;
  list->current=NULL;
}
