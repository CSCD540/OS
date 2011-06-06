#ifndef _HELPERS_H_ 
#define _HELPERS_H_ 1

#ifndef _GLOBALS_H_
#include "globals.h"
#endif

void init_gmem();
void init_mem();
void init_pt();
void init_reg();
void reset_memory();

void print_block_list(struct blockNode *blockList);
void print_disk(struct block disk[]);
void print_error();
void print_file_list(struct fileNode *head);
void print_gmem();
void print_mem();
void print_mem_pages();
int  print_page(int pageNum);
void print_processes();
void print_pt();
void print_lru();
void print_register(int reg[][REGISTERSIZE]);
void print_stack(int stack[][STACKSIZE],int sp[]);
//void show_register_data();

/*  ----------------------------------------------------
    Initialize globals 
    ----------------------------------------------------*/
void init_gmem()
{
  int i;
  
  for(i = 0; i < MAXGMEM; i++)
    gmem[i] = -1;
}

void init_mem()
{
  int i,j;
    
  for(i = 0; i < MAXPRO; i++)
	  for(j = 0; j < MAXMEM; j++)
      mem[i][j] = -1;
}

/* void init_pt() // Internal method
 * Description: This function initializes the page table to be empty (i.e. -1's)
 * Input: None
 * Output: None
 */
void init_pt()
{
  int i,j;
  
  int size = PAGESIZE - 1;//Pagesize is assumed to be a power of 2
  pageBits = 0;
  while(size>0)//Count the bits we need to shift for a page
  {
    pageBits += size & 1;
    size = size>>1;
  }
  
  for(i = 0; i < NUMPAGES; i++)
    for(j = 0; j < 4; j++)
      pageTable[i][j] = -1;
}

void init_reg()
{
  int i,j;
  
  for(i = 0; i < MAXPRO; i++)
	  for(j = 0; j < REGISTERSIZE; j++)
      reg[i][j] = -1; 
}

void reset_memory()
{
	init_gmem();
	init_mem();
	init_pt();
	init_reg();
}


/*  ----------------------------------------------------
    Debug routines 
    ----------------------------------------------------*/

/* void print_block_list(struct blockNode *head)
 * Description:
 *    Print out all the data in all the blocks in this blockList
 * Input:
 *    struct blockNode *head : Pointer to the first node in this blockList
 * Output:
 *    Screen output of the data in the blockList
 */
void print_block_list(struct blockNode *blockList)
{
  printf("\n");
  if(blockList == NULL)
    print_error(LIST_EMPTY);
  else
    while(blockList != NULL)
    {
      if(DEBUG) printf("blockList: %p\n", blockList);
      if(DEBUG) printf("blockList->block: %p\n", blockList->block);
      printf("Block #%2d ", blockList->block->blockNum);
      int i;
      for(i = 0; i < BLOCKSIZE; i++)
        printf(" %5d", blockList->block->instructions[i]);
      //  ;
      printf("|\n");
      blockList = blockList->nextBlock;
    }
    printf("\n");
}

/* void print_disk(struct block disk[])
 * Description:
 *    print out the contents of the disk
 * Input:
 *    struct block disk[] : The disk whos content's you wish to print
 * Output:
 *    screen output of all data stored in all blocks on the disk
 */
void print_disk(struct block disk[])
{
  int i, j;
  printf("\n");
  for(i = 0; i < NUMBLOCKS; i++)
  {
    printf("Block #%2d ", i);
    for(j = 0; j < BLOCKSIZE; j++)
    {
      printf(" %5d", disk[i].instructions[j]);
      if( j == (BLOCKSIZE - 1) || (j + 1) % BLOCKSIZE == 0)
          printf(" | %5d\n", i * BLOCKSIZE + j);
    }
  }
  printf("\n");
}

/* 
 * void print_error(int errno)
 * Description:
 *    Print out the error message associated with the errno
 * Input:
 *    int errno : The error number
 * Output:
 *    Screen output of what error occurred.
 */
