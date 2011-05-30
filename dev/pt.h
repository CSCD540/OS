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

#include <stdio.h>

#ifndef _GLOBALS_H_
#include "globals.h"
#endif

#ifndef _HELPERS_H_
#include "helpers.h"
#endif

// #include "efs.h"

int lookup_addr(int vip, int cur_proc, int rw);
int lookup_ip(struct process proc, int rw);
int lookup(struct process proc, int vpn, int rw);
int page_fault(struct process proc, int vpn);//, int rw);

/* int lookup_addr(int vip, int rw) // Externally accessible method
 *
 * Description: This function calculates the virtual page number of the instruction
 *              then calls lookup(int proc, int vpn, int rw)
 * Input:
 *        vip - The virtual address you are looking up
 *        cur_proc - the process you are currently in
 *        rw  - Memory read/write designation (0 - read; 1 - write)
 * Output: Returns the physical instruction number to run
 *          or -1 if the virtual page number is beyond the end of the file
 */
int lookup_addr(int vip, int cur_proc, int rw)
{
  int offset = 0;
  int page = 0;
  int vpn = 0;
  
  vpn = (vip)>>pageBits;
  offset = (vip) % PAGESIZE;
  page = (lookup(processes[cur_proc], vpn, rw)) << pageBits; //Left shift it back
  if(DEBUG) 
  { 
    printf("vip %d\n", vip);
    printf("Process Offset %d\n", processes[cur_proc].poffset);
    printf("vpn %d\n", vpn);
    printf("Offset %d\n", offset);
    printf("Page %d\n", page>>pageBits);
    printf("Physical Address %d\n", page | offset);
    print_mem();
  }
  
  if(page == ENDF)
  {
    printf("ERROR: Page not found.\n");
    return OUT_OF_RANGE;//Error
  }
  return (page | offset);
}

/* int lookup(int proc, int rw) // Externally accessible method
 *
 * Description: This function calculates the virtual page number of the instruction
 *              then calls lookup(int proc, int vpn, int rw)
 * Input:
 *        proc - The process of the virtual page you are looking up
 *        rw  - Memory read/write designation (0 - read; 1 - write)
 * Output: Returns the physical instruction number to run
 *          or -1 if the virtual page number is beyond the end of the file
 */
int lookup_ip(struct process proc, int rw)
{
  int vpn = 0;
  
  vpn = (proc.poffset + proc.ip)>>pageBits;
  proc.offset = (proc.poffset + proc.ip) % PAGESIZE;
  proc.page = (lookup(proc, vpn, rw)) << pageBits; //Left shift it back
  if(DEBUG) 
  { 
    printf("vip %d\n", proc.ip);
    printf("Process Offset %d\n", proc.poffset);
    printf("vpn %d\n", vpn);
    printf("Offset %d\n", proc.offset);
    printf("Page %d\n", proc.page>>pageBits);
    printf("Physical Address %d\n", proc.page | proc.offset);
  }
  
  if(proc.page == ENDF)
  {
    printf("ERROR: Page not found.\n");
    return OUT_OF_RANGE;//Error
  }
  return (proc.page | proc.offset);
}

// The process table array which is indexed on the process id, and
//  contains the priority (?), and the file descriptor/filename
//int processTable[MAXPRO][2];


/* int lookup(int proc, int vpn, int rw) // Externally accessible method
 *
 * Description: This function takes a virtual page number for a particular
 *                process and looks it up in the page table returning the
 *                physical page number for where that page is loaded in memory.
 *                If the page is not loaded into memory, a page fault occurs and
 *                the page will be loaded, swapping out the least recently used
 *                page in memory.
 * Input:
 *        proc - The process id of the virtual page you are looking up
 *        vpn - The virtual page number
 *        rw  - Memory read/write designation (0 - read; 1 - write)
 * Output: Returns the physical page number of the virtual page in memory
 *          or -1 if the virtual page number is beyond the end of the file
 */
int lookup(struct process proc, int vpn, int rw)
{
  int found = 0;
  
  // Find the virtual page number for this process in the page table
  /*********************************/
  /* Eventually swap this out for hash table lookup */
  int i;
  for(i = 0; i < NUMPAGES; i++)
  {
    // If we find it, stop looking
    if(pageTable[i][0] == proc.pid && pageTable[i][1] == vpn)
    {
      found = 1;
      break;
    }
  }
  /*********************************/
  
  
  int physPage = -1;

  if(found == 1) // If we found it, great...
  {
    if(DEBUG)
      printf("\n%c[%d;%d;%dmPage Found%c[%dm\n", 27, 1, 37, 44, 27, 0);
    physPage = i;
  }
  else // If we did not find it, we page fault
    physPage = page_fault(proc, vpn);
  
  // Reset this page to be most recently used
  pageTable[physPage][2] = -1;
  // Set the dirty bit
  if(pageTable[physPage][3] != 1)
    pageTable[physPage][3] = rw;
  
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
  
  return physPage;
}

