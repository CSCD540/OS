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

void print_gmem();
void print_mem();
void print_mem_pages();
void print_pt();
void print_register(int reg[][REGISTERSIZE]);
void print_stack(int stack[][STACKSIZE],int sp[]);
void show_register_data();

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
    case LIST_EMPTY:
      printf("List is empty.\n");
      break;
      
    case FILE_NOT_FOUND:
      printf("File not found.\n");
      break;
      
    case DISK_FULL:
      printf("Not enough room on the disk.\n");
      break;
      
    default:
      printf("An unspecified has error occurred.\n");
  }
}


void print_gmem()
{
  int i;
  printf("Global memory: size %d\n", MAXGMEM);
  for(i = 0; i < MAXGMEM; i++)
  {
    printf("%d\t", gmem[i]);
    if( i == (MAXGMEM - 1) || (i + 1) % 8 == 0)
        printf(" | %d\n", i);
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
    printf("Memory: size %d (per process)\n", MAXMEM);
    for(i = 0; i < MAXPRO; ++i)
    {
        printf("Process %d: Addresses 0 - %d\n", i, MAXMEM);
        for(j = 0; j < MAXMEM; ++j)
        {
            printf("%d\t", mem[i][j]);
            if( j == (MAXMEM - 1) || (j + 1) % 8 == 0)
                printf(" | %d\n", j);
        }
        printf("\n");
    }
}

void print_mem_pages()
{
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
  printf("------------------|-----|-----|-----|\r\n");
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
}
/* end of print_pt() method */


void print_register(int reg[][REGISTERSIZE])
{
  int i, j;
  printf("--------------------------------------------------\n");
  printf("-                Register data                   -\n");
  printf("--------------------------------------------------\n");
  for(i = 0; i < MAXPRO; i++)
  {
    printf("- Process %d: ",i);
    for(j = 0; j < REGISTERSIZE; j++)
    {
      printf("%d  ", reg[i][j]);
    }
    printf("      -\n");
  }
  printf("--------------------------------------------------\n");
}

void print_stack(int stack[][STACKSIZE], int sp[])
{
  int i, j;
  for(i = 0; i < MAXPRO; i++)
  {
    printf("Stack contents for process %d\n", i);
    for(j = 0; j < STACKSIZE; j++)
      printf("%d\n", stack[i][j]);
    printf("SP at %d\n\n", sp[i]);
  }
}

#endif //_HELPERS_H_

