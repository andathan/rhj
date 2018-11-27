#ifndef _PREDICATES_H__
#define _PREDICATES_H__

struct predicates{      /* struct predicates stores info for predicate in query*/
  int rel1;             /* index of 1st relation in predicate*/
  int col1;             /* index of column of 1st relation in predicate*/
  int rel2;             /* index of 2nd relation in predicate*/
  int col2;             /* index of column of 2nd relation in predicate*/
  char op;           /* operation between the 2 columns */
};
typedef struct predicates predicates;

void seperate_predicate (char * input, char * tokens[3]);
void print_predicates (predicates ** predicates,int num_predicates);
int find_num_of_predicates (char * token);
int make_number (int num_length,char * compute_num, int j);
predicates **fill_predicates (char * token, int num_predicates);

#endif
