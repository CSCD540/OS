#include "block.h"
#include <stdio.h>
#include <stdlib.h>

#define OUT_OF_DISK_SPACE = 1;

/* 
 * struct blockNode
 * Description: This struct defines a blockNode in the list.
 * Input: none
 * Output: none
 */
typedef struct {
  block *block; // Pointer to a block on the disk
  struct blockNode *next; // Pointer to the next blockNode in the list.
} blockNode;


/* 
 * struct fileNode
 * Description: This struct defines a fileNode in the list.
 * Input: none
 * Output: none
 */
typedef struct {
  char *filename; // This file's name
  int numBlocks; // The number of block this file occupies
  blockNode *firstBlock; // Pointer to the location where the file's first block begins, or the first node in it's blockList
  struct fileNode  *nextFile; // Pointer to the next file in the file list
} fileNode;


/* 
 * int add_file(fileNode *files, char *filename)
 * Description: Add a new file to the disk
 * Input:
 *    fileNode *files : Pointer to the current list of files
 *    char *filename : Name of the file to be added
 * Output:
 *    OUT_OF_DISK_SPACE : if there is not enough room on the disk to add the file
 *    SUCCESS : if the file was successfully added to the disk.
 *    New file will be stored on the disk. fileList, blockList and freeBlockList will be updated.
 */
int add_file(fileNode *files, char *filename)
{
  // TODO: Need to read through the file and count the number of instructions.
  
  
  
  
  // First file in the list
  if(files == NULL)
  {
    files = malloc(sizeof(fileNode));
  }
  
  // TODO: Add the node to the end of the list
  
  return 0;
}

