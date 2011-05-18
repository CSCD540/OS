/* 
 * File: pt.h
 * 
 * Author Brian Zier
 * 
 * Colored printf info:
 *   http://cc.byexamples.com/2007/01/20/print-color-string-without-ncurses/
 * And some more:
 *   http://tldp.org/LDP/LGNET/65/padala.html
 */

#ifndef _PT_H_
#define _PT_H_ 1

#include <stdio.h>
#ifndef _GLOBALS_H_
#include "globals.h"
#endif
#ifndef _EFS_H_
#include "efs.h"
#endif

int lookup(struct process pid, int vpn, int rw);
int page_fault(struct process pid, int vpn, int rw);
void init_pg_tbl();
void print_page_table();
int disk_read(struct process pid, int pageNum);

// The process table array which is indexed on the process id, and
//  contains the priority (?), and the file descriptor/filename
int processTable[MAXPRO][2];
// The page table array which contains the process id,
//  virtual page number, dirty bit, and LRU info
int pageTable[NUMPAGES][4];
int lru;

/* int lookup(int pid, int vpn, int rw) // Externally accessible method
 *
 * Description: This function takes a virtual page number for a particular
 *                process and looks it up in the page table returning the
 *                physical page number for where that page is loaded in memory.
 *                If the page is not loaded into memory, a page fault occurs and
 *                the page will be loaded, swapping out the least recently used
 *                page in memory.
 * Input:
 *        pid - The process id of the virtual page you are looking up
 *        vpn - The virtual page number
 *        rw  - Memory read/write designation (0 - read; 1 - write)
 * Output: Returns the physical page number of the virtual page in memory
 *          or -1 if the virtual page number is beyond the end of the file
 */
int lookup(struct process pid, int vpn, int rw)
{
  int found = 0;
  
  // Find the virtual page number for this process in the page table
  /*********************************/
  /* Eventually swap this out for hash table lookup */
  int i;
  for(i = 0; i < NUMPAGES; i++)
  {
    // If we find it, stop looking
    if(pageTable[i][0] == pid.pid && pageTable[i][1] == vpn)
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
    physPage = page_fault(pid, vpn, rw);
  
  
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

/* int page_fault(int pid, int vpn, int rw) // Internal method
 * Description: This function takes the virtual page number for a particular
 *                process and loads it from the disk to memory returning the
 *                physical page number of where it was loaded. The page which
 *                was previously loaded in memory, if dirty, is written back to
 *                the disk.
 * Input:
 *        pid - The process id of the virtual page you are looking up
 *        vpn - The virtual page number
 *        rw  - Memory read/write designation (0 - read; 1 - write)
 * Output: Returns the physical page number of the virtual page in memory
 *          or -1 if the virtual page number is beyond the end of the file
 */
int page_fault(struct process pid, int vpn, int rw)
{
  printf("\r\n%c[%d;%d;%dmPAGE FAULT%c[%dm\r\n", 27, 5, 37, 41, 27, 0);
  
  // find lru
  //int lru = least_recently_used();
  printf("LRU: %d\r\n", lru);
  
  // if the lru page has been used and is dirty, write it back to the disk
  if(pageTable[lru][0] == -1)
    printf("Empty page, no need to write out\r\n");
  else if(pageTable[lru][3] == 0)
    printf("Clean page, no need to write out\r\n");
  else
  {
    printf("Dirty page!\r\n");
    printf("Pretending to write existing page from memory to disk...\r\n");
    printf("Writing physical page %d to disk. PID: %d, VPN: %d\r\n",
                                  lru, pageTable[lru][0], pageTable[lru][1]);
  }
  
  // read new page in
  printf("\r\nReading new page from disk into memory...\r\n");
  //We need to read in from the file system here
  //fileNode file = getFile(pid.filename);
  FILE * fd = fopen(pid.filename, "r");
  int tmp = 0, i = 0, EOFreached = 0;
  for(; i < PAGESIZE * vpn; i++)
    if(fscanf(fd, "%d\n", &tmp) == EOF)
    {
      EOFreached = 1;
      break;
    }
  
  if(!EOFreached)
  {
    for(i = 0; i < PAGESIZE; i++)
    {
      int res = fscanf(fd, "%d\n", &tmp);
      if(res != EOF)
        mem[0][PAGESIZE * lru + i] = tmp;
      else
        mem[0][PAGESIZE * lru + i] = -1;
    }
    //  printf("First int in file: %d\r\n", tmp);
    fclose(fd);

    pageTable[lru][0] = pid.pid;
    pageTable[lru][1] = vpn;
    pageTable[lru][2] = 0;
    pageTable[lru][3] = rw;
    
    // return physical page num
    return lru;
  }
  
  return -1; // Page was beyond scope of file
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

/* init_pg_tbl() // Internal method
 * Description: This function initializes the page table to be empty (i.e. -1's)
 * Input: None
 * Output: None
 */
void init_pg_tbl()
{
  int i,j;
  for(i = 0; i < NUMPAGES; i++)
    for(j = 0; j < 4; j++)
      pageTable[i][j] = -1;
}

/* print_page_table(int vpn)
 * Description: Prints the current page table
 * Inputs: None
 * Output: None
 */
void print_page_table()
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
/* end of page_table method */


// Temporary fake disk read function
int disk_read(struct process pid, int pageNum)
{
  return 0;
}

#endif

