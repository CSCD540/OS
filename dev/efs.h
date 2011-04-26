#include "linkedlist.h"

#define DISKSIZE    16*1024               // Total size of the disk
#define BLOCKSIZE   4                     // Number of records per block
#define NUMBLOCKS   DISKSIZE / BLOCKSIZE  // Total number of blocks on the disk





/* 
 * struct block
 * Description: This struct defines a block in the disk.
 * Input: none
 * Output: none
 */
typedef struct {
  int instructions[BLOCKSIZE];
} block;


/* 
 * void init_disk(block disk[])
 * Description: Initialize the passed in disk. Assumes that the disk was created using the constants defined.
 * Input: block disk[] : an array of blocks. Assumes that the array is of size NUMBLOCKS.
 * Output: All entries in block disk[] will be initialized to -1.
 */
void init_disk(block disk[])
{
  int i, j;
  for(i = 0; i < NUMBLOCKS; i++)
    for(j = 0; j < BLOCKSIZE; j++)
      disk[i].instructions[j] = -1;
}



