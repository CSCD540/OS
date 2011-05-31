// #include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "easm.tab.h"
#include "globals.h"
#include "helpers.h"
#include "shell.h"
#include "pt.h"
// #include "efs.h"

// Methods declaration
int  exe(int stack[][STACKSIZE],int sp[], int next_instruct[], int cur_proc, int *terminate);
void executeit();
void grab_data(int index,int *grabdata);
int  peek(int stack[][STACKSIZE], int proc_id, int sp[], int offset);
void push(int stack[][STACKSIZE], int proc_id, int sp[],int data, int calledfrom);
int  pop(int stack[][STACKSIZE], int proc_id, int sp[], int calledfrom);
int  new_process(char * filename);
//end Methods declaration

int echoCmd = 0; // Echo the command on the shell back to the user?

int main(int argc, char *argv[])
{
  init_disk(disk);
  init_mem();
  init_gmem();
  init_pt();
  
  int proc_loaded = 0; //Number of processes loaded
  
  /* 
   * Shell command 
   * Commands: save,del,ls,exit,run,help
  */	
  memset(processes, 0, MAXPROGRAMS * sizeof(struct process));
  
  //Load a program from the disk
  if(argc > 1)
    load_program(argv[1]);
  
  int status;
  while(machineOn)
  {
    printf("evm$ ");

    fgets(input,sizeof(input),stdin);

    input[strlen(input)-1] = 0;

    // split the cmd and arg(file name)
    cmd = strtok(input, " ");
    arg1= strtok('\0', " ");
    
    // If echo is on, print out the command and argument
    if(echoCmd && !(strcmp(cmd,"echo")==0 && strcmp(arg1,"off")==0))
      if(arg1 != NULL)
        printf("%s %s\n", cmd, arg1);
      else
        printf("%s\n", cmd);
    
    if(strlen(input)<1)
    {
      continue; // When there is no input string, skipping everything.
    }
    else if(strcmp(cmd, "ls")==0) 
    {
      list_directory_contents(arg1);
    }
    else if(strcmp(cmd, "cat")==0)
    {
      concatenate(arg1);
    }
    else if(strcmp(cmd, "debug")==0)
    {
    }
    else if(strcmp(cmd, "diskdump")==0)
    {
      print_disk(disk);
    }
    else if(strcmp(cmd, "echo")==0)
    {
      if(arg1 != NULL)
      {
        if(strcmp(arg1, "on")==0)
        {
          echoCmd = 1;
          printf("echo on\n");
        }
        else if(strcmp(arg1, "off")==0)
          echoCmd = 0;
        else
          printf("echo on|off\n");
      }
      else
        printf("echo on|off\n");
    }
    else if(strcmp(cmd, "exit")==0)
    {
      show_exit();
    }
    else if(strcmp(cmd, "fdisk")==0)
    {
        init_disk(disk);
    }
    else if(strcmp(cmd, "filedump")==0)
    {
      dump_file(arg1);
    }
    else if(strcmp(cmd, "help")==0)
    {
      show_help();
    }
    //This should load from outside the VM
    else if(strcmp(cmd, "hdload")==0) 
    {
      load_program(arg1);
    }
    //This should load from VM filesystem
    else if(strcmp(cmd, "load")==0) 
    {
      proc_loaded = new_process(arg1);
      if(proc_loaded < 0)
        printf("Error on allocating process\n");
      else
        lookup_ip(processes[proc_loaded], 0);
    }
    else if(strcmp(cmd, "man")==0)
    {
    }
    else if(strcmp(cmd, "memdump")==0)
    {
        print_mem();
    }
    else if(strcmp(cmd, "piddump")==0)
    {
        print_processes();
    }
    else if(strcmp(cmd, "ptdump")==0)
    {
        print_pt();
    }
    else if(strcmp(cmd, "pwd")==0)
    {
      printf("/\n");
    }
    else if(strcmp(cmd, "rm")==0)
    {
      remove_file(arg1);
    }
    else if(strcmp(cmd, "run")==0)
    {
      executeit(proc_loaded);
    }
    else if(strcmp(cmd, "save")==0)
    {
      status = save_file(arg1);
      if(status != SUCCESS) print_error(status);
    }
    else if(strcmp(cmd, "showGlobalMem")==0)
    {
      print_gmem();
    }
    else if(strcmp(cmd, "showLRU")==0)
    {
      print_lru();
    }
    else if(strcmp(cmd, "showPage")==0)
    {
      if(arg1 != NULL)
      {
        int pgNum = atoi(arg1);
        
        status = print_page(pgNum);
        if(status != SUCCESS) print_error(status);
      }
      else
        printf("SYNTAX: showPage page_number\n"); 
    }
    else
		{
      printf("Command Not Found\n");
    }
  } // end while(machineOn)
  
  free(freeBlockList);
  free(fileList);
  
  return 0;
}

