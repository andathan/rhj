#include "rhj.h"

int main (void)
{
  Relation *R;
  Relation *S;
  Result *results;
  R = fill_table(3);
  S = fill_table(4);
  results = RadixHashJoin(R,S);
  PrintResults(results);
  deletelist(results);
  free(R);
  free(S);
}
