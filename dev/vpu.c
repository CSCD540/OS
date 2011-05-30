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
int  exe(int stack[][STACKSIZE],int sp[],int reg[][REGISTERSIZE], int next_instruct[],int next_inst[], int cur_proc, int *terminate, int * cur_p);
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
    else if(strcmp(cmd, "showRegisterData")==0)
    {
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
  int diff = 0;

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
          
        do{
          cur_proc = (cur_proc + 1) % curProcesses;//rand() % curProcesses; //Find one of the programs to run  
        }while(processes[cur_proc].status == TERMINATED);//Only use non-terminated processes
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
        diff = next_instruct[cur_proc];
        if(processes[cur_proc].status == READY)
        {
          processes[cur_proc].status = RUNNING;
          msg = exe(stack,sp,reg, next_instruct, next_instruct, cur_proc, &terminate, &cur_proc);
        }
        else
        {
          //Wait for IO to return
          //If we pick a number less than 10 out of 1000 the IO returned and set status to 0 ready
          processes[cur_proc].status = READY;
        }
        
        if(diff != next_instruct[cur_proc]) processes[cur_proc].ip++;//If the next_instruct changed increment
        
        if(msg==ENDPROCESS || terminate == TERMINATED)
        {
          processes[cur_proc].status = TERMINATED; //Terminated
          goto checkdone;
        }

        // increment next_instruction
        next_instruct[cur_proc]++;
        processes[cur_proc].ip++;
        if(DBGCPU && (abs( next_instruct[cur_proc] - diff) > 2) )
        {    
          if(0 < cur_proc % 3) printf("\t\t\t\t");
          if(2 ==cur_proc % 3) printf("\t\t\t\t");
          printf("Process %d: Next instr %d at Vip %d\n", cur_proc, next_instruct[cur_proc],processes[cur_proc].ip);
        }
        if(msg==UNLOCKED)
        {
          // printf("unlock\n");
          locked=UNLOCKED;
        }
        else if(msg==LOCKED || locked==LOCKED)
        {
          // printf("locked\n");
          locked=LOCKED;
        }
      }
      else
      {
        printf("Process %d complete, terminate=%d\n",cur_proc, terminate);
        processes[cur_proc].status = TERMINATED;
      }
      // check if all processes are done
    checkdone:
      for(i = 0; i< curProcesses; i++)//If at least one process is still running
        if(processes[i].state == NOT_FINISHED)
          goto cont;
      break;
  }
  // print_stack(stack,sp); stack should be all 0 and sp at -1
  //print_gmem();
  //print_register(reg);
}

