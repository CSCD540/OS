#ifndef _EFS_H_
#define _EFS_H_ 1

/* Author : Jordan Bondo */

#include <stdlib.h>
#include "efsLL.h"

#ifndef _GLOBAL_H_
#include "globals.h"
#endif

/* Forward Declarations */
struct fileNode * add_file(char *filename, int numBlocks);
struct fileNode * get_file(char *filename);
struct blockNode * get_free_block_node();
void init_disk(struct block disk[]);
int save_file(char *filename);
int write(struct fileNode **fileListNode, int data[], int count, int writeMode);
void print_block_list(struct blockNode *blockList);
void print_disk(struct block disk[]);
void print_file_list(struct fileNode *head);
/* End Forward Declarations */


struct fileNode *fileList = NULL;
struct blockNode *freeBlockList = NULL;


/* struct blockNode * add_file(char *filename)
 * Description:
 *    add a new file with the specified filename to the fileList
 * Input:
 *    char *filename : the name of the file to add
 * Output:
 *    a pointer to the first node in the blockList
 */
struct fileNode * add_file(char *filename, int numBlocks)
{
  // Add a new file to the fileList and get a pointer to the first block.  
  struct fileNode * newFileNode = malloc(sizeof(struct fileNode *));
  if(DEBUG) printf("pre add file  %p\n", newFileNode);
  newFileNode = add_file_node(&fileList, filename, numBlocks);
  if(DEBUG) printf("post add file %p\n", newFileNode);
  return newFileNode;
}


struct fileNode * get_file(char *filename)
{
  struct fileNode * file;
  file = find_file(&fileList, filename);
  return file;
}


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
 * int save_file(char *filename)
 * Description:
 *    Attempt to open a file on the physical disk. If it exists, read through it
 *    and count the number of lines (instructions) and number of blocks necessary.
 *    If disk space is available, rewind the file pointer and
 *    read all the instructions into an array. Add a new file to the fileList
 *    and call write() in OVERWRITE mode to write all the instructions to the disk.
 * Input:
 *    char *filename : Name of the file on the physical disk. This will also be used
 *    as the filename in the virtual disk.
 * Output:
 *    -1 : File could not be opened. Either it doesn't exist on the physical disk, 
 *          or somethign else happened.
 *    -2 : Insufficient disk space
 *     0 : File was successfully created and written to the virtual disk
 */
int save_file(char *filename)
{
  FILE *fd;
  fd = fopen(filename, "r");
  if(fd == NULL)
    return -1;

  int numBlocks = 1; // Start at one block because this is the first block.
  int numInstructs = 0; // Counter for the number of lines in the file
  int instruction = 0; // What the line says

  // Read the file through to the end
  int i;
  for(i = 0; fscanf(fd, "%d\n", &instruction) != EOF; i++)
  {
    // printf("Inst# %3d: %d\n", numInstructs, instruction);
    numInstructs++;
    
    // Check if we need to increment the number of blocks that will be used
    if(numInstructs % BLOCKSIZE == 0)
      numBlocks++;
  }
  // Enough available disk space?
  if(get_block_count(freeBlockList) < numBlocks)
  {
    fclose(fd);
    return -2;
  }
  rewind(fd);
  int instructions[numInstructs];

  for(i = 0; fscanf(fd, "%d\n", &instruction) != EOF; i++)
    instructions[i] = instruction;
  fclose(fd);

  struct fileNode *newFile; // Declare a fileNode pointer
  newFile = add_file(filename, numBlocks); // Add a new file to the list and get the pointer to that fileNode
  printf("newFile %p\n", newFile);
  newFile->blockList = get_free_block_node();
  write(&newFile, instructions, numInstructs, NEWFILE);
  
  return 0;
}// end save_file()


