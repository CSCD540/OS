#include <stdio.h>
#include "efs.h"

block disk[NUMBLOCKS];

int main(int argc, char *argv[])
{
  init_disk(disk);
  
  int i;
  for(i = 0; i < NUMBLOCKS; i++)
  {
    int j;
    printf("Block #%d:", i);
    for(j = 0; j < BLOCKSIZE; j++)
      printf(" %d", disk[i].instructions[j]);
    printf("\n");
  }

  return 0;
}
