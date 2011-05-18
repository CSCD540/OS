#ifndef _GLOBALS_H_ 
#define _GLOBALS_H_ 1

//-----------------------------------------------------------------------------
//                          Constants
//-----------------------------------------------------------------------------

#define MAXPRO        1   // max num of processes
#define MAXPROGRAMS   8   // max number of program a CPU can run
#define MAXMEM        64  // max size of a process in word/sizeof(int) bytes
#define STACKSIZE     100 // max size of the stack
#define REGISTERSIZE  10  // size of each process registers
#define MAXGMEM       20  // max size of global memory
#define NORMAL        0   // denotes a normal return from exe()
#define LOCKED        1   // stops process switching until unlock
#define UNLOCKED      2   // remove lock
#define ENDPROCESS    3

#define p0WRITE       4                           // tells p0 to run-p0 should only run after a write to gmem
#define DBGCPU        1
#define DBGCPU1       0

//General
#define BLOCKSIZE     4                           // size for per block

//Filesystem 
//#define DISKSIZE      (16*1024)                   // 16kB
#define DISKSIZE      ((BLOCKSIZE) * 14)          // Total size of the disk
#define BLOCKS        ((DISKSIZE) / (BLOCKSIZE))  // total number of blocks
#define NUMBLOCKS     ((DISKSIZE) / (BLOCKSIZE))  // Total number of blocks on the disk
#define LIST_EMPTY 0
#define APPEND 0 // begin writeing at the end of the file
#define OVERWRITE 1 // begin writing at the beginning of the file
#define NEWFILE 2 // new file

//Memory
#define PAGESIZE      ((BLOCKSIZE) * 4)           // size of each page in words 2-bytes
#define NUMPAGES      ((MAXMEM) / (PAGESIZE))     // Number of pages in page table

#define keyhit(a) {if(DBGCPU1){printf("hit enter --(%d)", a); getchar();}}


//-----------------------------------------------------------------------------
//                          Structs
//-----------------------------------------------------------------------------
/* 
 * struct blockNode
 * Description:
 *    This struct defines a blockNode in the list.
 */
struct block {
  int blockNum;
  int instructions[BLOCKSIZE];
};
/* 
 * struct blockNode
 * Description:
 *    This struct defines a blockNode in the list.
 */
struct blockNode {
  struct block     *block; // Pointer to a block on the disk
  struct blockNode *nextBlock;  // Pointer to the next blockNode in the list.
};


/* 
 * struct fileNode
 * Description:
 *    This struct defines a fileNode in the list.
 */
struct fileNode {
  char   *filename; // This file's name
  int    numBlocks; // The number of block this file occupies
  struct blockNode  *blockList; // Pointer to the location where the file's first block begins, or the first node in it's blockList
  struct fileNode   *nextFile;   // Pointer to the next file in the file list
};

struct process{
    int pid;
    char *filename;
    int ip;     //virtual IP this is where the process believes it's at
    int status; //0 = not finished, 1 = terminated
    int state;  //state - running, 0 ready, waiting, suspended  
};

//-----------------------------------------------------------------------------
//                          Variables
//-----------------------------------------------------------------------------

struct block disk[NUMBLOCKS];           //Our virtual HD
struct process processes[MAXPROGRAMS];  //Our pid table
int  gmem[MAXGMEM];                     // global var sit here
int  mem[MAXPRO][MAXMEM];               // Main mem for each process
int  reg[MAXPRO][REGISTERSIZE];
int  endprog[MAXPRO];                   // last instruction of proc
int  jsym[60];
int  pageBits = 0;
int  curProcesses = 0;                  //Number of processes loaded
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

//Pagetable variables
// The process table array which is indexed on the process id, and
//  contains the priority (?), and the file descriptor/filename
int processTable[MAXPRO][2];
// The page table array which contains the process id,
//  virtual page number, dirty bit, and LRU info
int pageTable[NUMPAGES][4];
int lru;
//End pagetable variables

// Jordan's Variables
char input[30]; //Console input
// end Jordan's Variables

#endif //_GLOBALS_H_

