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
  struct blockNode *next;  // Pointer to the next blockNode in the list.
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
 * int add_file(fileNode **fileList, char *filename)
 * Description:
 *    Add a new file to the disk
 * Input:
 *    fileNode **fileList : Pointer to the current list of files
 *    char *filename : Name of the file to be added
 * Output:
 *    New file will be stored on the disk. fileList, blockList and freeBlockList will be updated.
 */
int add_file(struct fileNode **fileList, char *filename)
{
  struct fileNode *temp;
  // First file in the list
  if((*fileList)->firstBlock == NULL)
  {
    temp = (struct fileNode *) malloc(sizeof(struct fileNode));
    temp->filename = filename;
  }
  
  // TODO: Add the node to the end of the list
  
  return 0;
}


/* 
 * void add_block(struct blockNode **blockList, struct block *block)
 * Description:
 *    Add a new block to the list
 * Input:
 *    blockNode *blockList : Pointer to the first node in the blockList.
 *    block *block : Pointer to a block on the disk.
 * Output:
 */
void add_block(struct blockNode **blockList, struct block *block)
{
  struct blockNode *temp;
  // List is empty. Add first.
  if((*blockList)->block == NULL)
  {
    temp = (struct blockNode *)malloc(sizeof(struct blockNode));
    temp->block = block;
    temp->next = NULL;
    *blockList = temp;
    // printf("blockNode %p\n", *blockList);
  }
  // List is NOT empty. Add last.
  else
  {
    struct blockNode *next;
    temp = *blockList;
    
    // Advance to the last node in the list
    while(temp->next != NULL)
    { temp = temp->next; }
    
    next = (struct blockNode *)malloc(sizeof(struct blockNode));
    next->block = block;
    next->next = NULL;
    temp->next = next;
  }
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
      head = head->next;
    }
}

