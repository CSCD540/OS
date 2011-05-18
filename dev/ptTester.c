/* 
 * File: ptTester.c
 * 
 * Author Brian Zier
 * 
 * 
 */

//#include "efs.h"
#include "shell.h"
#include "pt.h"

//int save_file(char *filename);
//int write(struct fileNode **fileListNode, int data[], int count, int writeMode, int offset);
void write2Page(int pid, int vpn);
void accessPage(int pid, int vpn);
//void init_main_mem();

main(int argc, char *argv[])
{
  struct process * firstProc = malloc(sizeof(struct process) + sizeof(char[50]));
  firstProc->pid = 0;
  firstProc->filename = "testingPT.out";
  printf("First Process - id: %d; filename: %s\n", firstProc->pid, firstProc->filename);
  processTable[0] = firstProc;
  
  init_disk(disk);
  int status = save_file("testingPT.out");
  printf("Save file testingPT.out to vfs status: %d\n", status);
  
  print_disk(disk);
  init_mem();
  init_pt();
  
  print_mem_pages();
  
  printf("\r\n--------------------------------------------------------\r\n");
  
  accessPage(0, 0);
  accessPage(0, 3);
  write2Page(0, 3);
  accessPage(0, 2);
  accessPage(0, 4);
  accessPage(0, 7);
  //accessPage(0, 3);
  accessPage(0, 2);
  accessPage(0, 5);
  accessPage(0, 6);
  accessPage(0, 3);
}

void write2Page(int pid, int vpn)
{
  printf("\r\nLooking up virtual page number %d for process %d\r\n", vpn, pid);
  
  print_pt();
  
  int physPgNm = lookup(pid, vpn, 1);
  mem[0][PAGESIZE * physPgNm] = 1024;
  
  printf("\r\nPhysical page number is: %d\r\n", physPgNm);
  
  print_mem_pages();
  
  printf("\r\n--------------------------------------------------------\r\n");
}

void accessPage(int pid, int vpn)
{
  printf("\r\nLooking up virtual page number %d for process %d\r\n", vpn, pid);
  
  print_pt();
  
  int physPgNm = lookup(pid, vpn, 0);
  
  printf("\r\nPhysical page number is: %d\r\n", physPgNm);
  
  print_mem_pages();
  
  printf("\r\n--------------------------------------------------------\r\n");
}
/*
void init_main_mem()
{
  int i,j;
  for(i = 0; i < MAXPRO; i++)
    for(j = 0; j < MAXMEM; j++)
      mem[i][j] = -1;
}
*/