/* void executeit()
 * Description: This function runs all the program in processes[] then returns
 * Input: None
 * Output: None
 */
void executeit()
{
  int cur_proc = 0, msg=-1;
  int stack[MAXPRO][STACKSIZE]; //Stack pre process
  int sp[MAXPRO]; //Stack pointer per process
  int next_instruct[MAXPRO];  //Next_instruction per process
  //int proc_complete[MAXPRO];  //Is the process done?
  int locked=UNLOCKED;
  int terminate = 0;

  //Clear the stack, registers and reset the stack pointers
  memset(stack, 0, MAXPRO*STACKSIZE*sizeof(int));
  memset(sp, -1, MAXPRO*sizeof(int));
  memset(reg, 0, 10 * MAXPRO * sizeof(int));
  //memset(proc_complete, 0, MAXPRO*sizeof(int));
  
  srand( time(NULL) );

  //Set the first instruction to 10
  int i = 0;
  for(;i<MAXPRO;i++)
    next_instruct[i]=10;

  keyhit(54);
  cur_proc = 0;
  while(1)
  {
    cont:
      keyhit(55); 
      if (HALTED == TERMINATED)
      {
        HALTED = NOT_FINISHED;
        break;
      }
      if(locked == UNLOCKED)
      {
        if(processes[cur_proc].status == RUNNING)
          processes[cur_proc].status = READY;
          
        do{//Select a new process to run
          cur_proc = (cur_proc + 1) % curProcesses;//rand() % curProcesses; //Find one of the programs to run  
        }while(processes[cur_proc].state == TERMINATED);//Only use non-terminated processes
      }
      
      next_instruct[cur_proc] = lookup_ip(processes[cur_proc], 0);
      if(next_instruct[cur_proc] < 0) //Can't have a negative IP
        return;
        
      if(mem[0][next_instruct[cur_proc]] <= -1)//-1 means we've reached the end of the program
      {
        terminate = TERMINATED;
        processes[cur_proc].state = TERMINATED;
        locked = UNLOCKED;
      }
        
      if(processes[cur_proc].status == TERMINATED)
      {
        if (DEBUG)
          printf("----------------------------cur_proc: %d\n",cur_proc);
        goto checkdone;
      }
      
      if(next_instruct[cur_proc] < 256) // safe guard
      {
        if(processes[cur_proc].status == READY)
        {
          processes[cur_proc].status = RUNNING;
          next_instruct[cur_proc];
          msg = exe(stack, sp, next_instruct, cur_proc, &terminate);
        }
        else
        {
          //Wait for IO to return
          //If we pick a number less than 10 out of 1000 the IO returned and set status to 0 ready
          processes[cur_proc].status = READY;
        }
                
        if(msg == ENDPROCESS || msg == ENDPROGRAM || terminate == TERMINATED)
        {
          processes[cur_proc].state = TERMINATED; //Terminated
          if(ENDPROGRAM == msg)//End all the programs associated with this process
          {
            for(i = 0; i< curProcesses; i++)//If at least one process is still running
              if(strcmp(processes[i].filename, processes[cur_proc].filename) == 0)
                processes[i].state = TERMINATED;
          }
          goto checkdone;
        }
        
        if(msg==UNLOCKED)
          locked=UNLOCKED;
        else if(msg==LOCKED || locked==LOCKED)
          locked = LOCKED;
          
      }
      else
      {
        printf("Process %d complete, terminate = %d\n",cur_proc, terminate);
        processes[cur_proc].state = TERMINATED;
      }
      // check if all processes are done
    checkdone:
      for(i = 0; i< curProcesses; i++)//If at least one process is still running
        if(processes[i].state != TERMINATED)
        {
          terminate = NOT_FINISHED;
          goto cont;
        }
      break;
  }
}

