/* 
 * File: pt.h
 * 
 * Author Brian Zier
 * 
 * Colored printf info: http://cc.byexamples.com/2007/01/20/print-color-string-without-ncurses/
 * And some more: http://tldp.org/LDP/LGNET/65/padala.html
 */

#include <stdio.h>
#include "efs.h"

#define MAXPRO        1   // max num of processes
#define MAXMEM        64  // max size of a process in word/sizeof(int) bytes
#define PAGESIZE      16            // size of each page in words 2-bytes
#define NUMPAGES MAXMEM / PAGESIZE  // Number of pages in page table

int pageTable[NUMPAGES][3];   // The page table array which contains the process id, virtual page number, and LRU info
int lru;
int processTable[MAXPRO][2];  // The process table array which is indexed on the process id, the priority, and the file descriptor


int lookup(int pid, int vpn)
{
  int found = 0;
  
  // Find the virtual page number for this process in the page table
  /*********************************/
  /* Eventually swap this out for hash table lookup */
  int i;
  for(i = 0; i < NUMPAGES; i++)
  {
    // If we find it, stop looking
    if(pageTable[i][0] == pid && pageTable[i][1] == vpn)
    {
      found = 1;
      break;
    }
  }
  /*********************************/
  
  
  int physPage = -1;
  
  if(found == 1) // If we found it, great...
  {
    printf("\r\n%c[%d;%d;%dmPage Found%c[%dm\r\n", 27, 1, 37, 44, 27, 0);
    physPage = i;
  }
  else // If we did not find it, we page fault
    physPage = page_fault(pid, vpn);
  
  
  // Increase every page's recently used counter
  // And update the LRU index
  int j, max = -1, emptyFound = 0;
  for(j = 0; j < NUMPAGES; j++)
  {
    if(pageTable[j][0] == -1)
    {
      if(!emptyFound)
      {
        lru = j;
        emptyFound = 1;
      }
    }
    else if(++pageTable[j][2] > max && !emptyFound)
    {
      lru = j;
      max = pageTable[j][2];
    }
  }
  
  // Reset this page to be most recently used
  pageTable[physPage][2] = 0;
  
  return physPage;
}


int page_fault(int pid, int vpn)
{
  printf("\r\n%c[%d;%d;%dmPAGE FAULT%c[%dm\r\n", 27, 5, 37, 41, 27, 0);
  
  // find lru
  //int lru = least_recently_used();
  printf("LRU: %d\r\n", lru);
  
  // write lru page to disk
  if(pageTable[lru][0] != -1)
  {
    printf("Pretending to write existing page from memory to disk...\r\n");
    printf("Writing physical page %d to disk. PID: %d, VPN: %d\r\n", lru, pageTable[lru][0], pageTable[lru][1]);
  }
  else
    printf("Empty page, no need to write out\r\n");
  
  // read new page in
  printf("\r\nPretending to read new page from disk into memory...\r\n");
  pageTable[lru][0] = pid;
  pageTable[lru][1] = vpn;
  pageTable[lru][2] = 0;
  
  // return physical page num
  return lru;
}

/*
 * Old LRU search
 * Now keeping track of it as we go instead
 *   of having to look for it every time
 *
// Returns the physical page number of the least recently used page
//  OR the first empty page
int least_recently_used()
{
  int i, max = -1, lru = 0;
  for(i = 0; i < NUMPAGES; i++)
    if(pageTable[i][0] == -1)
      return i;
    else if(pageTable[i][2] > max)
    {
      lru = i;
      max = pageTable[i][2];
    }

  return lru;
}
 *
 */

init_pg_tbl()
{
  int i,j;
  for(i = 0; i < NUMPAGES; i++)
    for(j = 0; j < 3; j++)
      pageTable[i][j] = -1;
}

/* Signature: page_table(int vpn)
 * Desc: Takes a virtual page number and '''handles paging''' to return a physical page number
 * Inputs: Virtual page number (int)
 * Output: Physical page number (int)
 */
int print_page_table()
{
  printf("Page Table (%c[%dmLRU in red%c[%dm):\r\n", 27, 31, 27, 0);
  printf("            pid  vpn  lru\r\n");
  printf("            ---  ---  ---\r\n");
  int i;
  for(i = 0; i < NUMPAGES; i++)
    if(i == lru)
    {
      printf("%c[%d;%dm", 27, 0, 31);
      printf("PysPage %2d: %3d  %3d  %3d\r\n", i, pageTable[i][0],  pageTable[i][1], pageTable[i][2]);
      printf("%c[%dm", 27, 0);
    }
    else
      printf("PysPage %2d: %3d  %3d  %3d\r\n", i, pageTable[i][0],  pageTable[i][1], pageTable[i][2]);
}
/* end of page_table method */


// Temporary fake disk read function
int disk_read(char *filename, int pageNum)
{
  return 0;
}