int exe(int stack[][STACKSIZE], int sp[], int reg[][REGISTERSIZE], int next_instruct[], int next_inst[], int cur_proc, int *terminate, int *cur_p)
{
  int i, k, m; // delete these after all accesses renamed, except i
  int tmp, tmp1, tmp2;
  int real_inst;
  char name[11];

  i = next_inst[cur_proc];
  real_inst = i;
  if(0 < cur_proc % 3)printf("\t\t\t\t");
  if(2 ==cur_proc % 3)printf("\t\t\t\t");
  printf("(pid=%d) PC = %d:  ", cur_proc, i);
  printf(" = %d\n", mem[0][i] );
  if(0 < cur_proc % 3)printf("\t\t\t\t");
  if(2 ==cur_proc % 3)printf("\t\t\t\t");


   if (DEBUG)	
		printf("In Main Memory: process %d, program counter %d, content:%d\n\n", cur_proc, i, mem[0][i]);

   switch (mem[0][i])
   {
     /** OPEN, READ, CLOSE, WRITE, SEEK ::  OS services **/
      case OPEN :
          if (DBGCPU) printf("Open file\n");
          processes[cur_proc].state = 2; //Set to waiting on IO
          break;
          
      case READ :
          if (DBGCPU) printf("Read file\n");
          processes[cur_proc].state = 2; //Set to waiting on IO
          break;

      case CLOSE :
          if (DBGCPU) printf("Close file\n");
          break;

      case WRITE :
          if (DBGCPU) printf("Open file\n");
          processes[cur_proc].state = 2; //Set to waiting on IO
          break;

      case SEEK :
            tmp = peek(stack, cur_proc, sp, 0) ;
            if (DBGCPU) printf("SEEK offset=  0,  data=%d\n", tmp);
            processes[cur_proc].state = 2; //Set to waiting on IO
            tmp1 = peek(stack, cur_proc, sp, -1) ;
            if (DBGCPU)
            {
							printf("SEEK offset= -1,  fd =%d\n", tmp1); 
							printf("OS service call  --- <SEEK> \n");
						}
          break;

      case POPD : //This takes one argument (The next 'instruction' is the register to pop into)
			      tmp = mem[0][i+1];
            tmp1 = pop(stack, cur_proc, sp, 10) ;
			      if(DBGCPU) printf("POPD: popd %d into %d\n", tmp1, tmp);
            if(tmp < 230)
            { gmem[tmp] = tmp1; }
            else
            {
              tmp = tmp-230;
              reg[cur_proc][tmp] = tmp1;
            }
            next_inst[cur_proc]++;
          break;

      case POP : 
          tmp1 = pop(stack, cur_proc, sp, 12);
          if(DBGCPU) printf("POP %d into nowhere\n",tmp1);
          break;

      case LD :
            tmp = pop(stack, cur_proc, sp, 14);
            tmp1 = gmem[tmp];
            if(DBGCPU) printf("LD %d from %d\n",tmp1, tmp);
            // printf("%04d LD %d %d\n",i,tmp1,tmp);
            push(stack, cur_proc, sp, tmp1, 15);
          break;

      case LA : 
            if(DBGCPU) printf("LA %d\n",tmp);
	          if(i == PAGESIZE-1 || i == PAGESIZE * 2-1) // This is the Boundary between every page.
	          {
              // load address of start of array
		          tmp = mem[0][i];
	          }
	          else
	            tmp = mem[0][i+1];
            push(stack, cur_proc, sp, tmp, 17);
	          // printf("%04d LA %d %d\n",i,tmp);
            next_inst[cur_proc]++;
          break;

      case LOAD : //Loads the gmem address at tmp
            if(DBGCPU)printf("LOAD ");
	          if(i == PAGESIZE-1 || i == PAGESIZE*2-1) // This is the Boundary between every page.
	            tmp = mem[0][i];
	          else
	            tmp = mem[0][i+1];
            // printf("load 1 %d\n",tmp);
            // printf("load 1 mem[%d][%d]\n",cur_proc, i+1);
            // printf("stack  0= %d\n", peek(stack,cur_proc,sp, 0)) ;
            // printf("stack -1= %d\n", peek(stack,cur_proc,sp, -1)) ;
            if(tmp < 230)
              tmp1 = gmem[tmp];
            else
            {
              tmp = tmp-230;
              tmp1 = reg[cur_proc][tmp];
            }
            if(DBGCPU)printf("%d from index %d = %d\n", tmp1, tmp, *cur_p);
            push(stack, cur_proc, sp, tmp1, 19);
            // printf("%04d load tmp %d %d %d\n",i+1,tmp,tmp1,cur_proc);
            // printf("stack  0= %d\n", peek(stack,cur_proc,sp, 0)) ;
            // printf("stack -1= %d\n", peek(stack,cur_proc,sp, -1)) ;
            next_inst[cur_proc]++;
          break;

      case LOADI : //Load the constant to the stack
            if(i == PAGESIZE-1 || i == PAGESIZE*2-1) // This is the Boundary between every page.
            {
              push(stack, cur_proc, sp, mem[0][i], 21);
              if(DBGCPU)printf("LOADI and Push: %d\n",mem[0][i]);
            }
            else
            {
              push(stack, cur_proc, sp, mem[0][i+1], 21);
              if(DBGCPU)printf("LOADI Push:%d\n",mem[0][i+1]);
            } 
            // printf("%04d loadi %d\n",i,stack[cur_proc][sp[cur_proc]] );
            next_inst[cur_proc]++;
          break;

      case STOR : //Stor at the gmem index indicated 
            tmp = pop(stack, cur_proc, sp, 68);
            if (i == PAGESIZE-1 || i == PAGESIZE * 2-1) // This is the Boundary between every page.
              tmp1 = mem[0][i];
            else
              tmp1 = mem[0][i+1];
            if(tmp1 < 230)
            {
              gmem[tmp1] = tmp;
              if (DBGCPU) printf("STOR wrote %d to global mem in index %d\n", gmem[tmp1], tmp1);
              // printf("returning p0WRITE\n"); keyhit(99);
              next_inst[cur_proc]++;
              return p0WRITE;
            } 
            else
            {
              reg[cur_proc][tmp1-230] = tmp;
              next_inst[cur_proc]++;
            }
          break;

      case ST :
            tmp = pop(stack, cur_proc, sp, 70); // does ST ever need to store in a register?
            // printf("%d\n",tmp);
            tmp1 = pop(stack, cur_proc, sp, 72);
            // printf("%d\n",tmp1);
            if(DBGCPU) printf("ST %d int gmem[%d]\n",tmp1, tmp);
            
            gmem[tmp] = tmp1;
            if (DBGCPU) printf("process %d wrote to global mem in index %d, %d\n", cur_proc, tmp, gmem[tmp]);
            return p0WRITE;
                  
      case JFALSE : //Jump to the address specified  (needs pt translation)
            tmp = pop(stack, cur_proc, sp, 74);
            if(i == PAGESIZE-1 || i == PAGESIZE*2-1) // This is the Boundary between every page.
            { 
              int temp = lookup_addr(next_inst[cur_proc]+1,cur_proc,0); 
              tmp2 = mem[0][temp] - 2;//Need to look up the next page and subtract one so our vitrual i is two less
            }
            else
              tmp2 = mem[0][i+1];
            if(DBGCPU) printf("JFALSE %d %d \n", tmp, tmp2);
            if(tmp == 0)
            {
              next_instruct[cur_proc] = lookup_addr(tmp2 - 1, cur_proc, 0); // sub one for PC in executeit()
              processes[cur_proc].ip = tmp2 - 1;
            }
            else
              next_inst[cur_proc]++;
          break;
      case JMP: //Jump to the address specified  (needs pt translation)
            if(i == PAGESIZE-1 || i == PAGESIZE*2-1) // This is the Boundary between every page.
            {
              int temp = lookup_addr(next_inst[cur_proc]+1,cur_proc,0); 
              tmp = mem[0][temp] - 2;//Need to look up the next page and subtract one so our vitrual i is one less
            }
            else
              tmp = lookup_addr(mem[0][i], cur_proc, 0);	//mem[0][i];
            next_instruct[cur_proc] = lookup_addr(tmp - 1, cur_proc, 0); // sub one for PC in executeit() 
            processes[cur_proc].ip = tmp - 1;
            if(DBGCPU) printf("JMP to %d\n", i, next_instruct[cur_proc]); 
            // next_inst[cur_proc]++;
            break;
      
      case LOCK :
			      if (DEBUG) printf("LOCK called by process %d\n", cur_proc); 
            return LOCKED;

      case UNLOCK : 
  					if (DEBUG) printf("UNLOCK called\n");
            return UNLOCKED;

      case HALT :
            printf("HALT called by process %d\n\n", cur_proc);
            // print_gmem();
            // print_register(reg);
            HALTED=1;
            *terminate = 1;
            // exit(0);
          break;    
          
      case END : 
            if (DEBUG) printf("Process %d completed normally\n", cur_proc);
            p0running=0;
            
            return ENDPROCESS;

      case ENDP :
            // printf("ENDP\n");
            *terminate = 1;
            break;
            
      case STOP :
            printf("STOP called by proccess %d, hit any key to continue\n", cur_proc);
            scanf("%d", &tmp2);
          break;    
          
      case ADD :
            tmp1 = pop(stack, cur_proc, sp, 16);
            tmp2 = pop(stack, cur_proc, sp, 18);
            tmp1 += tmp2;
            push(stack, cur_proc, sp, tmp1, 23);
            if(DBGCPU) printf("ADD %d += %d\n",i, tmp1, tmp2);
          break;

      case SUB :
            tmp1 = pop(stack, cur_proc, sp, 20);
            tmp2 = pop(stack, cur_proc, sp, 22);
            tmp1 = tmp2-tmp1;
            push(stack, cur_proc, sp, tmp1, 25);
            if(DBGCPU) printf("SUB %d -= %d\n",i, tmp1, tmp2);
          break;

      case MUL :
            tmp1 = pop(stack, cur_proc, sp, 24);
            tmp2 = pop(stack, cur_proc, sp, 26);
            tmp1 *= tmp2;
            push(stack, cur_proc, sp, tmp1, 27);
            if(DBGCPU) printf("MUL %d *= %d\n",i, tmp1, tmp2);
          break;

      case DIV :
            tmp1 = pop(stack, cur_proc, sp, 28);
            tmp2 = pop(stack, cur_proc, sp, 30);
            tmp1 = tmp2 / tmp1;
            push(stack, cur_proc, sp, tmp1, 29);
            if(DBGCPU) printf("DIV %d /= %d\n",i, tmp1, tmp2);
          break;
      
      case AND :
            tmp1 = pop(stack, cur_proc, sp, 32);
            tmp2 = pop(stack, cur_proc, sp, 34);
            tmp1 = tmp1 && tmp2;
            push(stack, cur_proc, sp, tmp1, 31);
            if(DBGCPU) printf("AND %d \n", tmp1); 
          break ;

      case OR:
            tmp1 = pop(stack, cur_proc, sp, 36);
            tmp2 = pop(stack, cur_proc, sp, 38);
            tmp1 = tmp1 || tmp2;
            push(stack, cur_proc, sp, tmp1, 33);
            if(DBGCPU) printf("OR %d \n", tmp1); 
          break ;

      case NOT :
            tmp1 = pop(stack, cur_proc, sp, 40);
            tmp1 = !tmp1;
            push(stack, cur_proc, sp, tmp1, 35);
            if(DBGCPU) printf("NOT %d \n", tmp1); 
          break;

      case LE_OP :
            tmp1 = pop(stack, cur_proc, sp, 42);
            tmp2 = pop(stack, cur_proc, sp, 44);
            tmp = tmp1 >= tmp2;
            push(stack, cur_proc, sp, tmp, 37);
            if(DBGCPU) printf("LE_OP %d \n", tmp); 
          break;

      case GE_OP :
            tmp1 = pop(stack, cur_proc, sp, 46);
            tmp2 = pop(stack, cur_proc, sp, 48);
            tmp = tmp1 <= tmp2;
            push(stack, cur_proc, sp, tmp, 39);
            if(DBGCPU) printf("GE_OP %d \n", tmp);  
          break;

      case LT_OP :
            tmp1 = pop(stack, cur_proc, sp, 50);
            tmp2 = pop(stack, cur_proc, sp, 52);
            tmp = tmp2 < tmp1;
            push(stack, cur_proc, sp, tmp, 41);
            if(DBGCPU) printf("LT_OP %d \n", tmp);
          break;

      case GT_OP :
            tmp1 = pop(stack, cur_proc, sp, 54);
            tmp2 = pop(stack, cur_proc, sp, 56);
            tmp = tmp2 > tmp1;
            push(stack, cur_proc, sp, tmp, 43);
            if(DBGCPU) printf("GT_OP %d \n", tmp);
          break;

      case EQ_OP :
            tmp1 = pop(stack, cur_proc, sp, 58);
            // printf("step 2 %d\n",sp[cur_proc]);
            tmp2 = pop(stack, cur_proc, sp, 60);
            tmp = tmp1 == tmp2;
            push(stack, cur_proc, sp, tmp, 45);
            if(DBGCPU) printf("EQ_OP %d\n", tmp);
          break;

      case NE_OP :
            tmp1 = pop(stack, cur_proc, sp, 62);
            tmp2 = pop(stack, cur_proc, sp, 64);
            tmp = tmp1 != tmp2;
            push(stack, cur_proc, sp, tmp, 47);
            if(DBGCPU) printf("NE_OP %d\n", tmp);
          break;
      default:
            printf("illegal instruction mem[0][%d] = %d\n", cur_proc,i, mem[0][i]);
            keyhit(127);
          break;
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
    //exit(-1);
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
  processes[nextPid].ip = 10; //First instruction is at 10
  processes[nextPid].status = READY;
  processes[nextPid].state = NOT_FINISHED;
  curProcesses = nextPid + 1;
  
  //Scan for more processes in the file.
  
  return nextPid++;
}

