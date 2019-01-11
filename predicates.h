#ifndef _PREDICATES_H__
#define _PREDICATES_H__
#include "rhj.h"
#include "inbetween.h"
#include "trees.h"

typedef struct relation_data relation_data;
typedef struct all_data all_data;
typedef struct inbetween_results inbetween_results;

struct exec_tree{
  int rel;
  int col;
  struct exec_tree *left;
  struct exec_tree *right;
};
typedef struct exec_tree exec_tree;

struct predicates{      /* struct predicates stores info for predicate in query*/
  int rel1;             /* index of 1st relation in predicate*/
  int col1;             /* index of column of 1st relation in predicate*/
  int rel2;             /* index of 2nd relation in predicate*/
  int col2;             /* index of column of 2nd relation in predicate*/
  char op;           /* operation between the 2 columns */
};
typedef struct predicates predicates;

struct query{
  char * sxeseis;
  char * proboles;
  predicates ** katigorimata;
  int num_of_predicates;
  int num_of_relations;
};

typedef struct query query;

struct batch{
    query ** queries_table;
    int num_of_queries;
};

typedef struct batch batch;

batch *InitBatch();
batch *AddToBatch();
void FreeBatch(batch *b);

void seperate_predicate (char * input, char * tokens[3]);
void print_predicates (predicates ** predicates,int num_predicates);
int find_num_of_predicates (char * token);
int make_number (int num_length,char * compute_num, int j);
predicates **fill_predicates (char * token, int num_predicates);
int select_predicate (int , predicates **,relation_data **,int , inbetween_results *);
void execute_query(query *in_query,all_data *data);
predicate * make_predicate (int rel1, int col1, int rel2, int col2, char op);
void restore_statistics (relation_data ** relations, int rel);
float update_statistics(relation_data ** relations, predicates * curr_pred);
#endif
