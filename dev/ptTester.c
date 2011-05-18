/* 
 * File: ptTester.c
 * 
 * Author Brian Zier
 * 
 * 
 */

#include "pt.h"
#include "helpers.h"

void accessPage(struct process pid, int vpn);
//void init_main_mem();
//void print_mem();

int main(int argc, char *argv[])
{
  char filename[] = "testingPT.out";
  struct process pid0, pid1, pid2, pid3;
  pid0.filename = filename;
  pid0.pid = 0;
  pid1.filename = filename;
  pid1.pid = 1;
  pid2.filename = filename;
  pid2.pid = 2;
  pid3.filename = filename;
  pid3.pid = 2;
  
  init_mem();
  init_pg_tbl();
  
  print_mem();
  
  //print_page_table();
  printf("\r\n--------------------------------------------------------\r\n");

  accessPage(pid1, 4);
  accessPage(pid2, 3);  
  accessPage(pid1, 2);
  accessPage(pid1, 4);
  accessPage(pid0, 6);
  accessPage(pid3, 6);
  
  return 0;
}

void accessPage(struct process pid, int vpn)
{
  printf("\r\nLooking up virtual page number %d for process %d\r\n", vpn, pid.pid);
  
  print_page_table();
  
  int physPgNm = lookup(pid, vpn, 0);
  
  printf("\r\nPhysical page number is: %d\r\n", physPgNm);
  
  print_mem();
  
  printf("\r\n--------------------------------------------------------\r\n");
}

/*void init_main_mem()
{
  int i,j;
  for(i = 0; i < MAXPRO; i++)
    for(j = 0; j < MAXMEM; j++)
      mem[i][j] = -1;
}//*/

/*void print_mem()
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
}//*/

