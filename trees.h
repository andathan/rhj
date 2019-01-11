#ifndef _TREES_H__
#define _TREES_H__
#include "rhj.h"
#include "inbetween.h"
#include "predicates.h"

struct tree{
  int rel;
  int col;
  struct tree *left;
  struct tree *right;
};
typedef struct tree tree;

struct exec_information{
  tree * exec_tree;
  int total_cost;
};
typedef struct exec_information exec_information;

tree * new(relation_data ** relations, int r1, int r2, int c1, int c2);
tree * insert (tree * root, int r, int c, relation_data ** relations);
void restore_data(tree * root, relation_data ** relations);
int calculate_total_cost(tree * root);
char execute (tree * root, int * r1, int * r2, int * c1, int * c2);
int connected(predicates ** predicates, int num_predicates, int rel1, int col1, int rel2, int col2);
void printarray(exec_information ** store_information, int * store_info_counter, relation_data ** relations, predicates ** predicates, int num_predicates, int ** arr, int size);
void swap(int ** arr, int a, int b);
void permutation(exec_information ** store_information, int * store_info_counter, relation_data ** relations,predicates ** predicates, int num_predicates, int ** arr, int start, int end);
void find_permutations (exec_information ** store_information, predicates ** predicates, int num_predicates, int ** join_table, int size);
int find_min_cost (exec_information ** store_information);
#endif
