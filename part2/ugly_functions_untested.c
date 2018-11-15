Relation *BuildRelation(stepping_list *head,Relation *init_relation){
  /* input: list of inbetween results , initial Relation
      output : a Relation with inbetween tuples*/
  stepping_stone->stepping_arrays[predicates[i]->rel1],arrays_in_query[predicates[i]->rel1]->columns[col1]
  stepping_list *current = head;
  Relation *new_relation = (Relation *)malloc(sizeof(Relation));
  new_relation->tuples = malloc(sizeof(tuple)*head->total);
  new_relation->num_tuples = head->total;
  int i,c=0;
  while(current!=NULL){ //for every node in list
    for(i=0;i<stepping_list->counter;i++){  //for every rowID in node
      new_relation[c].key = stepping_list->rowIDS[i];
      new_relation[c].payload = init_relation[new_relation[c].key].payload;
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
