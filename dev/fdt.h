/* fdt.h
 * 
 * Author : Jordan Bondo 
 * 
 * This is a linked list used for storing file descriptors
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _GLOBALS_H_
#include "globals.h"
#endif

struct fileDescriptor {
  int fd;
  struct fileNode * file;
  struct fileDescriptor * nextFD;
};

