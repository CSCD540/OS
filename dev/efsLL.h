/*
 * Author: Jordan Bondo
 * 
 * A lot of the linked list stuff came from http://www.daniweb.com/software-development/c/threads/216353
 */

#include "block.h"
#include <stdio.h>
#include <stdlib.h>


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
  struct blockNode  *firstBlock; // Pointer to the location where the file's first block begins, or the first node in it's blockList
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
void add_block_node(struct blockNode **blockList, struct block *block)
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
}


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
struct fileNode * add_file_node(struct fileNode **fileList, char *filename)
{
  struct fileNode *temp;
  // First file in the list
  if((*fileList)->filename == NULL)
  {
    temp = malloc(sizeof(struct fileNode));
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
    next->filename = filename;
    next->nextFile = NULL;
    temp->nextFile = next;    
  }
  return temp;
}


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
  // Check for empty list
  if((*blockList)->block==NULL)
    return -1;
  
  struct blockNode *temp, *last;
  temp = *blockList;
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
}


struct block * get_block(struct blockNode **blockList, int blockIndex)
{
  struct blockNode *temp;
  temp = *blockList;
  // Check for empty list
  if((*blockList)->block==NULL || temp == NULL)
    return NULL;
  
  int i;
  for(i = 0; i < blockIndex && temp != NULL; i++)
    temp = temp->nextBlock;
  
  return temp->block;
}


/* 
 * void print_block_list(struct blockNode *head)
 * Description:
 *    Print out all the data in all the blocks in this blockList
 * Input:
 *    struct blockNode *head : Pointer to the first node in this blockList
 * Output:
 *    Screen output of the data in the blockList
 */
void print_block_list(struct blockNode *head)
{
  if(head == NULL)
  { printf("\n\nList is empty!\n\n"); }
  else
    while(head != NULL)
    {
      printf("Block #%d:", head->block->blockNum);
      int i;
      for(i = 0; i < BLOCKSIZE; i++)
        printf(" %d", head->block->instructions[i]);
      printf("\n");
      head = head->nextBlock;
    }
}


/* 
 * void print_file_list(struct fileNode *head)
 * Description:
 *    Print out all the file currently stored on the disk
 * Input:
 *    struct fileNode *head : Pointer to the first node in the fileList
 * Output:
 *    Screen output of the files in the fileList
 */
void print_file_list(struct fileNode *head)
{
  if(head == NULL)
  { printf("\n\nList is empty!\n\n"); }
  else
    while(head != NULL)
    {
      printf("%s\n", head->filename);
      head = head->nextFile;
    }
}

