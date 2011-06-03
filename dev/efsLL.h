/*
 * Author: Jordan Bondo
 * 
 * A lot of this stuff came from http://www.daniweb.com/software-development/c/threads/216353
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _GLOBALS_H_
#include "globals.h"
#endif
#ifndef _HELPERS_H_
#include "helpers.h"
#endif

/* Forward Declarations */
struct blockNode * add_block_node(struct blockNode **blockList, struct block *block);
struct fileNode * add_file_node(struct fileNode **fileList, char *filename, int numBlocks);
int delete_block_node(struct blockNode **blockList, struct block *block);
struct fileNode * find_file(struct fileNode **fileList, char *filename);
struct blockNode * get_block_node(struct blockNode **blockList, int blockIndex);
int get_block_count(struct blockNode *blockList);
void print_error(int errno);
/* End Forward Declarations */


/* void add_block_node(struct blockNode **blockList, struct block *block)
 * Description:
 *    Add a new block to the list
 * Input:
 *    blockNode *blockList : Pointer to the first node in the blockList.
 *    block *block : Pointer to a block on the disk.
 * Output:
 */
struct blockNode * add_block_node(struct blockNode **blockList, struct block *block)
{
  if(DEBUG) printf("add_block_node:in-block: %p\n", block);
  struct blockNode *temp;
  // List is empty. Add first.
  if((*blockList)->block == NULL)
  {
    temp = malloc(sizeof(struct blockNode));
    temp->block = block;
    if(DEBUG) printf("add_block->new list %p\n", temp->block);
    temp->nextBlock = NULL;
    *blockList = temp;
     return *blockList;
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
    if(DEBUG) printf("add_block->old list %p\n", next->block);
    next->nextBlock = NULL;
    temp->nextBlock = next;
    return next;
  }
} // end add_block_node()


/* struct fileNode * add_file_node(fileNode **fileList, char *filename)
 * Description:
 *    Add a file to the disk. This is an in-ordered add, so all files are added in lexicographical order.
 * Input:
 *    fileNode **fileList : Pointer to the current list of files
 *    char *filename : Name of the file to be added
 *    int numBlocks : The number of blocks this file occupies
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
    temp->filename = malloc(strlen(filename) + 1);
    strcpy(temp->filename, filename);
    temp->nextFile = NULL;
    *fileList = temp;
    if(DEBUG) printf("fileNode %p\n", *fileList);
  }
  else
  {
    struct fileNode *next;
    temp = *fileList;
    next = malloc(sizeof(struct fileNode));
    next->numBlocks = numBlocks;
    next->filename = malloc(strlen(filename) + 1);
    strcpy(next->filename, filename);    
     
    // Add first?
    if(strcmp(filename, temp->filename) < 0)
    {
      next->nextFile = temp;
      *fileList = next;
      return next;
    }
      
    // Advance to the correct node in the list
    while(temp->nextFile != NULL && strcmp(filename, temp->nextFile->filename) > 0)
    { temp = temp->nextFile; }
        
    next->nextFile = temp->nextFile;
    temp->nextFile = next;
    temp = temp->nextFile;
  }
  if(DEBUG) printf("add file node %p\n", temp);
  return temp;
} // end add_file_node()


/* int delete_block_node(struct blockNode **blockList, struct block *block)
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
  if(is_block_list_empty(blockList))
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
      return 0;
    }
    last = temp;
    temp = temp->nextBlock;
  }
  return 1;
} // end delete_block_node()


/* fileNode * find_file(struct fileNode **fileList, char *filename)
 * Description:
 *    Search the fileList for a file with a matching filename
 * Input:
 *    struct fileNode **fileList : A pointer to the file list to be searched
 *    char *filename : The name of the file to search for
 * Output:
 *    NULL : The file was not found, either because the list was empty or the file was not in the list
 *    file : A pointer to the fileNode containing the file information
 */
struct fileNode * find_file(struct fileNode **fileList, char *filename)
{
  struct fileNode * file = *fileList;
  if(is_file_list_empty(fileList) || is_block_list_empty(&(file->blockList)))
  {
    print_error(LIST_EMPTY);
    return NULL;
  }
  
  while(file != NULL)
  {
    if(strcmp(file->filename, filename) == 0)
    {
      if(DEBUG) printf("\nfind_file %p\n", file->blockList->block);
      return file;
    }
    else
    {
      file = file->nextFile;
      if(DEBUG) printf("File not found\n");
    }
  }
  return NULL;
}


/* struct block * get_block(struct blockNode **blockList, int blockIndex)
 * Description:
 *    Get a block in the blockList at the specified index
 * Input:
 *    struct blockNode **blockList : Pointer to the first node in this blockList
 *    int blockIndex : The index number of the desired block
 * Output:
 *    NULL : The list is empty
 *    temp->block : The block at the location specified by blockIndex.
 */
struct blockNode * get_block_node(struct blockNode **blockList, int blockIndex)
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
  
  return temp;
}


/* int get_block_count(struct blockNode *blockList)
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


/* int is_file_list_empty(struct fileNode **fileList)
 * Description:
 *    Check if the fileList is empty
 * Input:
 *    struct fileNode ** fileList : Memory address of a file list
 * Output:
 *    1 : The file list is empty
 *    0 : The file list is populated
 */
int is_file_list_empty(struct fileNode **fileList)
{
  if((*fileList)->filename == NULL)
    return 1;
  else
    return 0;
}


/* int is_block_list_empty(struct blockNode **blockList)
 * Description:
 *    Check if the blockList is empty
 * Input:
 *    struct blockNode **blockList : Memory address of a block list
 * Output:
 *    1 : The block list is empty
 *    0 : The block list is populated
 */
int is_block_list_empty(struct blockNode **blockList)
{
  if(*blockList == NULL)
    return 1;
  else
    return 0;
}
