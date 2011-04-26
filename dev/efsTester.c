#include <stdio.h>
#include "efs.h"

block filesystem[NUMBLOCKS];

int main(int argc, char *argv[])
{
  init_filesystem(filesystem);
  
  int i;
  for(i = 0; i < NUMBLOCKS; i++)
  {
    int j;
    printf("Block #%d:", i);
    for(j = 0; j < BLOCKSIZE; j++)
      printf(" %d", filesystem[i].instructions[j]);
    printf("\n");
  }

  return 0;
}

