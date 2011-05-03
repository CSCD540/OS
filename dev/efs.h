#include <stdlib.h>
#include "efsLL.h"

#define DISKSIZE    16*1                  // Total size of the disk
#define NUMBLOCKS   DISKSIZE / BLOCKSIZE  // Total number of blocks on the disk

struct fileNode *fileList = NULL;
struct blockNode *freeBlockList = NULL;


/* 
 * struct blockNode * add_file(char *filename)
 * Description:
 *    add a new file with the specified filename to the fileList
 * Input:
 *    char *filename - the name of the file to add
 * Output:
 *    a pointer to the first block of the file, which will be null.
 *    user must point the block to a free block in the free list
 *    ...don't forget to remove that block from the free list!
 */
struct blockNode * add_file(char *filename)
{
  // Add a new file to the fileList and get a pointer to the first block.  
  struct blockNode * blockNode;
  blockNode = add_file_node(&fileList, filename);
  return blockNode;
}


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


/* 
 * void print_disk(struct block disk[])
 * Description:
 *    print out the contents of the disk
 * Input:
 *    struct block disk[] : The disk whos content's you wish to print
 * Output:
 *    screen output of all data stored in all blocks on the disk
 */
void print_disk(struct block disk[])
{
  int i, j;
  for(i = 0; i < NUMBLOCKS; i++)
  {
    printf("Block #%d", i);
    for(j = 0; j < BLOCKSIZE; j++)
      printf(" %d", disk[i].instructions[j]);
    printf("\n");
  }
}

