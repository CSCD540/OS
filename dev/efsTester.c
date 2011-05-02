#include <stdio.h>
#include "efs.h"

struct block disk[NUMBLOCKS];

int main(int argc, char *argv[])
{
  init_disk(disk);
  
  print_block_list(freeBlockList);

  return 0;
}

