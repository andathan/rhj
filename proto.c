struct tuple {
int32_t key;    //rowID
Int32_t payload;   //value
};

struct relation {
  tuple  *tuples;   //pinakas apo tuples
  uint32_t num_tuples;  //megethos pinaka
};

struct list{
  result *first;
  result *current;
}
struct result {
 int32_t buffer[1024*1024/64][2];
 int32_t size;
 result *next;
};
/** Radix Hash Join**/
result* RadixHashJoin(relation *relR, relation *relS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
n = arithmos bits pou tha paro
histogram , pinakas apo integers midenismenos
result *list_of_results;
exw 2 relation : A ,B
A_new = tmimatopoihsh(A)  //na girisoume kai ta offsets,histogram
B_new = tmimatopoihsh(B)  //na girisoume kai ta offsets,histogram

gia kathe bucket tou A Kai B:
  chosen_bucket =epilego to bucket pou exei histogram_A[i]<histogram_B[i]  ,diladi ligotera stoixeia
  
  gia auto to bucket: //esto oti exoume parei A'
    euros_hashfunction2=1os prime arithmo meta to histogram[i] //diladi to megethos tou chosen_bucket
    desmeuo kai enan chain me megethos histogram[i] arxikoihmeno me -1
    kai enan pinaka hashtable me megethos=euros_hashfunction2 arxikoihmeno me -1
    gia kathe tuple tou chosen_bucket:
      thesi_ston_hashtable = H2(tuple->value) % euros_hashfunction2
      previous = hashtable[thesi_ston_hashtable]
      new = tuple->key
      hashtable[thesi_ston_hashtable] = new;
      if(previous!=-1)
        chain[new] = previous;
    //edo exoume ftiaksei to euretirio
    //ara ksekiname tin sigkrisi
    gia to allo bucket:     //esto oti eimaste sto B'
      gia kathe tuple tou bucket: -- i
        thesi_ston_hashtable = H2(tuple->value)%euros_hashfunction2
        rowID = hashtable[thesi_ston_hashtable]
        while(rowID!=-1){
          if(A_new[rowID]->value == B_new[i]_value)
            eisagogi_sta_results
          rowID = chain[rowID]
        }

tmimatopoihsh{
  gia kathe tuple tou pinaka:
  index_bucket = H1(A[i])   //index_bucket = se pio bucket tha mpei auto to tuple
  histogram[index_bucket]++;
  ftiaxno ta offsets aka psum
  gia kathe tuple tou pinaka:
  index_bucket=H1(A[i])
  A_new[offsets[index_bucket]]=tuple // rowID,value
  offsets[index_bucket]++;
}

eisagogi_sta_results{
  current = deiktis se kombo pou eimai
  if(current->size==1024*1024/64)
      ftiakse kainourio kombo
      enose tous
  buffer[size]=tuple;





}
