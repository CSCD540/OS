/* 
 * File: ptTester.c
 * 
 * Author Brian Zier
 * 
 * 
 */

#include "pt.h"

void accessPage(int pid, int vpn);

main(int argc, char *argv[])
{
  init_pg_tbl();

  print_page_table();
  printf("\r\n-------------------------\r\n");
  
  accessPage(1, 4);
  accessPage(2, 3);
  accessPage(1, 2);
  accessPage(1, 4);
  accessPage(0, 6);
  accessPage(3, 6);
}

void accessPage(int pid, int vpn)
{
  printf("\r\nLooking up virtual page number %d for process %d\r\n", vpn, pid);
  
  int physPgNm = lookup(pid, vpn, 0);
  
  printf("\r\nPhysical page number is: %d\r\n", physPgNm);
  
  print_page_table();
  printf("\r\n-------------------------\r\n");
}

