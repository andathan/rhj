
int getnextprime(int num)
{
    char flag;
    for (int i = num;; ++i)
    {
      flag=0;
      for (int j=2;j<sqrt(num);j++)//psaxno mexri tin riza tou arithmou, afou meta tin riza apoklietai na exei diairetes pou den exoume idi tsekarei
      {
        if (i%j==0)
        flag=1;//brika kapoion arithmo pou na diairetai
        break;//ara den einai Prime
      }
      if(flag==0)//den exw brei kanenan arithmo pou na diaireitai
        {
        printf("Next Prime of %d is %d.\n",num,i);//ara einai prime
        return i;
      }
    }
}
int H2(int a) //Thomas Wang integer hash method
{
  a = (a ^ 61) ^ (a >> 16);
  a = a + (a << 3);
  a = a ^ (a >> 4);
  a = a * 0x27d4eb2d;
  a = a ^ (a >> 15);
  return a;
}
