#ifndef _GLOBALS_H_ 
#define _GLOBALS_H_ 1

//-----------------------------------------------------------------------------
//                          Debug
//-----------------------------------------------------------------------------

#define DEBUG         0 // Run debugging
#define PT_DBG_LVL    0 // The page table debugging level
#define DBGCPU        1 // Turn CPU debugging on(1)/off(0)
#define STEPCPU       0 // Make user hit key to advance program counter
const char * indent0 = "";
const char * indent1 = "\t\t\t\t\t";
const char * indent2 = "\t\t\t\t\t\t\t\t\t\t";
char * indent;        //Used for formatting scheduled processes

//-----------------------------------------------------------------------------
//                          Constants
//-----------------------------------------------------------------------------

#define MAXPRO        8   // max num of processes
#define MAXMEM        64  // max size of a process in word/sizeof(int) bytes
#define STACKSIZE     128 // max size of the stack
#define REGISTERSIZE  10  // size of each process registers
#define MAXGMEM       32  // max size of global memory
#define NORMAL        0   // denotes a normal return from exe()
#define LOCKED        1   // stops process switching until unlock
#define UNLOCKED      2   // remove lock
#define ENDPROCESS    3
#define ENDPROGRAM    4

// General
#define BLOCKSIZE     4  // number of instructions per block

// Filesystem 
#define DISKSIZE      ((BLOCKSIZE) * 110)          // Total size of the disk
#define BLOCKS        ((DISKSIZE) / (BLOCKSIZE))  // Total number of blocks
#define MAXFILES      1000                        // How many files can be opened at once
#define NUMBLOCKS     ((DISKSIZE) / (BLOCKSIZE))  // Total number of blocks on the disk

//Process Status codes
#define READY             0   //Process ready to run
#define RUNNING           1   //Process running
#define WAITING           2   //Process waiting on IO to return
#define SUSPENDED         3   //Process waiting on IO to return

//Process states
#define NOT_FINISHED      0   //Processes still running
#define TERMINATED        1   //Process ended


// Return Values
#define SUCCESS           0   // No error
#define APPEND            1   // Begin writing at the end of the file
#define OVERWRITE         2   // Begin writing at the beginning of the file
#define NEWFILE           3   // New file
#define DISK_FULL         4   // No more room on the disk!
#define FILE_NOT_FOUND   -5   // File was not located
#define LIST_EMPTY        6   // List contains no elements
#define FD_LIMIT_EXCEED  -7   // Maximum number of file descriptors has been reached
#define ENDF             -8   // At the end of the process or file
#define OUT_OF_RANGE     -9   // Argument was out of range
#define DUPLICATE_FILE   10   // Another file with that file name was found in the pwd

// Memory
#define PAGESIZE   ((BLOCKSIZE) * 4)        // size of each page in words 2-bytes
#define NUMPAGES   ((MAXMEM) / (PAGESIZE))  // Number of pages in page table

#define keyhit(a) {if(STEPCPU){printf("hit enter --(%d)", a); getchar();}}


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


struct process {
    int pid;
    char *filename;
    int ip;       //virtual IP this is where the process believes it's at
    int page;     //Actual page number returned from lookup
    int offset;   //Offset from the ip
    int roffset;  //Offset to the beginning of the registers for the process
    int poffset;  //Offset to the beginning of the process
    int status;   //0 = not finished, 1 = terminated
    int state;    //state - 0 ready, 1 running, 2 waiting on IO, 3 suspended (not implemented)
    int iodelay;  //Number of cycles to delay for io simulation
};

//-----------------------------------------------------------------------------
//                          Variables
//-----------------------------------------------------------------------------

struct block disk[NUMBLOCKS];           //Our virtual HD
struct process processes[MAXPRO];  //Our pid table
int  gmem[MAXGMEM];                     // global var sit here
int  mem[MAXPRO][MAXMEM];               // Main mem for each process
//int  endprog[MAXPRO];                   // last instruction of proc
//int  jsym[60];
int  pageBits = 0;
int  curProcesses = 0;                  //Number of processes loaded


// Variables
int  HALTED = 0;

int  endprog[MAXPRO];       // Last instruction of proc
int  gmem[MAXGMEM];         // Global var sit here
int  jsym[60];
int  mem[MAXPRO][MAXMEM];   // Main mem for each process
int  machineOn = 1;         // Is the machine still running?
int  reg[MAXPRO][REGISTERSIZE];
int  tempmem[MAXPRO][200];  // For PTB - loading all of the process information here

char *arg1;      // The argument(file's name)
char *cmd;       // The command(save, load, ls...etc.)
char input[30];  // Console input

//Pagetable variables
// The process table array which is indexed on the process id, and
//  contains the priority (?), and the file descriptor/filename
//int processTable[MAXPRO][2];
// The page table array which contains the process id,
//  virtual page number, dirty bit, and LRU info
int pageTable[NUMPAGES][4];
int lru;
//End pagetable variables
struct process * processTable[MAXPRO];

// Jordan's Variables
char input[30]; //Console input
// end Jordan's Variables

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