/* int page_fault(int proc, int vpn, int rw) // Internal method
 * Description: This function takes the virtual page number for a particular
 *                process and loads it from the disk to memory returning the
 *                physical page number of where it was loaded. The page which
 *                was previously loaded in memory, if dirty, is written back to
 *                the disk.
 * Input:
 *        proc - The process id of the virtual page you are looking up
 *        vpn - The virtual page number
 *        rw  - Memory read/write designation (0 - read; 1 - write)
 * Output: Returns the physical page number of the virtual page in memory
 *          or -1 if the virtual page number is beyond the end of the file
 */
int page_fault(struct process proc, int vpn)
{

  if (PT_DBG_LVL > 0) printf("\n%c[%d;%d;%dmPAGE FAULT%c[%dm\n\n", 27, 1, 37, 41, 27, 0);

  // find lru
  //int lru = least_recently_used();
  if (PT_DBG_LVL > 1) printf("LRU: %d\n", lru);
  
  // if the lru page has been used and is dirty, write it back to the disk
  if(pageTable[lru][0] == -1)
  {  if(PT_DBG_LVL > 2) printf("Empty page, no need to write out\n");}

  else if(pageTable[lru][3] == 0)
  {  if(PT_DBG_LVL > 2) printf("Clean page, no need to write out\n");}

  else
  {
    if(PT_DBG_LVL > 2) printf("Dirty page!\n");
    
    struct fileNode * dirtyFile = get_file(proc.filename);
    
    if(PT_DBG_LVL > 3)
    {
      printf("\nFile before writing the dirty page back to disk...\n");
      print_block_list(dirtyFile->blockList);
    }
    
    int k = 0;
    int dirtyData[PAGESIZE];
    for(; k < PAGESIZE; k++)
    {
      dirtyData[k] = mem[0][PAGESIZE * lru + k];
      //printf("dirtyData[%d] = mem[0][%d] : %d\n", k, PAGESIZE * lru + k, mem[0][PAGESIZE * lru + k]);
    }
    
    //printf("dirtyFname: %s, dd[0]: %d, dd[15]: %d, pagesize: %d, mode: OVERWRITE, offset: %d\n", dirtyFile->filename, dirtyData[0], dirtyData[15], PAGESIZE, PAGESIZE * pageTable[lru][1]);
    write(&dirtyFile, dirtyData, PAGESIZE, OVERWRITE, PAGESIZE * pageTable[lru][1]);
    
    if(PT_DBG_LVL > 3)
    {
      printf("\nFile after writing the dirty page back to disk...\n");
      print_block_list(dirtyFile->blockList);
    }
    
    pageTable[lru][3] = 0;
    /*
    printf("Writing physical page %d to disk. PID: %d, VPN: %d\n",
                                  lru, pageTable[lru][0], pageTable[lru][1]);
    */
  }
  
  if(PT_DBG_LVL > 2) printf("\nReading new page from virtual disk into memory...\n");
  
  // Open the file for reading
  int fd = open(proc.filename);
  if(fd < 0)
  {
    // BAD FD, return something useful...
    ;
  }
  
  // Skip to the right spot (aka seek)
  int tmp = 0, i = 0, EOFreached = 0;
  for(; i < PAGESIZE * vpn; i++)
  {
    if((tmp = read(fd)) == ENDF)
    {
      EOFreached = 1;
      break;
    }
  }
  
  // As long as we didn't pass the end of the file already
  //   Read the page into memory
  if(!EOFreached)
  {
    int res;
    for(i = 0; i < PAGESIZE && (res = read(fd)) != ENDF; i++)
      mem[0][PAGESIZE * lru + i] = res;
    
    // Close the file to free space in the file descriptor table
    close(fd);
    
    // Fill the rest of the page with -1
    while(i < PAGESIZE)
    {
      mem[0][PAGESIZE * lru + i] = -1;
      i++;
    }
    
    pageTable[lru][0] = proc.pid;
    pageTable[lru][1] = vpn;
    pageTable[lru][2] = 0;
    
    if (PT_DBG_LVL > 0) printf("Page read into memory\n\n");
    
    // return physical page num
    return lru;
  }
  
  return ENDF; // Page was beyond scope of file
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


