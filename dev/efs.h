#include "linkedlist.h"

#define DISKSIZE    16*1024               // Total size of the disk
#define BLOCKSIZE   4                     // Number of records per block
#define NUMBLOCKS   DISKSIZE / BLOCKSIZE  // Total number of blocks on the disk





/* struct block
 * Description: This struct defines a block in the filesystem.
 * Input: none
 * Output: none
 */
typedef struct {
  int instructions[BLOCKSIZE];
} block;


/* 
 * void init_filesystem(block filesystem[])
 * Description: Initialize the passed in filesystem. Assumes that the filesystem was created using the constants defined.
 * Input: block filesystem[] : an array of blocks. Assumes that the array is of size NUMBLOCKS.
 * Output: All entries in block filesystem[] will be initialized to -1.
 */
void init_filesystem(block filesystem[])
{
  int i, j;
  for(i = 0; i < NUMBLOCKS; i++)
    for(j = 0; j < BLOCKSIZE; j++)
      filesystem[i].instructions[j] = -1;
}



