#include <stdlib.h>
#include "efsLL.h"

#define DISKSIZE    16*1                  // Total size of the disk
#define NUMBLOCKS   DISKSIZE / BLOCKSIZE  // Total number of blocks on the disk

struct fileNode *fileList = NULL;
struct blockNode *freeBlockList = NULL;

/* 
 * void init_disk(block disk[])
 * Description:
 *    Initialize the passed in disk. Assumes that the disk was created using the constants defined.
 * Input:
 *    block disk[] : an array of blocks. Assumes that the array is of size NUMBLOCKS.
 * Output:
 *    All entries in block disk[] will be initialized to -1.
 */
void init_disk(struct block disk[])
{
  // Allocate memory for the head node in freeBlockList
  freeBlockList = malloc(sizeof(struct blockNode));
  // Initialize all blocks in the disk and add all blocks to freeBlockList
  int i, j;
  for(i = 0; i < NUMBLOCKS; i++)
  {
    for(j = 0; j < BLOCKSIZE; j++)
      disk[i].instructions[j] = -1;
    disk[i].blockNum = i;
    add_block_node(&freeBlockList, &disk[i]);
  }
  
  // Allocate memory for the head node in fileList
  fileList = malloc(sizeof(struct fileNode));
}



int add_file(char *filename)
{
  FILE *fd;
  fd = fopen(filename, "r");
  if(fd == NULL)
    return -1;
  
  // Add a new file to the fileList and get a pointer to the first block.  
  struct blockNode * blockNode;
  blockNode = add_file_node(&fileList, filename);
  
  // TODO : Get head from freeBlockList, free and remove the first node in the freeBlockList
  
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
  printf("NumBlocks: %d\n", numBlocks);
  return fclose(fd);
}

