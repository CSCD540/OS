#ifndef _GLOBALS_H_
#include "globals.h"
#endif
//#define BLOCKSIZE   4 // Number of records per block

/* 
 * struct block
 * Description:
 *    This struct defines a block in the disk.
 * Input:
 *    none
 * Output:
 *    none
 */
struct block {
  int blockNum;
  int instructions[BLOCKSIZE];
};

