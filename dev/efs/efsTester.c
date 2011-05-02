#include <stdio.h>
#include "efs.h"

struct block disk[NUMBLOCKS];

int main(int argc, char *argv[])
{
  init_disk(disk);
  
  print_block_list(freeBlockList);
  
  int status = 0;
  status = add_file("milk1.out");
  
  if(status == -1)
    printf("File open error");
  else
    printf("Status: %d\n", status);
  
  print_file_list(fileList);

  return 0;
}

