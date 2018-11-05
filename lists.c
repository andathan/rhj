#include "rhj.h"

Result *ListInit(){
  Result *node = (Result *)malloc(sizeof(Result));
  node->first=NULL;
  node->current=NULL;
}

void InsertResult(Result *head,int rowID2,int rowID1){
  /*list is empty*/
  if(head->current==NULL){
    head->current = (ResultNode *)malloc(sizeof(ResultNode));
    head->first = head->current ;
    head->first->next = NULL;
    head->first->size=0;
  }else{
    /*if current buffer is full , initialize the next*/
    if(head->current->size == BUFFERSIZE-1){
      head->current->next = (ResultNode *)malloc(sizeof(ResultNode));
      head->current = head->current->next;
      head->current->size=0;
    }
  }
  head->current->buffer[head->current->size][0] = rowID1;
  head->current->buffer[head->current->size][1] = rowID2;
  head->current->size++;
}

void PrintResults(Result *head){
  ResultNode *temp;
  int i,count=1;
  temp = head->first;
  if(head==NULL || temp==NULL){
    printf("There are no matches.\n");
    return ;
  }else{
    printf("A\t|B\t\n");
    printf("-----------\n");
    while(temp!=NULL){
      for(i=0;i<temp->size;i++){
        printf("%d:\t%d\t|%d\t\n",count,temp->buffer[i][0],temp->buffer[i][1]);
        count++;
      }
      temp = temp->next;
    }
  }
}

void deletelist(Result *head)
{
  if(head==NULL) return;
  ResultNode *temp;
  ResultNode *to_free;
  temp = head->first;
  if(temp==NULL){
    free(head);
    return ;
  }else{
    while(temp!=NULL){
      to_free=temp;
      temp = temp->next;
      free(to_free);
    }
    free(head);
    return;
  }
}
