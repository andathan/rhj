#include <stdio.h>
#include <stdlib.h>
#include "inbetween.h"

inbet_node *InitInbetNode(){
  inbet_node *node = malloc(sizeof(inbet_node));
  node->num_tuples=0;
  node->next=NULL;
  return node;
}

inbet_list *InitInbetList(){
  inbet_list *list = (inbet_list *) malloc(sizeof(inbet_list));
  list->head=NULL;
  list->current=NULL;
  list->total_tuples=0;
  list->joined=-1;
}

inbetween_results *InitInbetResults(int n){
  /*initializes the struct of inbtween results*/
  int i;
  inbetween_results *results = malloc(sizeof(inbetween_results));
  results->num_lists=n;
  results->inbet_lists = malloc(sizeof(inbet_list *)*results->num_lists);
  for(i=0;i<results->num_lists;i++){
    results->inbet_lists[i] = InitInbetList();
  }
  return results;
}

int divi(int num1,int num2,int *div , int *mod){
  *div=0;
  while(num1>=num2){
    num1 -= num2 ;
    (*div)++;
  }
  *mod = num1;
}
int GetValue(inbet_list *list,int pos){
  int node ,rem ;
  divi(pos,BUFFERSIZE,&node ,&rem);
  printf("pos = %d ,node = %d , rem = %d\n",pos,node,rem );
  inbet_node *current = list->head;
  while(node!=0){
      current = current->next;
      node--;
  }
  return current->rowIDS[rem];
}


inbetween_results *UpdateInbetList(inbetween_results *inb_results,inbet_list *result1,inbet_list *result2,int rel1,int rel2){
  /*ta row_results stin proti stili periexoun tis theseis twn keys sta results pou prepei na mpoun sta kenuria results*/
  //brisko poia prepei na kratiso apo ta proigoumena,sta row_results h proti stili tha mu deixnei tis
  //grammes pou prepei na ksanaperaso apo ta proigoumena apotelesmata sta kenuria
  int matching_rel,i,j,key,pos,total_counter=0;
  inbetween_results *new_inb_res;
  inbet_node *current;
  inbet_list *previous_res,*new_res;
  int value;

  if(inb_results == NULL) return NULL;

  if(inb_results->inbet_lists[rel1]->joined==-1 && inb_results->inbet_lists[rel2]->joined==-1){ //
    inb_results->inbet_lists[rel1] = result1;
    inb_results->inbet_lists[rel2] = result2;
    inb_results->inbet_lists[rel1]->joined =1;
    inb_results->inbet_lists[rel2]->joined =1;
    return inb_results;
  }else{  //uparxoun apotelesmata kai prepei na kano antistoixisi
    new_inb_res = InitInbetResults(inb_results->num_lists);
    if(inb_results->inbet_lists[rel1]->joined==1){
      previous_res = result1;
    }else if(inb_results->inbet_lists[rel2]->joined==1){
      previous_res = result2;
    }

    for(i=0;i<inb_results->num_lists;i++){
      if(inb_results->inbet_lists[i]->joined==1){   //rel j has previous results
        current = previous_res->head;
        while(current!=NULL){
          for(j=0;j<current->num_tuples;j++){
            value = GetValue(inb_results->inbet_lists[i],current->rowIDS[j]);
            InsertInbetList(new_inb_res->inbet_lists[i],value);
          }
          current=current->next;
        }
        new_inb_res->inbet_lists[i]->joined=1;
      }
    }

    if(inb_results->inbet_lists[rel1]->joined==-1){ //if rel1 has no previous , results are results from join
      printf("%d got joined and has no previous , so it takes its results\n",rel1 );
      new_inb_res->inbet_lists[rel1]->joined =1;
      new_inb_res->inbet_lists[rel1] = result1;
      //free result2
    }
    if(inb_results->inbet_lists[rel2]->joined==-1){
      printf("%d got joined and has no previous , so it takes its results\n",rel2 );
      new_inb_res->inbet_lists[rel2] = result2;
      new_inb_res->inbet_lists[rel2]->joined =1;
      //free result1
    }

    FreeInbetList(inb_results);
    return new_inb_res;
  }
}

inbetween_results *UpdateInbetList2(inbetween_results *inb_results,inbet_list *results,int rel_id){
  inbetween_results *new_inb_res;
  Relation *old_keys,*new_keys;
  inbet_node *current;
  int pos,key;
  int value;
  if(inb_results->inbet_lists[rel_id]->joined==-1){
    inb_results->inbet_lists[rel_id] = results;
    inb_results->inbet_lists[rel_id]->joined=1;
    return inb_results;
  }else{
    new_inb_res = InitInbetResults(inb_results->num_lists);
    for(int i=0;i<inb_results->num_lists;i++){
      printf("%d relation is joined and gets updated\n",i);
      if(inb_results->inbet_lists[i]->joined==1){   //rel j has previous results
        current = results->head;
        while(current!=NULL){
          for(int j=0;j<current->num_tuples;j++){
            value = GetValue(inb_results->inbet_lists[i],current->rowIDS[j]);
            printf("key to be inserted:%d\n",current->rowIDS[j]);
            InsertInbetList(new_inb_res->inbet_lists[i],value);
          }
          current=current->next;
        }
      }
    }
    return new_inb_res;
  }
}

void InsertInbetList(inbet_list *list,int key){
  if(list == NULL) return;
  if(list->head==NULL){     /*list is empty*/
    list->head = InitInbetNode();
    list->current = list->head;
  }else if(list->current->num_tuples==BUFFERSIZE-1){ /*current buffer is full*/
    printf("deutero node\n");
    list->current->next = InitInbetNode();
    list->current = list->current->next;
  }
  list->current->rowIDS[list->current->num_tuples]=key;
  list->current->num_tuples++;
  list->total_tuples++;
}

void PrintInbetList(inbet_list *list){
  inbet_node *current=list->head;
  while (current!=NULL) {
    for(int i=0;i<current->num_tuples;i++){
      printf("%d -",current->rowIDS[i]);
    }
    current=current->next;
  }
}


void FreeInbetList(inbetween_results *list){
  /*frees current list so it gets initialized for new inbetween results */
  inbet_node *current = NULL;
  inbet_node *temp = NULL;
  if(list==NULL) return;
  for(int i=0;i<list->num_lists;i++){
    current = list->inbet_lists[i]->head;
    temp = list->inbet_lists[i]->head;
    while(current!=NULL){
      temp = current;
      current = current->next;
      free(temp);
    }
    free(list->inbet_lists[i]);
  }
  free(list);
}
