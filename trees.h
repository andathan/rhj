#ifndef _TREES_H__
#define _TREES_H__
#include "rhj.h"
#include <stdint.h>
struct tree{
  int rel;
  int col;
  struct tree *l;
  struct tree *r;
};
typedef struct tree tree;

struct exec_information{
  tree * exec_tree;
  int total_cost;
};
typedef struct exec_information exec_information;

tree * new(relation_data ** relations, int r1, int r2, int c1, int c2);
tree * insert(relation_data ** relations, tree * root, int r, int c);
void restore_data(tree * root, relation_data ** relations);
int calculate_total_cost(tree * root);
int execute (tree * root, predicates **, int);
int connected(predicates ** predicates, int num_predicates, int rel1, int col1, int rel2, int col2);
void permutation_to_trees(exec_information ** store_information, int * store_info_counter, relation_data ** relations, predicates ** predicates, int num_predicates, int ** arr, int size);
void swap(int ** arr, int a, int b);
void permutation(exec_information ** store_information, int * store_info_counter, relation_data ** relations,predicates ** predicates, int num_predicates, int ** arr, int start, int end);
int * find_permutations (int, int *, relation_data ** relations, predicates ** predicates, int num_predicates, int ** join_table);
int find_min_cost (exec_information ** store_information);
#endif