void print_error(int errno)
{
  printf("\nERROR: ");
  switch(errno)
  {  
    case DISK_FULL:
      printf("Not enough room on the disk.\n");
      break;
    
    case DUPLICATE_FILE:
      printf("Another file with that name already exists.\n");
      break;
      
    case FILE_NOT_FOUND:
      printf("File not found.\n");
      break;

    case LIST_EMPTY:
      printf("List is empty.\n");
      break;
    
    case OUT_OF_RANGE:
      printf("Value out of range.\n");
      break;
      
    case FD_LIMIT_EXCEED:
      printf("Too many files open.\n");
      break;
      
    default:
      printf("An unspecified has error occurred. %d\n", errno);
  }
}

/* void print_file_list(struct fileNode *head)
 * Description:
 *    Print out all the file currently stored on the disk
 * Input:
 *    struct fileNode *head : Pointer to the first node in the fileList
 * Output:
 *    Screen output of the files in the fileList
 */
void print_file_list(struct fileNode *head)
{
  printf("\n");
  if(head == NULL)
    print_error(LIST_EMPTY);
  else
  {
    printf("Filename      Blocks\n");
    while(head != NULL)
    {
      printf("%s%11d\n", head->filename, head->numBlocks);
      head = head->nextFile;
      
    }
  }
  printf("\n");
}

void print_gmem()
{
  int i;
  printf("\r\n%c[%d;%dm   GLOBAL MEMORY   %c[%dm\r\n", 27, 1, 42, 27, 0);
  printf("Global memory: size %d\n", MAXGMEM);
  for(i = 0; i < MAXGMEM; i++)
  {
    printf(" %5d", gmem[i]);
    if( i == (MAXGMEM - 1) || (i + 1) % 8 == 0)
        printf(" | %5d\n", i);
  }
  printf("\n");
}

/* void print_mem()
 * Description: This function does a memory dump of mem[][]
 * Input: none
 * Output: Displays the contents of mem on the screen
 */
void print_mem()
{
    int i,j;
    printf("\n");
    printf("\r\n%c[%d;%dm   MAIN MEMORY   %c[%dm\r\n", 27, 1, 42, 27, 0);
    printf("Memory: size %d (per process)\n", MAXMEM);
    for(i = 0; i < 1/*MAXPRO*/; ++i)
    {
        printf("Process %d: Addresses 0 - %d with %d pages of size %d\n", i, MAXMEM, NUMPAGES, PAGESIZE);
        for(j = 0; j < MAXMEM; ++j)
        {
            printf(" %5d", mem[i][j]);
            if( j == (MAXMEM - 1) || (j + 1) % PAGESIZE == 0)
                printf(" | %5d\n", j);
        }
        printf("\n");
    }
    printf("\n");
}

/* void print_mem_pages()
 * Description: This function does a memory dump of mem[][]
 * Input: none
 * Output: Displays the contents of mem on the screen in a long format optimal
 *           for viewing the memory pages
 */
void print_mem_pages()
{
  printf("\n");
  printf("\r\n------------------\r\n|%c[%d;%dm   MAIN MEMORY  %c[%dm|\r\n|----------------|\r\n", 27, 1, 42, 27, 0);
  
  int i,j;
  for(i = 0; i < MAXPRO; i++)
  {
//    printf("| Mem column: %2d |\r\n", i);
    for(j = 0; j < MAXMEM; j++)
    {
      if(j % PAGESIZE == 0)
      {
        printf("|                |\r\n");
        printf("|-- PhysPage %d --|\r\n", j / PAGESIZE);
        printf("|                |\r\n");
        printf("|PhysAddr | Value|\r\n");
        printf("|-------- | -----|\r\n");
      }
      printf("|%5d:   | %5d|\r\n", j, mem[i][j]);
    }
  }
  
  printf("------------------\r\n");
  printf("\n");
}

