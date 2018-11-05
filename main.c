#include "rhj.h"
#include "lists.h"

int main (void)
{
  Relation *R;
  Relation *S;
  Result *results;
  R = fill_table(2);
  S = fill_table(1);
  results = RadixHashJoin(R,S);
  PrintResults(results);
  deletelist(results);
  free(R->tuples);
  free(R);
  free(S->tuples);
  free(S);
}
