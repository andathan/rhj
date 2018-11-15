Relation *BuildRelation(stepping_list *head,Relation *init_relation){
  /* input: list of inbetween results , initial Relation
      output : a Relation with inbetween tuples*/
  stepping_stone->stepping_arrays[predicates[i]->rel1],arrays_in_query[predicates[i]->rel1]->columns[col1]
  stepping_list *current = head;
  Relation *new_relation = malloc(sizeof(int)*head->total);
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
