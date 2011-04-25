#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAXMEM 64 //max word size of a process (2-bytes)
#define PAGESIZE 16 //size of each page in words 2-bytes
#define NUMPAGES MAXMEM/PAGESIZE //Number of pages in page table

int page_table(int vpn);

main(int argc, char *argv[])
{
    page_table(NUMPAGES);
}

/* Signature: page_table(int vpn)
 * Desc: Takes a virtual page number and '''handles paging''' to return a physical page number
 * Inputs: Virtual page number (int)
 * Output: Physical page number (int)
 */
int page_table(int vpn)
{
    printf("Test: %d\r\n", vpn);
}
/* end of page_table method */

