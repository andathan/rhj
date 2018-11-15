struct relation_data{
  uint64_t numColumns
  uint64_t numTuples
  Relation **columns;     //pinakas apo deiktes stin arxi kathe column
};
typedef struct relation_data relation_data;

struct all_data{
  relation_data ** table;  //array of pointers to relation_data
  int num_relations;       //number of total relations
};
typedef struct all_data all_data;

struct predicates{      /* struct predicates stores info for predicate in query*/
  int rel1;             /* index of 1st relation in predicate*/
  int col1;             /* index of column of 1st relation in predicate*/
  int rel2;             /* index of 2nd relation in predicate*/
  int col2;             /* index of column of 2nd relation in predicate*/
  char[3] op;           /* operation between the 2 columns */
}
typedef struct predicates predicates;

struct inbet_node{           /*struct to store inbetween relations*/
  int rowIDS[BUFFERSIZE]        /*array of rowIDS*/
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
  inbet_list **inbetween_lists; /*array of pointers to lists*/
  int num_lists;                  /*number of lists*/
}

