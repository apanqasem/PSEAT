#include<stdlib.h>
#include<stdio.h>

#define DIMSIZE 10000

int main() {
  short **a;
  
  a = (short **) malloc(sizeof(short *) * DIMSIZE);
  
  int i, j;
  for (i = 0; i < DIMSIZE; i++) 
    a[i] = (short *) malloc(sizeof(short) * DIMSIZE);

  for (i = 0; i < DIMSIZE; i++)
    for (j = 0; j < DIMSIZE; j++) 
      a[i][j] = 17;

  printf("%d\n", a[17][17]);

  return 0;

}
