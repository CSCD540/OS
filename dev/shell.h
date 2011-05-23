#include "efs.h"
#include "fileio.h"
#ifndef _GLOBALS_H_
#include "globals.h"
#endif

//void show_man_page(char *cmd);  //Not yet implemented

// Jordan's method declarations
// void concatenate(char *filename);
// void list_directory_contents();
// void dump_file(char *filename);
// int  load_program(char *filename);
// void remove_file(char *filename);
// int  save_file(char *filename);
// void show_help();
// end Jordan's method declarations
void show_exit();


/* Concatenate the contents of the file specified
 */
void concatenate(char *filename)
{
  printf("\n");
  if(filename == NULL)
  {
    printf("SYNTAX: cat filename\n\n");
    return;
  }
  //printf("Concatenate the file %s\n", filename);
  
  struct fileNode * file = get_file(filename);
  
  if(file == NULL)
  {
    print_error(FILE_NOT_FOUND);
    return;
  }
  
  struct blockNode *blockList = file->blockList;
  
  while(blockList != NULL)
  {
    int j;
    for(j = 0; j < BLOCKSIZE; j++)
    {
      int res = blockList->block->instructions[j];
      if(res != -1)
        printf("%c", res);
    }
    blockList = blockList->nextBlock;
  }
  printf("\n");
}


/* Dump the contents of the file specified
 */
void dump_file(char *filename)
{
  printf("\n");
  if(filename == NULL)
  {
    printf("SYNTAX: filedump filename\n\n");
    return;
  }
  
  struct fileNode * file = get_file(filename);
  if(file == NULL)
  {
      print_error(FILE_NOT_FOUND);
      return;
  }
  struct blockNode * blockList = file->blockList;
  printf("\n%s\n", filename);
  print_block_list(blockList);
}


/* List the contents of the current directory
 */
void list_directory_contents()
{
  // printf("Print out the contents of current directory.\n");
  // print_file_list(fileList);
  struct fileNode *fnode = fileList;
  printf("\n");
  if(fnode->filename == NULL)
    return;
  while(fnode != NULL)
  {
    printf("%s\t", fnode->filename);
    if(arg1 != NULL && strcmp(arg1, "-l") == 0)
    { 
      printf("%d blk\t", fnode->numBlocks);
      printf("\n");
    }
    fnode = fnode->nextFile;
  }
  if(arg1 == NULL || strcmp(arg1, "-l") != 0)
    printf("\n");
  printf("\n");
}


/* int load_file(char *file)  
 * Description: This function grabs the program from the disk and loads it into mem[0] starting from [0]
 * Input: filename with path if needed
 * Output: Returns -1 if it fails to open the file or the status of fclose() 
 *  -1: if the file wasn't loaded
 *   0: if everything was alright
 */
int load_program(char *filename)
{
  int fi = 0;
  int coni = 0;
  int size = 0;
  FILE *f;
  int status = 0;
  
  printf("Loading from HD: %s\n", filename);
  f = fopen(filename, "r");
  if (f == NULL)
  {
    return -1; // -1 means file opening fail
  }

  while(fscanf(f, "%d\n", &fi) != EOF)
  {
    mem[0][coni]=fi;
    printf("%d: %d\n", coni, mem[0][coni]);
    size++;
    coni++;
  }
  status = fclose(f);
  return status;
} // End load_file


/* Delete a file from the disk
 */
void remove_file(char *filename)
{
  printf("Delete file \"%s\" from the filesystem.\n", filename);
}


/* int save_file(char *filename)
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
    return FILE_NOT_FOUND;

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
    return DISK_FULL;
  }
  rewind(fd);
  int instructions[numInstructs];
  for(i = 0; fscanf(fd, "%d\n", &instruction) != EOF; i++)
    instructions[i] = instruction;
  fclose(fd);

  struct fileNode *newFile; // Declare a fileNode pointer
  newFile = add_file(filename, numBlocks); // Add a new file to the list and get the pointer to that fileNode
  if(newFile == NULL)
    return DUPLICATE_FILE;
  if(DEBUG) printf("newFile %p\n", newFile);
  newFile->blockList = get_free_block_node();
  write(&newFile, instructions, numInstructs, NEWFILE, 0);
  
  return SUCCESS;
}// end save_file()


/* Show the help screen for the shell
 */
void show_help()
{
  printf("\n ls\n   list the contents of the current directory.\n\n");
  printf(" cat\n   display the contents of a file as ascii text\n\n");
  printf(" debug\n   dunno what this is supposed to do\n\n");
  printf(" diskdump\n   display out all of the disk information\n\n");
  printf(" echo on|off\n   echo the commands typed on the command line\n\n");
  printf(" exit\n   exit the virtual machine\n\n");
  printf(" fdisk\n   format the virtual disk\n\n");
  printf(" filedump\n   display out the binary representation of a file\n\n");
  printf(" help\n   display this document\n\n");
  printf(" hdload\n   load a previously saved virtual hard disk into the current disk\n\n");
  printf(" load\n   schedule a program to run\n\n");
  printf(" man\n   get specific help for a particular command\n\n");
  printf(" memdump\n   display out the current contents of memory\n\n");
  printf(" piddump\n   display out the process table\n\n");
  printf(" ptdump\n   display out the page table\n\n");
  printf(" pwd\n   print the current working directory\n\n");
  printf(" rm\n   delete a file from the disk\n\n");
  printf(" run\n   execute all loaded programs\n\n");
  printf(" save\n   save a file from the physical disk into the virtual disk\n\n");
  printf(" showGlobalMem\n   display the current contents of the global memory\n\n");
  printf(" showLRU\n   display the current least recently used page\n\n");
  printf(" showPage\n   display out a physical memory page\n\n");
  printf(" showRegisterData\n   display the current regester data\n\n");
}


/* Show the exit message and set machineOn to 0 (turn it off)
 */
void show_exit()
{
  printf("***********************************************************\n");
  printf("*       Thanks for using the Eagle Virtual Machine!       *\n");
  printf("*                         GOOD-BYE!                       *\n");
  printf("***********************************************************\n");
  machineOn = 0;
}

