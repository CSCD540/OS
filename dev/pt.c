#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXPRO        1   // max num of processes
#define MAXMEM        64  // max size of a process in word/sizeof(int) bytes
#define PAGESIZE      16            // size of each page in words 2-bytes
#define NUMPAGES MAXMEM / PAGESIZE  // Number of pages in page table

int pageTable[NUMPAGES][3];   // The page table array which contains the process id, virtual page number, and LRU info
int processTable[MAXPRO][2];  // The process table array which is indexed on the process id, the priority, and the file descriptor

int page_table(int vpn);

main(int argc, char *argv[])
{
  int i,j;
  for(i = 0; i < NUMPAGES; i++)
    for(j = 0; j < 3; j++)
      pageTable[i][j] = -1;
  
  page_table(NUMPAGES - 1);
}

/* Signature: page_table(int vpn)
 * Desc: Takes a virtual page number and '''handles paging''' to return a physical page number
 * Inputs: Virtual page number (int)
 * Output: Physical page number (int)
 */
int page_table(int vpn)
{
  printf("Virtual page num: %d\r\nPage Table:\r\n", vpn);
  printf("            pid  va  lru\r\n");
  printf("            ---  --  ---\r\n");
  int i;
  for(i = 0; i < NUMPAGES; i++)
    printf("PysPage %2d: %3d  %2d  %3d\r\n", i, pageTable[i][0],  pageTable[i][1], pageTable[i][2]);
}
/* end of page_table method */

