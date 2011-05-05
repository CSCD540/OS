/*
 * Author: Jordan Bondo
 * 
 * A lot of this stuff came from http://www.daniweb.com/software-development/c/threads/216353
 */

#include "block.h"
#include <stdio.h>
#include <stdlib.h>

/* Definitions */
#define LIST_EMPTY 0
/* End Definitions */

/* Forward Declarations */
void print_error(int errno);
/* End Forward Declarations */

/* 
 * struct blockNode
 * Description:
 *    This struct defines a blockNode in the list.
 * Input:
 *    none
 * Output:
 *    none
 */
struct blockNode {
  struct block     *block; // Pointer to a block on the disk
  struct blockNode *nextBlock;  // Pointer to the next blockNode in the list.
};


/* 
 * struct fileNode
 * Description:
 *    This struct defines a fileNode in the list.
 * Input:
 *    none
 * Output:
 *    none
 */
struct fileNode {
  char   *filename; // This file's name
  int    numBlocks; // The number of block this file occupies
  struct blockNode  *blockList; // Pointer to the location where the file's first block begins, or the first node in it's blockList
  struct fileNode   *nextFile;   // Pointer to the next file in the file list
};


/* 
 * void add_block_node(struct blockNode **blockList, struct block *block)
 * Description:
 *    Add a new block to the list
 * Input:
 *    blockNode *blockList : Pointer to the first node in the blockList.
 *    block *block : Pointer to a block on the disk.
 * Output:
 */
struct blockNode * add_block_node(struct blockNode **blockList, struct block *block)
{
  struct blockNode *temp;
  // List is empty. Add first.
  if((*blockList)->block == NULL)
  {
    temp = malloc(sizeof(struct blockNode));
    temp->block = block;
    temp->nextBlock = NULL;
    *blockList = temp;
    // printf("blockNode %p\n", *blockList);
  }
  // List is NOT empty. Add last.
  else
  {
    struct blockNode *next;
    temp = *blockList;
    
    // Advance to the last node in the list
    while(temp->nextBlock != NULL)
    { temp = temp->nextBlock; }
    
    next = malloc(sizeof(struct blockNode));
    next->block = block;
    next->nextBlock = NULL;
    temp->nextBlock = next;
  }
  return temp;
} // end add_block_node()


/* 
 * struct fileNode * add_file_node(fileNode **fileList, char *filename)
 * Description:
 *    Add a new file to the disk
 * Input:
 *    fileNode **fileList : Pointer to the current list of files
 *    char *filename : Name of the file to be added
 * Output:
 *    New file will be stored on the disk. fileList, blockList and freeBlockList will be updated.
 */
struct fileNode * add_file_node(struct fileNode **fileList, char *filename, int numBlocks)
{
  struct fileNode *temp;
  // First file in the list
  if((*fileList)->filename == NULL)
  {
    temp = malloc(sizeof(struct fileNode));
    temp->numBlocks = numBlocks;
    temp->filename = filename;
    temp->nextFile = NULL;
    *fileList = temp;
    // printf("fileNode %p\n", *fileList);
  }
  else
  {
    struct fileNode *next;
    temp = *fileList;
    // Advance to the last node in the lsit
    while(temp->nextFile != NULL)
    { temp = temp->nextFile; }
    
    next = malloc(sizeof(struct fileNode));
    next->numBlocks = numBlocks;
    next->filename = filename;
    next->nextFile = NULL;
    temp->nextFile = next;    
  }
  return temp;
} // end add_file_node()


/* 
 * int delete_block_node(struct blockNode **blockList, struct block *block)
 * Description:
 *    Delete the specified block from the list
 * Input:
 *    struct blockNode **blockList : pointer to the head node of the blockList
 *    struct block *block : the block to be deleted
 * Output:
 *    -1 : blockList is empty
 *     0 : block was found and the node was successfully removed from the list
 *     1 : block was not found. No changes made to the list
 */
int delete_block_node(struct blockNode **blockList, struct block *block)
{
  struct blockNode *temp, *last;
  temp = *blockList;
  // Check for empty list
  if(temp == NULL)
  {
    print_error(LIST_EMPTY);
    return -1;
  }
    
  while(temp != NULL)
  {
    if(temp->block == block)
    {
      // Initialize the block
      int i;
      for(i = 0; i < BLOCKSIZE; i++)
        block->instructions[i] = -1;
      
      // First node in the list?
      if(temp == *blockList)
        *blockList = temp->nextBlock;
      else
        last->nextBlock = temp->nextBlock;
      free(temp);
      return 0;
    }
    last = temp;
    temp = temp->nextBlock;
  }
  return 1;
} // end delete_block_node()


/* 
 * struct block * get_block(struct blockNode **blockList, int blockIndex)
 * Description:
 *    Get a block in the blockList at the specified index
 * Input:
 *    struct blockNode **blockList : Pointer to the first node in this blockList
 *    int blockIndex : The index number of the desired block
 * Output:
 *    NULL : The list is empty
 *    temp->block : The block at the location specified by blockIndex.
 */
struct block * get_block(struct blockNode **blockList, int blockIndex)
{
  struct blockNode *temp;
  temp = *blockList;
  // Check for empty list
  if(temp == NULL)
  {
    print_error(LIST_EMPTY);
    return NULL;
  }
  
  int i;
  for(i = 0; i < blockIndex && temp != NULL; i++)
    temp = temp->nextBlock;
  
  return temp->block;
}


/* 
 * int get_block_count(struct blockNode *blockList)
 * Description:
 *    Count the number of nodes in the list
 * Input:
 *    struct blockNode *blockList : Pointer to the first node in this blockList
 * Output:
 *    0 : The list is empty
 *    i : The number of nodes in this list
 */
int get_block_count(struct blockNode *blockList)
{
  int i;
  if(blockList == NULL)
  { 
    print_error(LIST_EMPTY);
    return 0;
  }
  else
  {
    i = 0;
    while(blockList != NULL)
    {
      i++;
      blockList = blockList->nextBlock;
    }
  }
  return i;
}


/* 
 * void print_error(int errno)
 * Description:
 *    Print out the error message associated with the errno
 * Input:
 *    int errno : The error number
 * Output:
 *    Screen output of what error occurred.
 */
void print_error(int errno)
{
  printf("\nERROR: ");
  switch(errno)
  {
    case LIST_EMPTY:
      printf("List is empty!\n");
      break;
      
    default:
      printf("An unspecified has error occurred.\n");
  }
}

