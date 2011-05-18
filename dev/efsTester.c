#include <stdio.h>
#include "efs.h"


//int save_file(char *filename);
//int write(struct fileNode **fileListNode, int data[], int count, int writeMode);

//struct block disk[NUMBLOCKS];

int main(int argc, char *argv[])
{
  printf("\nfreeBlockList before initialization:\n");
  print_block_list(freeBlockList);
  printf("\nfile before initialization:\n");
  print_file_list(fileList);
  
  printf("\nInitializing Disk...\n");
  init_disk(disk);
  
  printf("\nfreeBlockList after initialization:\n");
  print_block_list(freeBlockList);
  printf("\nfile after initialization:\n");
  print_file_list(fileList);
  
  int status;
  
/*  int i;*/
/*  for(i = 0; i < 4; i++)*/
/*  {*/
    printf("\nSaving file...\n");
    status = save_file("testFile.out");
    struct fileNode * file = get_file("testFile.out");
    
    if(status == 0 && file != NULL)
    {
      printf("File %s found\n", file->filename);
      printf("\nFilelist after adding \"testFile.out\":\n");
      print_file_list(fileList);
      printf("\nDisk after adding file:\n");
      print_disk(disk);
      printf("\nfreeBlockList after deleting nodes used for file:\n");
      print_block_list(freeBlockList);
      printf("\n%s blockList:\n", file->filename);
      print_block_list(file->blockList);
    }
    else
      printf("\nFile not saved successfully. Status returned %d. \n", status);
/*  }*/
  printf("\n");
  return 0;
}




