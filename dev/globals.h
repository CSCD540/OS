#ifndef _GLOBALS_H_ 
#define _GLOBALS_H_ 1

#define MAXPRO        1   // max num of processes
#define MAXMEM        64  // max size of a process in word/sizeof(int) bytes
#define STACKSIZE     100 // max size of the stack
#define REGISTERSIZE  10  // size of each process registers
#define MAXGMEM       20  // max size of global memory
#define NORMAL        0   // denotes a normal return from exe()
#define LOCKED        1   // stops process switching until unlock
#define UNLOCKED      2   // remove lock
#define ENDPROCESS    3

#define p0WRITE       4   // tells p0 to run-p0 should only run after a write to gmem
#define DBGCPU        1
#define DBGCPU1       0

// General
#define BLOCKSIZE     4   // size for per block

// Filesystem 
#define DISKSIZE      ((BLOCKSIZE) * 20)          // Total size of the disk
#define BLOCKS        ((DISKSIZE) / (BLOCKSIZE))  // total number of blocks
#define NUMBLOCKS     ((DISKSIZE) / (BLOCKSIZE))  // Total number of blocks on the disk

// Return Values
#define SUCCESS 0        // No error
#define APPEND 1         // Begin writing at the end of the file
#define OVERWRITE 2      // Begin writing at the beginning of the file
#define NEWFILE 3        // New file
#define DISK_FULL 4      // No more room on the disk!
#define FILE_NOT_FOUND 5 // File was not located
#define LIST_EMPTY 6     // List contains no elements

// Memory
#define PAGESIZE      ((BLOCKSIZE) * 4)           // size of each page in words 2-bytes
#define NUMPAGES      ((MAXMEM) / (PAGESIZE))     // Number of pages in page table

#define keyhit(a) {if(DBGCPU1){printf("hit enter --(%d)", a); getchar();}}

struct block {
  int blockNum;
  int instructions[BLOCKSIZE];
};

struct process{
    int pid;
    char * filename;
    
};

// Variables
struct block disk[NUMBLOCKS]; //Our virtual HD
int  gmem[MAXGMEM];         // global var sit here
int  mem[MAXPRO][MAXMEM];   // Main mem for each process
int  reg[MAXPRO][REGISTERSIZE];
int  endprog[MAXPRO];       // last instruction of proc
int  jsym[60];
int  pid = 0;               // process id
int  p0running;
int  sizeOfDisk = DISKSIZE;
int  DEBUG = 0;
int  totalBlocks = BLOCKS;  // Total blocks
int  fileIndex=0;           // main index
int  searchIndex=0;         // temp index for search
char *cmd;                  // The command(save, load, ls...etc.)
char *arg1;                 // The argument(file's name)
int  diskIndex=0;           // main index
int  machineOn = 1;
int  tempmem[MAXPRO][200];  // for PTB - loading all of the process information here
int  fileMonitor[30];
int  HALTED=0;
int  gfd;                   // file discriptor

// end Variables

struct process * processTable[MAXPRO];

// Page Table Variables

/// The page table array which contains the process id,
///  virtual page number, dirty bit, and LRU info
int pageTable[NUMPAGES][4];
int lru;

// End page table variables


// Jordan's Variables
char input[30]; //Console input
// end Jordan's Variables

#endif //_GLOBALS_H_

