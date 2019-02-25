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
  return list;
}

inbetween_results *InitInbetResults(int n){
  /*initializes the struct of inbtween results*/
  int i;
  inbetween_results *results = malloc(sizeof(inbetween_results));
  results->num_lists = n;
  results->inbetween=(int **)malloc(sizeof(int *)*n);
  results->joined=(int *)malloc(sizeof(int)*n);
  results->joined_pairs=malloc(sizeof(int *)*n);
  for(i=0;i<n;i++){
    results->joined[i]=-1;
    results->joined_pairs[i]=malloc(sizeof(int)*n);
    for(int j=0;j<n;j++) results->joined_pairs[i][j]=-1;
  }

  return results;
}


inbetween_results *UpdateInbetList(inbetween_results *inb_results,inbet_list *result1,inbet_list *result2,int rel1,int rel2){
  /*ananewnei tin endiamesi domi me ta kainouria apotelesmata*/
  int matching_rel,i,j,key,pos,total_counter=0;
  inbetween_results *new_inb_res;
  inbet_node *current;
  inbet_list *previous_res,*other_res;
  int value;

  if(inb_results == NULL) return NULL;

  if(inb_results->joined[rel1]==-1 && inb_results->joined[rel2]==-1){ //both relations have no previous results
    inb_results->inbetween[rel1] = malloc(sizeof(int)*result1->total_tuples);
    inb_results->joined[rel1] = result1->total_tuples;
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

    inb_results->joined[rel2] = result2->total_tuples;
    inb_results->inbetween[rel2] = malloc(sizeof(int)*result2->total_tuples);
    current = result2->head;
    while(current!=NULL){
      for(i=0;i<current->num_tuples;i++){
        inb_results->inbetween[rel2][total_counter] = current->rowIDS[i];
        total_counter++;
      }
      current=current->next;
    }
    inb_results->joined_pairs[rel1][rel2]=1;
    inb_results->joined_pairs[rel2][rel1]=1;
    return inb_results;
  }else{  //uparxoun apotelesmata kai prepei na kano antistoixisi
    int other_rel;
    new_inb_res = InitInbetResults(inb_results->num_lists);
    for(i=0;i<inb_results->num_lists;i++){
      for(int j=0;j<inb_results->num_lists;j++)
        new_inb_res->joined_pairs[i][j]=inb_results->joined_pairs[i][j];
    }
    if(inb_results->joined[rel1]!=-1){
      previous_res = result1;
      other_rel = rel2;
      other_res = result2;
    }else if(inb_results->joined[rel2]!=-1){
      previous_res = result2;
      other_rel = rel1;
      other_res = result1;
    }
    //malloc for the relations which have previous results
    for(i=0;i<inb_results->num_lists;i++){
      if(inb_results->joined[i]!=-1){   //rel j has previous results
        new_inb_res->inbetween[i]=malloc(sizeof(int)*previous_res->total_tuples);
      }
    }
    current = previous_res->head;
    total_counter=0;
    while(current!=NULL){
      for(j=0;j<current->num_tuples;j++){
        for(i=0;i<inb_results->num_lists;i++){
          if(inb_results->joined[i]!=-1){   //rel j has previous results
            new_inb_res->inbetween[i][total_counter]= inb_results->inbetween[i][current->rowIDS[j]];
          }
        }
        total_counter++;
      }
      current=current->next;
    }
    for(i=0;i<inb_results->num_lists;i++){
      if(inb_results->joined[i]!=-1){   //rel j has previous results
        new_inb_res->joined[i]=previous_res->total_tuples;
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
    new_inb_res->joined_pairs[rel1][rel2]=1;
    new_inb_res->joined_pairs[rel2][rel1]=1;
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
      }
    }

    current = results->head;
    total_counter=0;
    while(current!=NULL){
      for(int j=0;j<current->num_tuples;j++){
        for(int i=0;i<inb_results->num_lists;i++){
          if(inb_results->joined[i]!=-1){   //rel j has previous results
            new_inb_res->inbetween[i][total_counter] = inb_results->inbetween[i][current->rowIDS[j]];
          }
        }
        total_counter++;
      }
      current=current->next;
    }
    for(int i=0;i<inb_results->num_lists;i++){
      if(inb_results->joined[i]!=-1){   //rel j has previous results
        new_inb_res->joined[i]=results->total_tuples;
      }
    }
  }

  return new_inb_res;

}

void InsertInbetList(inbet_list *list,int key){
  inbet_node *curr=NULL;
  if(list == NULL) return;
  if(list->head==NULL){     /*list is empty*/
    curr = InitInbetNode();
    list->head = curr;
    list->current = curr;
  }else if(list->current->num_tuples==BUFFERSIZE-1){ /*current buffer is full*/
    curr = InitInbetNode();
    list->current->next = curr;
    list->current = curr;
  }else{
    curr = list->current;
  }
  curr->rowIDS[curr->num_tuples]=key;
  curr->num_tuples++;
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

void FreeResults(inbet_list *res){
  if(res==NULL) return;
  inbet_node *curr = res->head;
  inbet_node *next = NULL;
  while(curr!=NULL){
    next = curr->next;
    free(curr);
    curr = next;
  }
  free(res);
}

void FreeInbetList(inbetween_results *list){
  /*frees current list so it gets initialized for new inbetween results */
  if(list==NULL) return;
  for(int i=0;i<list->num_lists;i++){
    if(list->joined[i]!=-1){
      free(list->inbetween[i]);
    }
  }
  free(list->joined);
  free(list->inbetween);
  free(list);
}
