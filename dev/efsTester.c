#include <stdio.h>
#include "efs.h"

int save_file(char *filename);

struct block disk[NUMBLOCKS];

int main(int argc, char *argv[])
{
  init_disk(disk);
  
  int status = save_file("milk1.out");
  
  printf("\nFilelist after adding \"milk1.out\":\n");
  print_file_list(fileList);
  
  status = delete_block_node(&freeBlockList, freeBlockList->block);
  printf("\nfreeBlockList after deleting nodes used for file:\n");
  print_block_list(freeBlockList);
  
  
  printf("\nDisk after adding file:\n");
  print_disk(disk);

  printf("\n");
  return 0;
}


/* 
 * Save a file to the disk
 */
int save_file(char *filename)
{
  FILE *fd;
  fd = fopen(filename, "r");
  if(fd == NULL)
    return -1;
  
  struct blockNode *blockNode;
  blockNode = add_file(filename);
    
  int numBlocks = 1; // Start at one block because this is the first block.
  int instNum = 0; // Basically the line number in the file
  int instruction = 0; // What the line says
  while(fscanf(fd, "%d\n", &instruction) != EOF)
  {
    // TODO: save 4 instructions to current block
    printf("Inst# %3d: %d\n", instNum, instruction);
    instNum++;
    // Check if the current block is full
    if(instNum % BLOCKSIZE == 0)
    {
      // TODO: Get the next free block, free and remove the first node in the free block list
      numBlocks++;
    }
  }
  return fclose(fd);
}