int exe(int stack[][STACKSIZE], int sp[], int next_instruct[], int cur_proc, int *terminate)
{
  int i, k, m; // delete these after all accesses renamed, except i
  int diff, tmp, tmp1, tmp2;
  int real_inst;
  char name[11];

  char * indent = "\t\t\t\t\t";
  i = next_instruct[cur_proc];
  diff = i;
  real_inst = i;
  if(0 < cur_proc % 3)printf("%s", indent);
  if(2 ==cur_proc % 3)printf("%s", indent);
  printf("(pid=%d) IP = %d:  ", cur_proc, processes[cur_proc].ip);
  printf("mem[0][%d] = %d\n", i, mem[0][i] );
  if(0 < cur_proc % 3)printf("%s", indent);
  if(2 ==cur_proc % 3)printf("%s", indent);


  if (DEBUG)	
    printf("In Main Memory: process %d, program counter %d, content:%d\n\n", cur_proc, i, mem[0][i]);

  switch (mem[0][i])
  {
    /** OPEN, READ, CLOSE, WRITE, SEEK ::  OS services **/
    case OPEN :
        if (DBGCPU) printf("Open file\n");
        processes[cur_proc].state = WAITING; //Set to waiting on IO
        break;
        
    case READ :
        if (DBGCPU) printf("Read file\n");
        processes[cur_proc].state = WAITING; //Set to waiting on IO
        break;

    case CLOSE :
        if (DBGCPU) printf("Close file\n");
        break;

    case WRITE :
        if (DBGCPU) printf("Open file\n");
        processes[cur_proc].state = WAITING; //Set to waiting on IO
        break;

    case SEEK :
          tmp = peek(stack, cur_proc, sp, 0) ;
          if (DBGCPU) printf("SEEK offset=  0,  data=%d\n", tmp);
          processes[cur_proc].state = WAITING; //Set to waiting on IO
          tmp1 = peek(stack, cur_proc, sp, -1) ;
          if (DBGCPU)
          {
						printf("SEEK offset= -1,  fd =%d\n", tmp1); 
						printf("OS service call  --- <SEEK> \n");
					}
        break;

    case POPD : //This takes one argument (The next 'instruction' is the register to pop into)
          tmp1 = pop(stack, cur_proc, sp, 10) ;
          if((i % PAGESIZE) == PAGESIZE-1) // This is the Boundary between every page.
          {
            next_instruct[cur_proc] = lookup_addr(next_instruct[cur_proc] + 1, cur_proc, 0); 
            i = next_instruct[cur_proc] - 1;
          }
		      tmp = mem[0][i+1];
          if(DBGCPU) printf("POPD %d into %d\n", tmp1, tmp);
          
          gmem[tmp] = tmp1; 
          next_instruct[cur_proc]++;
          processes[cur_proc].ip++;
          break;

    case POP : 
          tmp1 = pop(stack, cur_proc, sp, 12);
          if(DBGCPU) printf("POP %d into nowhere\n",tmp1);
          break;

    case LD :
          tmp = pop(stack, cur_proc, sp, 14);
          tmp1 = gmem[tmp];
          if(DBGCPU) printf("LD %d from %d\n",tmp1, tmp);
          push(stack, cur_proc, sp, tmp1, 15);
          break;

    case LA : 
          if((i % PAGESIZE) == PAGESIZE-1) // This is the Boundary between every page.
          {
            next_instruct[cur_proc] = lookup_addr(next_instruct[cur_proc] + 1, cur_proc, 0); 
            i = next_instruct[cur_proc] - 1;
          }
          
          tmp = mem[0][i+1];
          if(DBGCPU) printf("LA %d\n",tmp);
          
          push(stack, cur_proc, sp, tmp, 17);
          next_instruct[cur_proc]++;
          processes[cur_proc].ip++;
          break;

    case LOAD : //Loads the gmem address at tmp
          if((i % PAGESIZE) == PAGESIZE-1) // This is the Boundary between every page.
          {
            next_instruct[cur_proc] = lookup_addr(next_instruct[cur_proc] + 1, cur_proc, 0); 
            i = next_instruct[cur_proc] - 1;
          }
          
          tmp = mem[0][i+1];
          
          tmp1 = gmem[tmp];
          
          if(DBGCPU)printf("LOAD %d from index %d\n", tmp1, tmp);
          push(stack, cur_proc, sp, tmp1, 19);
          next_instruct[cur_proc]++;
          processes[cur_proc].ip++;
          break;

    case LOADI : //Load the constant to the stack
          if((i % PAGESIZE) == PAGESIZE-1) // This is the Boundary between every page.
          {
            next_instruct[cur_proc] = lookup_addr(next_instruct[cur_proc] + 1, cur_proc, 0); 
            i = next_instruct[cur_proc] - 1;
          }
          push(stack, cur_proc, sp, mem[0][i + 1], 21);
          if(DBGCPU)printf("LOADI and Push:%d\n",mem[0][i+1]);
          next_instruct[cur_proc]++;
          processes[cur_proc].ip++;
          break;

    case STOR : //Stor at the gmem index indicated 
          tmp = pop(stack, cur_proc, sp, 68);
          if((i % PAGESIZE) == PAGESIZE-1) // This is the Boundary between every page.
          {
            next_instruct[cur_proc] = lookup_addr(next_instruct[cur_proc] + 1, cur_proc, 0); 
            i = next_instruct[cur_proc] - 1;
          }

          tmp1 = mem[0][i+1];
          
          gmem[tmp1] = tmp;
          if (DBGCPU) printf("STOR gmem[%d] = %d\n", tmp1, gmem[tmp1]);
          
          next_instruct[cur_proc]++;
          processes[cur_proc].ip++;
          break;

    case ST :
          tmp = pop(stack, cur_proc, sp, 70); // does ST ever need to store in a register?
          tmp1 = pop(stack, cur_proc, sp, 72);
          gmem[tmp] = tmp1;
          if(DBGCPU) printf("ST %d into gmem[%d]\n",tmp1, tmp);
          break;
          
    case JFALSE : //Conditional Jump to the address specified  (needs pt translation)
          tmp = pop(stack, cur_proc, sp, 74);
          if((i % PAGESIZE) == PAGESIZE-1) // This is the Boundary between every page.
          {
            next_instruct[cur_proc] = lookup_addr(next_instruct[cur_proc] + 1, cur_proc, 0) - 1; 
            i = next_instruct[cur_proc];
          } //This updates our next_inst[cur_proc] and i to point to the next instruction
          
          //Locations in mem are absoluate addresses from the beginning of the file starting at 1
          
          //Indexing starts at 0 not 1, also adjust for the poffset            
          tmp2 = mem[0][i+1] - processes[cur_proc].poffset - 1;
          if(tmp == 0)
          {
            next_instruct[cur_proc] = lookup_addr(tmp2, cur_proc, 0) - 1; // sub one for IP increment at end of funciton
            processes[cur_proc].ip = tmp2 - 1;
          }//Jump to the right spot
          else
          {
            next_instruct[cur_proc]++;
            processes[cur_proc].ip++;
          }
          
          if(DBGCPU) printf("JFALSE (%d == 0) goto %d: mem[0][%d] = %d \n", tmp, tmp2, next_instruct[cur_proc] + 1, mem[0][next_instruct[cur_proc] + 1]);
          break;
          
    case JMP: //Unconditional Jump to the address specified  (needs pt translation)
          if((i % PAGESIZE) == PAGESIZE-1) // This is the Boundary between every page.
          {
            next_instruct[cur_proc] = lookup_addr(next_instruct[cur_proc] + 1, cur_proc, 0) - 1; 
            i = next_instruct[cur_proc];
          } //This updates our next_inst[cur_proc] and i to point to the next instruction

          //Locations in mem are absoluate addresses from the beginning of the file starting at 1
          
          //Indexing starts at 0 not 1, also adjust for the poffset            
          tmp = mem[0][i + 1] - processes[cur_proc].poffset - 1;
          
          next_instruct[cur_proc] = lookup_addr(tmp - 1, cur_proc, 0); // sub one for IP increment at end of funciton
          processes[cur_proc].ip = tmp - 1;
          if(DBGCPU) printf("JMP goto %d: mem[0][%d] = %d \n", tmp - 1, next_instruct[cur_proc] + 1, mem[0][next_instruct[cur_proc]+1]);
          break;
    
    case LOCK :
		      if (DBGCPU) printf("LOCK called by process %d\n", cur_proc); 
          return LOCKED;

    case UNLOCK : 
					if (DBGCPU) printf("UNLOCK called by process %d\n", cur_proc);
          return UNLOCKED;
        
    case END : 
          if (DBGCPU) printf("END: Process %d completed normally\n", cur_proc);
          *terminate = TERMINATED;
          return ENDPROCESS;

    case ENDP : //This should end all processes in the file.
          if (DBGCPU) printf("ENDP: Program %s completed normally\n", processes[cur_proc].filename);
          *terminate = TERMINATED;
          return ENDPROGRAM;
          
    case HALT :
          printf("HALT called by process %d\n\n", cur_proc);
          HALTED = TERMINATED;
          *terminate = TERMINATED;
          return ENDPROCESS;    
          
    case STOP : //Wait until a key is pressed
          printf("STOP called by proccess %d, hit any key to continue\n", cur_proc);
          scanf("%d", &tmp2);
          break;    
        
    case ADD :
          tmp1 = pop(stack, cur_proc, sp, 16);
          tmp2 = pop(stack, cur_proc, sp, 18);
          if(DBGCPU) printf("ADD %d = %d + %d \n", tmp1 + tmp2, tmp1, tmp2);
          tmp1 += tmp2;
          push(stack, cur_proc, sp, tmp1, 23);
          break;

    case SUB :
          tmp1 = pop(stack, cur_proc, sp, 20);
          tmp2 = pop(stack, cur_proc, sp, 22);
          if(DBGCPU) printf("SUB %d = %d - %d \n", tmp2 - tmp1, tmp2, tmp1);
          tmp1 = tmp2 - tmp1;
          push(stack, cur_proc, sp, tmp1, 25);
          break;

    case MUL :
          tmp1 = pop(stack, cur_proc, sp, 24);
          tmp2 = pop(stack, cur_proc, sp, 26);
          tmp1 *= tmp2;
          push(stack, cur_proc, sp, tmp1, 27);
          if(DBGCPU) printf("MUL %d = %d* %d\n",i, tmp1, tmp1 / tmp2, tmp2);
          break;

    case DIV :
          tmp1 = pop(stack, cur_proc, sp, 28);
          tmp2 = pop(stack, cur_proc, sp, 30);
          if(DBGCPU) printf("DIV %d = %d / %d \n", tmp1 / tmp2, tmp1, tmp2);
          tmp1 /= tmp2;
          push(stack, cur_proc, sp, tmp1, 29);
          break;
    
    case AND :
          tmp1 = pop(stack, cur_proc, sp, 32);
          tmp2 = pop(stack, cur_proc, sp, 34);
          if(DBGCPU) printf("AND %d = %d && %d \n", tmp1 && tmp2, tmp1, tmp2);
          tmp1 = tmp1 && tmp2;
          push(stack, cur_proc, sp, tmp1, 31);
          break ;

    case OR:
          tmp1 = pop(stack, cur_proc, sp, 36);
          tmp2 = pop(stack, cur_proc, sp, 38);
          if(DBGCPU) printf("OR %d = %d || %d \n", tmp1 || tmp2, tmp1, tmp2);
          tmp1 = tmp1 || tmp2;
          push(stack, cur_proc, sp, tmp1, 33);
          if(DBGCPU) printf("OR %d \n", tmp1); 
          break ;

    case NOT :
          tmp1 = pop(stack, cur_proc, sp, 40);
          if(DBGCPU) printf("NOT %d = !%d \n", !tmp1, tmp1);
          tmp1 = !tmp1;
          push(stack, cur_proc, sp, tmp1, 35);
          break;

    case LE_OP :
          tmp1 = pop(stack, cur_proc, sp, 42);
          tmp2 = pop(stack, cur_proc, sp, 44);
          tmp = tmp1 >= tmp2;
          push(stack, cur_proc, sp, tmp, 37);
          if(DBGCPU) printf("LE_OP (%d >= %d) == %d \n", tmp1, tmp2, tmp); 
          break;

    case GE_OP :
          tmp1 = pop(stack, cur_proc, sp, 46);
          tmp2 = pop(stack, cur_proc, sp, 48);
          tmp = tmp1 <= tmp2;
          push(stack, cur_proc, sp, tmp, 39);
          if(DBGCPU) printf("GE_OP (%d <= %d) == %d \n", tmp1, tmp2, tmp); 
          break;

    case LT_OP :
          tmp1 = pop(stack, cur_proc, sp, 50);
          tmp2 = pop(stack, cur_proc, sp, 52);
          tmp = tmp2 < tmp1;
          push(stack, cur_proc, sp, tmp, 41);
          if(DBGCPU) printf("LT_OP (%d < %d) == %d \n", tmp1, tmp2, tmp); 
          break;

    case GT_OP :
          tmp1 = pop(stack, cur_proc, sp, 54);
          tmp2 = pop(stack, cur_proc, sp, 56);
          tmp = tmp2 > tmp1;
          push(stack, cur_proc, sp, tmp, 43);
          if(DBGCPU) printf("GT_OP (%d > %d) == %d \n", tmp1, tmp2, tmp); 
          break;

    case EQ_OP :
          tmp1 = pop(stack, cur_proc, sp, 58);
          tmp2 = pop(stack, cur_proc, sp, 60);
          tmp = tmp1 == tmp2;
          push(stack, cur_proc, sp, tmp, 45);
          if(DBGCPU) printf("EQ_OP (%d == %d) == %d \n", tmp1, tmp2, tmp); 
          break;

    case NE_OP :
          tmp1 = pop(stack, cur_proc, sp, 62);
          tmp2 = pop(stack, cur_proc, sp, 64);
          tmp = tmp1 != tmp2;
          push(stack, cur_proc, sp, tmp, 47);
          if(DBGCPU) printf("NE_OP (%d != %d) == %d \n", tmp1, tmp2, tmp); 
          break;
          
    default:
          printf("Process %d: illegal instruction mem[0][%d] = %d\n", cur_proc,i, mem[0][i]);
          keyhit(127);
          break;
  }
  // increment next_instruction
  next_instruct[cur_proc]++;
  processes[cur_proc].ip++;
  
  if(DBGCPU && (abs( next_instruct[cur_proc] - diff) > 2) )
  {    
    if(0 < cur_proc % 3) printf("%s", indent);
    if(2 ==cur_proc % 3) printf("%s", indent);
    printf("Process %d: Next instr %d at Virtual IP %d\n", cur_proc, next_instruct[cur_proc], processes[cur_proc].ip);
  }
   
  return NORMAL;
}

