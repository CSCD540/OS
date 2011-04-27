#include <stdlib.h>
#include "efsLL.h"

#define DISKSIZE    16*1                  // Total size of the disk
#define NUMBLOCKS   DISKSIZE / BLOCKSIZE  // Total number of blocks on the disk

struct fileNode *fileList = NULL;
struct blockNode *freeBlockList = NULL;

/* 
 * void init_disk(block disk[])
 * Description: Initialize the passed in disk. Assumes that the disk was created using the constants defined.
 * Input: block disk[] : an array of blocks. Assumes that the array is of size NUMBLOCKS.
 * Output: All entries in block disk[] will be initialized to -1.
 */
void init_disk(struct block disk[])
{
  int i, j;
  freeBlockList = malloc(sizeof(struct blockNode));

  for(i = 0; i < NUMBLOCKS; i++)
  {
    for(j = 0; j < BLOCKSIZE; j++)
      disk[i].instructions[j] = -1;
    disk[i].blockNum = i;
    add_block(&freeBlockList, &disk[i]);
  }
}




