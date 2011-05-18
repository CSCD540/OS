#ifndef _GLOBALS_H_
#include "globals.h"
#endif

void init_gmem();
void init_mem();
void init_reg();
void reset_memory();

void print_gmem();
void print_mem();
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
    gmem[i] = 0;
}

void init_mem()
{
  int i,j;
  
  for(i = 0; i < MAXPRO; i++)
	  for(j = 0; j < MAXMEM; j++)
      mem[i][j] = 0;
}

void init_reg()
{
  int i,j;
  
  for(i = 0; i < MAXPRO; i++)
	  for(j = 0; j < REGISTERSIZE; j++)
      reg[i][j] = 0; 
}

void reset_memory()
{
	int i, j;

	for(i = 0; i < MAXPRO; i++)
		for(j = 0; j < REGISTERSIZE; j++)
			reg[i][j]=0;
	
	for(i = 0; i < MAXGMEM; i++)
		gmem[i] = 0;
		
	for(i = 0; i < MAXPRO; i++)
	  for(j = 0; j < MAXMEM; j++)
      mem[i][j] = 0;
}


/*  ----------------------------------------------------
    Debug routines 
    ----------------------------------------------------*/
void print_gmem()
{
  int i;
  printf("\r\n--------------------\r\n|%c[%d;%dm   GLOBAL MEMORY  %c[%dm|\r\n|------------------|\r\n", 27, 1, 42, 27, 0);
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
    printf("\r\n------------------\r\n|%c[%d;%dm   MAIN MEMORY  %c[%dm|\r\n|----------------|\r\n", 27, 1, 42, 27, 0);
    printf("Memory: size %d (per process)\n", MAXMEM);
    for(i = 0; i < MAXPRO; ++i)
    {
        printf("Process %d: Addresses 0 - %d with %d pages of size %d\n", i, MAXMEM, NUMPAGES, PAGESIZE);
        for(j = 0; j < MAXMEM; ++j)
        {
            printf("%d\t", mem[i][j]);
            if( j == (MAXMEM - 1) || (j + 1) % PAGESIZE == 0)
                printf(" | %d\n", j);
        }
        printf("\n");
    }
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

