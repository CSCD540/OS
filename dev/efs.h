#ifndef _EFS_H_
#define _EFS_H_ 1

/* Author : Jordan Bondo */

#include <stdlib.h>
#include "efsLL.h"

#ifndef _GLOBAL_H_
#include "globals.h"
#endif

/* Forward Declarations */
int delete_file(char * filename);
struct fileNode * add_file(char *filename, int numBlocks);
struct fileNode * get_file(char *filename);
struct blockNode * get_free_block_node();
void init_disk(struct block disk[]);
/* End Forward Declarations */

struct fileNode *fileList = NULL;
struct blockNode *freeBlockList = NULL;


/* struct fileNode * add_file(char *filename)
 * Description:
 *    add a new file with the specified filename to the fileList
 * Input:
 *    char *filename : the name of the file to add
 * Output:
 *    a pointer to the first node in the fileList
 */
struct fileNode * add_file(char *filename, int numBlocks)
{
  if(!is_file_list_empty(&fileList))
  {
    // Check if a file with the same name already exists
    struct fileNode * temp = find_file(&fileList, filename);
    if(temp != NULL)
    { return  NULL; }
  }
  
  // Add a new file to the fileList and get a pointer to the first block.
  struct fileNode * newFileNode;// = malloc(sizeof(struct fileNode *));
  if(DEBUG) printf("pre add file  %p\n", newFileNode);
  newFileNode = add_file_node(&fileList, filename, numBlocks);
  if(DEBUG) printf("post add file %p\n", newFileNode);
  return newFileNode;
}


/* int delete_file(char * filename)
 * Description:
 *    Delete a file with the specified filename from the fileList
 * Input:
 *    char *filename : the name of the file to delete
 * Output:
 *    a pointer to the first node in the fileList
 */
int delete_file(char * filename)
{
  struct fileNode * file;
  file = find_file(&fileList, filename);
  if(file == NULL)
  { return FILE_NOT_FOUND; }
  
  // Write -1 to all the instructions
  int blocksToWrite = file->numBlocks; // Have to convert from 1 based count to 0
  int dataSize = blocksToWrite * BLOCKSIZE;
  int data[dataSize];
  int i = 0;
  for(; i < dataSize; i++)
  { data[i] = -1; }
  write(&file,  data, dataSize, OVERWRITE, 0);
  
  // Move the blocks back to the freeBlockList
  struct blockNode * blockNode = file->blockList;
  while(blockNode != NULL)
  {
    add_block_node(&freeBlockList, blockNode->block);
    blockNode = blockNode->nextBlock;
    // Free stuff?
  }
  
  // Remove the file from the file list
  delete_file_node(&fileList, filename);
  
  return SUCCESS;
}


/* struct fileNode * get_file(char *filename)
 * Description:
 *    Searches the filesystem for a matching file and returns the results.
 * Input:
 *    char *filename : Name of the file to search for
 * Output:
 *    NULL : File was not found in the system
 *    file : A pointer to the file
 */
struct fileNode * get_file(char *filename)
{
  struct fileNode * file;
  file = find_file(&fileList, filename);
  return file;
}  // Write the file system
  


/* struct block * get_free_block_node()
 * Description:
 *    Get the first free blockNode from the freeBlockList, then remove it from the freeBlockList
 * Input:
 *    none
 * Output:
 *    The first available free blockNode
 */
struct blockNode * get_free_block_node()
{
  struct blockNode * freeNode;
  int blockIndex = 0; // Just want the first free node in the freeBlockList

  freeNode = get_block_node(&freeBlockList, blockIndex);
  if(freeNode != NULL)
  {
    delete_block_node(&freeBlockList, freeNode->block);
    freeNode->nextBlock = NULL;
  }
  return freeNode;
}


/* void init_disk(block disk[])
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
  freeBlockList->block = NULL;
  
  
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
  fileList->blockList = NULL;
  fileList->filename = NULL;
}


/* int export_filesystem()
 * Description:
 *    Writes all the contents of the file system to a file. Writes all the contents of the disk,
 *    all the current file names, the number of blocks associated with those files, and the
 *    block numbers on the disk where the data resides.
 * Input:
 *    None
 * Output:
 *    A file named "efs.vhd" containing all the current filesystem information.
 */
int export_filesystem()
{
  if(is_file_list_empty(&fileList))
  { return LIST_EMPTY; }
  char * filename = "efs.vhd"; // Name of the backup file to write to
  FILE * file;
  // Does the file already exist? If so, annihilate it.
  if(file = fopen(filename, "r"))
  {
    fclose(file);
    remove(filename);
  }
  // Open a file for writing.
  file = fopen(filename, "w");
  fprintf(file, "%d\n", DISKSIZE);
  fprintf(file, "%d\n", BLOCKSIZE);
  fprintf(file, "%d\n", NUMBLOCKS);
  
  // Write the contents of the disk
  int i = 0;
  for( ; i < NUMBLOCKS; i++)
  {
    int j = 0;
    for( ; j < BLOCKSIZE; j++)
    { fprintf(file, "%d\n", disk[i].instructions[j]); }
  }
  
  // Write the file system
  struct fileNode * fileNode = fileList;
  while(fileNode != NULL)
  {
    // Write file specific information
    fprintf(file, "%s\n", fileNode->filename);
    fprintf(file, "%d\n", fileNode->numBlocks);
    
    // Write which block numbers are associated with this file
    struct blockNode * blockNode = fileNode->blockList;
    while(blockNode != NULL)
    {
      fprintf(file, "%d\n", blockNode->block->blockNum);
      blockNode = blockNode->nextBlock;
    }
    fileNode = fileNode->nextFile;
  }
  
  fclose(file);
  return SUCCESS;
}


int import_filesystem()
{
  char * filename = "efs.vhd"; // Name of the backup file to write to
  FILE * file;
  // Does the file exist?
  if(!(file = fopen(filename, "r")))
  { return FILE_NOT_FOUND; }
  
  // Check for consistancy
  int disksize, blocksize, numblocks;
  fscanf(file, "%d", &disksize);
  fscanf(file, "%d", &blocksize);
  fscanf(file, "%d", &numblocks);
  if(disksize != DISKSIZE || blocksize != BLOCKSIZE || numblocks != NUMBLOCKS)
  { printf("Inconsistency with filesystem data specification. File system not restored!\n"); }
  
  // Write the contents of the file to the disk
  int i = 0;
  for( ; i < NUMBLOCKS; i++)
  {
    int j = 0;
    for( ; j < BLOCKSIZE; j++)
    { fscanf(file, "%d", &disk[i].instructions[j]); }
  }
  
  char str [100];
  while(!feof(file))
  {
    fscanf(file, "%s", str);
    printf("filename %s\n", str);
    int blockCount;
    fscanf(file, "%d", &blockCount);
    for(i = 0; i < blockCount; i++)
    {
      int bnum;
      fscanf(file, "%d", &bnum);
      printf("bnum %d\n", bnum);
    }
  }
  
  return SUCCESS;
}
#endif //_EFS_H_