int peek(int stack[][STACKSIZE], int proc_id, int sp[], int offset)
{
  int val = stack[proc_id][sp[proc_id] + offset];
  if (DEBUG)
    printf("peek : %d\n", val);
  return val;
}

int pop(int stack[][STACKSIZE], int proc_id, int sp[], int calledfrom)
{
  int val = stack[proc_id][sp[proc_id]];
  sp[proc_id]--;
  if(sp[proc_id] < -1)
  {
    printf("Stack Underflow: process %d %d\n", proc_id, sp[proc_id]);
    printf("Called from  %d\n", calledfrom);
    exit(-1);
  }
  return val;
}

void push(int stack[][STACKSIZE], int proc_id, int sp[], int data, int calledfrom)
{
  sp[proc_id]++;

  if(sp[proc_id] > STACKSIZE)
  {
    printf("Stack Overflow: process %d %d %d\n", proc_id, sp[proc_id], data);
    printf("PUSH Called from  %d\n", calledfrom);
    exit(-1);
  }
  stack[proc_id][sp[proc_id]] = data;
}

/* int newProcess(char * filename)
 * Description: This function first scans mem to build the PID table then runs the programs in mem
 * Input: The file to load into the process
 * Output: Returns the process ID index or -1 if an error occurs
 */
int new_process(char * filename)
{
  static int nextPid = 0;
  int len = strlen(arg1);
  
  processes[nextPid].filename = (char *) calloc(len,sizeof(char));
  if (processes[nextPid].filename == NULL)
    return -1;
  //Make the first process entry for the file.
  strncpy(processes[nextPid].filename, arg1, len);
  processes[nextPid].pid = nextPid;
  processes[nextPid].ip = 0; //First instruction is at 10 use the poffset to store this
  processes[nextPid].poffset = 10; //First process in the file
  processes[nextPid].status = READY;
  processes[nextPid].state = NOT_FINISHED;
  curProcesses = nextPid + 1;
  
  //Scan for more processes in the file.
  
  return nextPid++;
}