int print_page(int pageNum)
{
  printf("\n");
  if(pageNum >= NUMPAGES || pageNum < 0)
  {
    return OUT_OF_RANGE;
  }
  
  printf("\nPage %d:\n", pageNum);
  int i = PAGESIZE * pageNum;
  int lastAddr = i + PAGESIZE;
  for(; i < lastAddr; i++)
    printf(" %5d", mem[0][i]);
  printf("\n\n");
  
  return SUCCESS;
}

void print_processes()
{
  printf("\n");
  printf("\r\nPID Table:\r\n");
  printf("|-----|-------|--------|---------|---------|-----------------|\r\n");
  printf("| Pid | state | status | poffset | roffset |        filename |\r\n");
  printf("|-----|-------|--------|---------|---------|-----------------|\r\n");
  int i;
  for(i = 0; i < MAXPRO; i++)
  {
    //printf("%c[%d;%dm", 27, 0, 31);
    printf("|  %2d |  %3d  |  %4d  |  %5d  |  %5d  | %14s  |\r\n", 
            i, 
            processes[i].state,
            processes[i].status,  
            processes[i].poffset, 
            processes[i].roffset, 
            processes[i].filename);
    printf("%c[%dm", 27, 0);
    printf("|-----|-------|--------|---------|---------|-----------------|\r\n");
  }
  printf("\n");
}

/* void print_pt()
 * Description: Prints the current page table
 * Inputs: None
 * Output: None
 */
void print_pt()
{
  printf("\r\nPage Table (%c[%dmLRU in red%c[%dm):\r\n", 27, 31, 27, 0);
  printf("            -------------------------\r\n");
  printf("            | pid | vpn | lru | drt |\r\n");
  printf("------------|-----|-----|-----|-----|\r\n");
  int i;
  for(i = 0; i < NUMPAGES; i++)
  {
    if(i == lru)
    {
      printf("%c[%d;%dm", 27, 0, 31);
      printf("|PysPage %2d | %3d | %3d | %3d | %3d |\r\n", i, pageTable[i][0],  pageTable[i][1], pageTable[i][2], pageTable[i][3]);
      printf("%c[%dm", 27, 0);
    }
    else
      printf("|PysPage %2d | %3d | %3d | %3d | %3d |\r\n", i, pageTable[i][0],  pageTable[i][1], pageTable[i][2], pageTable[i][3]);
    printf("|-----------------------------------|\r\n");
  }
  printf("\r\n");
}
/* end of print_pt() method */

void print_lru()
{
  printf("\r\nShould this print just the least recently used page number or the actual page information?\r\n");
  printf("LRU: %d\r\n\r\n", lru);
}

void print_register(int reg[][REGISTERSIZE])
{
  int i, j;
  printf("\r\n--------------------\r\n|%c[%d;%dm   REGISTER DATA  %c[%dm|\r\n|------------------|\r\n", 27, 1, 42, 27, 0);
  for(i = 0; i < MAXPRO; i++)
  {
    printf("- Process %d: ",i);
    for(j = 0; j < REGISTERSIZE; j++)
    {
      printf(" %5d", reg[i][j]);
    }
    printf("      -\n");
  }
  printf("--------------------------------------------------\n");
  printf("\n");
}

void print_stack(int stack[][STACKSIZE], int sp[])
{
  int i, j;
  printf("\n");
  for(i = 1; i < MAXPRO; i++)
  {
    printf("Stack contents for process %d\n", i);
    for(j = 0; j < STACKSIZE; j++)
    {
        printf(" %5d", stack[i][j]);
        if( j == (STACKSIZE - 1) || (j + 1) % PAGESIZE == 0)
          printf(" | %d\n", j);
    }
    printf(" | SP at %5d\n", sp[i]);
  }
  printf("\n");
}
#endif //_HELPERS_H_
