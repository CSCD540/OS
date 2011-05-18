#ifndef _GLOBALS_H_ 
#define _GLOBALS_H_ 1

#define DEBUG   0 // Run debugging

#define MAXPRO        1   // max num of processes
#define MAXMEM        64  // max size of a process in word/sizeof(int) bytes
#define STACKSIZE     100 // max size of the stack
#define REGISTERSIZE  10  // size of each process registers
#define MAXGMEM       20  // max size of global memory
#define NORMAL        0   // denotes a normal return from exe()
#define LOCKED        1   // stops process switching until unlock
#define UNLOCKED      2   // remove lock
#define ENDPROCESS    3

#define p0WRITE   4  // tells p0 to run-p0 should only run after a write to gmem
#define DBGCPU    1
#define DBGCPU1   0

// General
#define BLOCKSIZE  4  // number of instructions per block

// Filesystem 
#define DISKSIZE      ((BLOCKSIZE) * 40)          // Total size of the disk
#define BLOCKS        ((DISKSIZE) / (BLOCKSIZE))  // Total number of blocks
#define MAXFILES      1000                        // How many files can be opened at once
#define NUMBLOCKS     ((DISKSIZE) / (BLOCKSIZE))  // Total number of blocks on the disk

// Return Values
#define SUCCESS         0   // No error
#define APPEND          1   // Begin writing at the end of the file
#define OVERWRITE       2   // Begin writing at the beginning of the file
#define NEWFILE         3   // New file
#define DISK_FULL       4   // No more room on the disk!
#define FILE_NOT_FOUND  -5  // File was not located
#define LIST_EMPTY      6   // List contains no elements
#define FD_LIMIT_EXCEED -7  // Maximum number of file descriptors has been reached

// Memory
#define PAGESIZE   ((BLOCKSIZE) * 4)        // size of each page in words 2-bytes
#define NUMPAGES   ((MAXMEM) / (PAGESIZE))  // Number of pages in page table

#define keyhit(a) {if(DBGCPU1){printf("hit enter --(%d)", a); getchar();}}


/* struct block
 * Description:
 *    This struct defines the base elements of our virtual disk.
 */
struct block {
  int blockNum;
  int instructions[BLOCKSIZE];
};


/* struct fileDescriptor
 * Description:
 *    This struct defines a file descriptor. It is used to hold information about a
 *    file that is currently open and being accessed.
 */
struct fileDescriptor {
  char * filename;
  int fdNum;
  int curInstruction;
  struct blockNode * curBlockNode;
};


struct process{
    int pid;
    char * filename;
};


// Variables
int  HALTED = 0;

int  endprog[MAXPRO];       // Last instruction of proc
int  gmem[MAXGMEM];         // Global var sit here
int  jsym[60];
int  mem[MAXPRO][MAXMEM];   // Main mem for each process
int  machineOn = 1;         // Is the machine still running?
int  pid = 0;               // Process id
int  p0running;
int  reg[MAXPRO][REGISTERSIZE];
int  tempmem[MAXPRO][200];  // For PTB - loading all of the process information here

char *arg1;      // The argument(file's name)
char *cmd;       // The command(save, load, ls...etc.)
char input[30];  // Console input

struct block disk[NUMBLOCKS];          // Our virtual HD
struct fileDescriptor files[MAXFILES]; // File descriptor table
struct process * processTable[MAXPRO];
// end Variables


// Page Table Variables
/// The page table array which contains the process id,
///  virtual page number, dirty bit, and LRU info
int pageTable[NUMPAGES][4];
int lru;
// End page table variables
#endif //_GLOBALS_H_