/* 
 * int write(struct fileNode **fileListNode, int data[], int count, int writeMode)
 * Description:
 *    Write data to a file on the VFS. If writeMode is OVERWRITE, writing begins at
 *    the beginning of the file, but 
 * Input:
 *    char *filename : Name of the file on the physical disk. This will also be used
 *    as the filename in the virtual disk.
 * Output:
 *    -1 : File could not be opened. Either it doesn't exist on the physical disk, 
 *          or somethign else happened.
 *    -2 : Insufficient disk space
 *     0 : File was successfully created and written to the virtual disk
 */
int write(struct fileNode **fileListNode, int data[], int count, int writeMode)
{
  int newFile;
  newFile = writeMode;
  struct blockNode *blockNode = (*fileListNode)->blockList;
  struct block *curBlock; // Declare a block pointer.
  printf("\nstart write : fileListNode->blockList->block %p\n", (*fileListNode)->blockList->block);
  printf("blockNode %p\n", blockNode);
  // Appending to end of file?
  int i; // First empty index in instructions array
  i = 0;
   // TODO: FIX APPEND.
  if(writeMode == APPEND)
  {
/*    // Forward the list to the last node*/
/*    while(blockNode->nextBlock != NULL)*/
/*      blockNode = blockNode->nextBlock;*/
/*    */
/*    curBlock = blockNode->block;*/
/*    // Does this block already have data in it?*/
/*    if(curBlock != NULL)*/
/*      // Find the first free instruction index*/
/*      for(i; curBlock->instructions[i] != -1 || i < BLOCKSIZE; i++);*/
/*    // Get the next free block on the disk*/
/*    else*/
/*      curBlock = get_free_block(); // Get the first available block on the disk*/
  }
  else // OVERWRITE or NEWFILE
    curBlock = blockNode->block;
  
  int j;
  for(j = 0; j < count; j++)
  {
    if((i == 0) && (newFile != NEWFILE))
    { 
      printf("curBlock %p\n", curBlock);
      printf("\nadding new blockNode...\n");
      blockNode->nextBlock = get_free_block_node();
      blockNode = blockNode->nextBlock;
      curBlock = blockNode->block;
      printf("new blockNode %p\n", blockNode);
      printf("new block %p\n", curBlock);
    }
    printf("writing  %3d  to block  %2d\n", data[j], curBlock->blockNum);
    curBlock->instructions[i] = data[j];

    i++;
    // If i == BLOCKSIZE, we have written to the last availble instruction location and need to get a new blockNode next time.
    if(i == BLOCKSIZE)
      i = 0;
    // If this is a new file, set newFile to 0 so that when i == 0 we will get a new blockNode
    if(newFile == 2)
      newFile = 0;
  }
  printf("end write : fileListNode->blockList->block %p\n", (*fileListNode)->blockList->block);
  blockNode->nextBlock = NULL;
  return 0;
} // end write()

/* void print_block_list(struct blockNode *head)
 * Description:
 *    Print out all the data in all the blocks in this blockList
 * Input:
 *    struct blockNode *head : Pointer to the first node in this blockList
 * Output:
 *    Screen output of the data in the blockList
 */
void print_block_list(struct blockNode *blockList)
{
  if(blockList == NULL)
    print_error(LIST_EMPTY);
  else
    while(blockList != NULL)
    {

      if(DEBUG) printf("blockList: %p\n", blockList);
      if(DEBUG) printf("blockList->block: %p\n", blockList->block);
      printf("Block #%2d ", blockList->block->blockNum);
      int i;
      for(i = 0; i < BLOCKSIZE; i++)
        printf(" %d", blockList->block->instructions[i]);
      //  ;
      printf("\n");
      blockList = blockList->nextBlock;
    }
}


/* void print_disk(struct block disk[])
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
    printf("Block #%2d ", i);
    for(j = 0; j < BLOCKSIZE; j++)
      printf(" %d", disk[i].instructions[j]);
    printf("\n");
  }
}


/* void print_file_list(struct fileNode *head)
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
    print_error(LIST_EMPTY);
  else
  {
    printf("Filename      Blocks\n");
    while(head != NULL)
    {
      printf("%s%11d\n", head->filename, head->numBlocks);
      head = head->nextFile;
    }
  }
}

#endif //_EFS_H_
