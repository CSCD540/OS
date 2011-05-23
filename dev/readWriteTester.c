#ifndef _GLOBAL_H_
#include "globals.h"
#endif

#include <stdio.h>
#include "shell.h"

// int open(char * filename)
// {
//   struct fileNode * file;
//   file = get_file(filename);
//   if(file == NULL)
//     return FILE_NOT_FOUND;
//   printf("File Found\n");
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
// 
// 
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


int main()
{
  init_disk(disk);
  int fd = open("milk1.out");
  printf("fd == %d\n", fd);
  save_file("milk1.out");
  save_file("testFile.out");

  int inst;
//   fd = open("milk1.out");
//   printf("fd == %d\n", fd);
//   while((inst = read(fd)) != ENDF)
//     printf("read : %d\n", inst);
  
  fd = open("milk1.out");
  printf("fd == %d\n", fd);
  if(fd >= 0)
    while((inst = read(fd)) != ENDF)
      printf("read : %d\n", inst);
  
  
  
  free(freeBlockList);
  free(fileList);
}
