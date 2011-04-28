#include <stdio.h>
//#include <stdlib.h>
//#include <ctype.h>
//#include <string.h>

#define MAXPRO        1   // max num of processes
#define MAXMEM        64  // max size of a process in word/sizeof(int) bytes
#define PAGESIZE      16            // size of each page in words 2-bytes
#define NUMPAGES MAXMEM / PAGESIZE  // Number of pages in page table

int pageTable[NUMPAGES][3];   // The page table array which contains the process id, virtual page number, and LRU info
int processTable[MAXPRO][2];  // The process table array which is indexed on the process id, the priority, and the file descriptor

int lookup(int pid, int vpn);
int page_fault(int pid, int vpn);
int least_recently_used();
int print_page_table();
int disk_read(char *filename, int pageNum);

main(int argc, char *argv[])
{
  init_pg_tbl();

  print_page_table();
  
  int pid = 1;
  int vpn = 4;
  printf("\r\nLooking up virtual page number %d for process %d\r\n", vpn, pid);
  
  int physPgNm = lookup(pid, vpn);
  
  printf("\r\nPhysical page number is: %d\r\n", physPgNm);
  
  print_page_table();
}

int lookup(int pid, int vpn)
{
  int found = 0;
  
  // Find the virtual page number for this process in the page table
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
  
  // Increase every page's recently used counter
  int j;
  for(j = 0; j < NUMPAGES; j++)
  {
    if(pageTable[j][0] != -1)
      pageTable[j][2]++;
  }
    
  if(found == 1) // If we found it, update LRU info and return the physical page number
  {    
    // Reset this page to be most recently used
    pageTable[i][2] = 0;
    
    return i;
  }
  else // If we did not find it, we page fault
    return page_fault(pid, vpn);
}

int page_fault(int pid, int vpn)
{
  printf("\r\nPAGE FAULT\r\n");
  
  // find lru
  int lru = least_recently_used();
  printf("LRU: %d\r\n", lru);
  
  // write lru page to disk
  printf("Pretending to write existing page to disk...\r\n");
  
  // read new page in
  printf("Pretending to read new page from disk...\r\n");
  pageTable[lru][0] = pid;
  pageTable[lru][1] = vpn;
  pageTable[lru][2] = 0;
  
  // return physical page num
  
  return lru;
}

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
  printf("Page Table:\r\n");
  printf("            pid  vpn  lru\r\n");
  printf("            ---  ---  ---\r\n");
  int i;
  for(i = 0; i < NUMPAGES; i++)
    printf("PysPage %2d: %3d  %3d  %3d\r\n", i, pageTable[i][0],  pageTable[i][1], pageTable[i][2]);
}
/* end of page_table method */


int disk_read(char *filename, int pageNum)
{
  return 0;
}



