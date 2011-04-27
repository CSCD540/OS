#define BLOCKSIZE   4 // Number of records per block

/* 
 * struct block
 * Description: This struct defines a block in the disk.
 * Input: none
 * Output: none
 */
typedef struct {
  int instructions[BLOCKSIZE];
} block;

