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
}

inbetween_results *InitInbetResults(int n){
  /*initializes the struct of inbtween results*/
  int i;
  inbetween_results *results = malloc(sizeof(inbetween_results));
  results->num_lists = n;
  results->inbetween=(int **)malloc(sizeof(int *)*n);
  results->joined=(int *)malloc(sizeof(int)*n);
  for(i=0;i<n;i++){
    results->joined[i]=-1;
  }
  return results;
}



inbetween_results *UpdateInbetList(inbetween_results *inb_results,inbet_list *result1,inbet_list *result2,int rel1,int rel2){
  /*ta row_results stin proti stili periexoun tis theseis twn keys sta results pou prepei na mpoun sta kenuria results*/
  //brisko poia prepei na kratiso apo ta proigoumena,sta row_results h proti stili tha mu deixnei tis
  //grammes pou prepei na ksanaperaso apo ta proigoumena apotelesmata sta kenuria
  int matching_rel,i,j,key,pos,total_counter=0;
  inbetween_results *new_inb_res;
  inbet_node *current;
  inbet_list *previous_res,*other_res;
  int value;

  if(inb_results == NULL) return NULL;

  if(inb_results->joined[rel1]==-1 && inb_results->joined[rel2]==-1){ //
    inb_results->inbetween[rel1] = malloc(sizeof(int)*result1->total_tuples);
    inb_results->inbetween[rel2] = malloc(sizeof(int)*result2->total_tuples);
    inb_results->joined[rel1] = result1->total_tuples;
    inb_results->joined[rel2] = result2->total_tuples;
    total_counter=0;
    current = result1->head;
    //store every key
    while(current!=NULL){
      for(i=0;i<current->num_tuples;i++){
        inb_results->inbetween[rel1][total_counter] = current->rowIDS[i];
        total_counter++;
      }
      current=current->next;
    }
    total_counter=0;
    current = result2->head;
    while(current!=NULL){
      for(i=0;i<current->num_tuples;i++){
        inb_results->inbetween[rel2][total_counter] = current->rowIDS[i];
        total_counter++;
      }
      current=current->next;
    }
    return inb_results;
  }else{  //uparxoun apotelesmata kai prepei na kano antistoixisi
    int other_rel;
    new_inb_res = InitInbetResults(inb_results->num_lists);
    if(inb_results->joined[rel1]!=-1){
      previous_res = result1;
      other_rel = rel2;
      other_res = result2;
    }else if(inb_results->joined[rel2]!=-1){
      previous_res = result2;
      other_rel = rel1;
      other_res = result1;
    }

    for(i=0;i<inb_results->num_lists;i++){
      if(inb_results->joined[i]!=-1 && i!=other_rel){   //rel j has previous results
        new_inb_res->inbetween[i]=malloc(sizeof(int)*previous_res->total_tuples);
        current = previous_res->head;
        total_counter=0;
        while(current!=NULL){
          for(j=0;j<current->num_tuples;j++){
            new_inb_res->inbetween[i][total_counter]= inb_results->inbetween[i][current->rowIDS[j]];
            total_counter++;
          }
          current=current->next;
        }
        new_inb_res->joined[i]=total_counter;
      }else if(other_rel==i && inb_results->joined[other_rel]!=-1){
        new_inb_res->inbetween[i]=malloc(sizeof(int)*other_res->total_tuples);
        current = other_res->head;
        total_counter=0;
        while(current!=NULL){
          for(j=0;j<current->num_tuples;j++){
            new_inb_res->inbetween[i][total_counter]=inb_results->inbetween[i][current->rowIDS[j]];
            total_counter++;
          }
          current=current->next;
        }
        new_inb_res->joined[i]=total_counter;
      }
    }

    if(inb_results->joined[rel1]==-1){ //if rel1 has no previous , results are results from join
      new_inb_res->inbetween[rel1]=malloc(sizeof(int)*result1->total_tuples);
      current = result1->head;
      total_counter=0;
      while(current!=NULL){
        for(i=0;i<current->num_tuples;i++){
          new_inb_res->inbetween[rel1][total_counter] = current->rowIDS[i];
          total_counter++;
        }
        current=current->next;
      }
      new_inb_res->joined[rel1]= total_counter;
    }
    if(inb_results->joined[rel2]==-1){
      new_inb_res->inbetween[rel2]=malloc(sizeof(int)*result2->total_tuples);
      current = result2->head;
      total_counter=0;
      while(current!=NULL){
        for(i=0;i<current->num_tuples;i++){
          new_inb_res->inbetween[rel2][total_counter] = current->rowIDS[i];
          total_counter++;
        }
        current=current->next;
      }
      new_inb_res->joined[rel2]= total_counter;
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
  int total_counter=0;
  if(inb_results->joined[rel_id]==-1){
    inb_results->inbetween[rel_id] = malloc(sizeof(int)*results->total_tuples);
    total_counter=0;
    current = results->head;
    while(current!=NULL){
      for(int i=0;i<current->num_tuples;i++){
        inb_results->inbetween[rel_id][total_counter] = current->rowIDS[i];
        total_counter++;
      }
      current=current->next;
    }
    inb_results->joined[rel_id]= total_counter;
    return inb_results;
  }else{
    new_inb_res = InitInbetResults(inb_results->num_lists);
    for(int i=0;i<inb_results->num_lists;i++){
      if(inb_results->joined[i]!=-1){   //rel j has previous results
        new_inb_res->inbetween[i] = malloc(sizeof(int)*results->total_tuples);
        current = results->head;
        while(current!=NULL){
          for(int j=0;j<current->num_tuples;j++){
            new_inb_res->inbetween[i][total_counter] = inb_results->inbetween[i][current->rowIDS[j]];
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
    }
    current=current->next;
  }
}


void FreeInbetList(inbetween_results *list){
  /*frees current list so it gets initialized for new inbetween results */

}
