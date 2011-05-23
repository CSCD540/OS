#ifndef _GLOBAL_H_
#include "globals.h"
#endif

#include <stdio.h>
#include "shell.h"

// /* int open(char * filename)
//  * Description:
//  *    Look for a file on the virtual disk. If it's found, get it's information and setup a 
//  *    new file descriptor in the file descriptor table and return the file descriptor.
//  * Input:
//  *    char * filename : The name of the file to be opened
//  * Output:
//  *    FILE_NOT_FOUND : The requested file was not found on the virtual disk
//  *    FD_LIMIT_EXCEED : The maximum amount of file descriptors has been reached
//  *    i : The file was found and a file descriptor was successfully created. i is the
//  *        file descriptor. In actuality, it is just an index into the file descriptor table.
//  */
// int open(char * filename)
// {
//   struct fileNode * file;
//   file = get_file(filename);
//   if(file == NULL)
//     return FILE_NOT_FOUND;
//   
//   // Find the first available file descriptor
//   int i = 0;
//   for( ; i < MAXFILES; i++)
//     if(files[i].curBlockNode == NULL)
//       break;
//   
//   // Is a file descriptor available?
//   if(i == MAXFILES)
//     return FD_LIMIT_EXCEED;
//   
//   // Found an available file descriptor. Set it up!
//   struct fileDescriptor fd;
//   fd.fdNum = i;
//   fd.filename = file->filename;
//   fd.curInstruction = 0;
//   fd.curBlockNode = file->blockList;
//   files[i] = fd;
//   
//   return i;
// }


// /* int read(int fd)
//  * Description:
//  *    Read the next instruction from the file pointed to by the passed in file descriptor.
//  * Input:
//  *    int fd : The index of the file descriptor in the file descriptor table.
//  * Output:
//  *    OUT_OF_RANGE : Cannot read because the file descriptor is incremented past the end of the file.
//  *    ENDF : End of the file has been reached.
//  *    inst : The instruction located at the file's current locations specificed by the file descriptor.
//  */
// int read(int fd)
// {
//   if(files[fd].curBlockNode == NULL)
//     return OUT_OF_RANGE;
//   
//   int inst;
//   inst = (*files[fd].curBlockNode->block).instructions[files[fd].curInstruction];
//   if(inst < 0)
//     return ENDF;
//   
//   files[fd].curInstruction++;
//   if(files[fd].curInstruction == BLOCKSIZE)
//   {
//     files[fd].curInstruction = 0;
//     files[fd].curBlockNode = files[fd].curBlockNode->nextBlock;
//   }
//   
//   return inst;
// }


// /* int close(int fd)
//  * Description:
//  *    Close a file. Basically just nulls out all the values stored in the file descriptor.
//  * Input:
//  *    int fd : The index of the file descriptor in the file descriptor table.
//  * Output:
//  *    FILE_NOT_FOUND : The file descriptor specified was not found in the file descriptor table
//  *    SUCCESS : The file was successfully closed and the file descriptor freed up.
//  */
// int close(int fd)
// {
//   if(files[fd].curBlockNode == NULL)
//     return FILE_NOT_FOUND;
//   
//   files[fd].fdNum = -1;
//   files[fd].filename = NULL;
//   files[fd].curInstruction = -1;
//   files[fd].curBlockNode = NULL;
//   return SUCCESS;
// }


int main()
{
  init_disk(disk);
  int milk1 = open("milk1.out");
  printf("milk1 == %d\n", milk1);
  save_file("milk1.out");
  save_file("sort.out");

  int inst;
  printf("\nOpening milk1.out..... \n");
  milk1 = open("milk1.out");
  printf("fd milk1 == %d\n", milk1);
  while((inst = read(milk1)) != ENDF)
    printf("%d ", inst);
  
  printf("\n\nOpening sort.out..... \n");
  int sort;
  sort = open("sort.out");
  printf("fd sort == %d\n", sort);
  while((inst = read(sort)) != ENDF)
    printf("%d ", inst);
  
  printf("\n\nOpening milk1.out..... \n");
  int milk2;
  milk2 = open("milk1.out");
  printf("fd milk2 == %d\n", milk2);
  while((inst = read(milk2)) != ENDF)
    printf("%d ", inst);
  
  printf("\nClosing milk1... ");
  close(milk1);
  files[milk1].curBlockNode == NULL ? printf("success!\n") : printf("fail!\n");
  
  printf("\n\nOpening milk1.out..... \n");
  int milk3;
  milk3 = open("milk1.out");
  printf("fd milk3 == %d\n", milk3);
  while((inst = read(milk3)) != ENDF)
    printf("%d ", inst);
  
  printf("\nClosing sort... ");
  close(sort);
  files[sort].curBlockNode == NULL ? printf("success!\n") : printf("fail!\n");
  
  printf("Closing milk2... ");
  close(milk2);
  files[milk2].curBlockNode == NULL ? printf("success!\n") : printf("fail!\n");
  
  printf("Closing milk3... ");
  close(milk3);
  files[milk3].curBlockNode == NULL ? printf("success!\n") : printf("fail!\n");
    
  free(freeBlockList);
  free(fileList);
}
