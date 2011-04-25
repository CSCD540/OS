#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "easm.tab.h"
#include "linkedlist.h"

/*begin isaac execute vars*/
#define MAXPRO 1   //max num of processes
#define MAXMEM 64 //max word size of a process
#define STACKSIZE 100 //max size of the stack
#define REGISTERSIZE 10 //size of each process registers
#define MAXGMEM 20 //max size of global memory
#define NORMAL 0 //denotes a normal return from exe()
#define LOCKED 1 //stops process switching until unlock
#define UNLOCKED 2 //remove lock
#define ENDPROCESS 3

#define p0WRITE 4 //tells p0 to run-p0 should only run after a write to gmem
#define BLOCKSIZE 4 //size for per block
#define BLOCKS DISKSIZE/BLOCKSIZE// total number of blocks
#define DISKSIZE 16*1024//16kB
#define PAGESIZE 16 //size of each page
#define NUMPAGES 2 //Number of pages in page table
#define DBGCPU 1
#define DBGCPU1 0

#define keyhit(a) {if(DBGCPU1){printf("hit enter --(%d)", a); getchar();}}

// Variables
int gmem[MAXGMEM]; // global var sit here
int mem[MAXPRO][MAXMEM]; // Main mem for each process
int endprog[MAXPRO]; // last instruction of proc
int jsym[60];
int pid = 0;  //process id
int p0running;
int  size_of_disk=DISKSIZE;
int  DEBUG = 0;
int  total_blocks=BLOCKS; // Total blocks
int  file_index=0; // main index
int  search_index=0; // temp index for search
char *cmd; // The command(save, load, ls...etc.)
char *arg1; // The argument(file's name)
int  disk_index=0; // main index
int  machine_on = 1;
int  tempmem[MAXPRO][200]; // for PTB - loading all of the process information here
int  fileMonitor[30];
int  HALTED=0;
int  gfd; // file discriptor
int  reg[MAXPRO][REGISTERSIZE];
// end Variables

// Jordan's Variables
char input[30]; //Console input
// end Jordan's Variables

// Methods declaration
int  exe(int stack[][STACKSIZE],int sp[],int reg[][REGISTERSIZE], int next_instruct[],int next_inst[], int cur_proc, int *terminate);
void executeit();
void grabData(int index,int *grabdata);
void initGmem();
void initReg();
void print_register(int reg[][REGISTERSIZE]);
void print_gmem();
void print_stack(int stack[][STACKSIZE],int sp[]);
int  peek(int stack[][STACKSIZE], int proc_id, int sp[], int offset);
void push(int stack[][STACKSIZE], int proc_id, int sp[],int data, int calledfrom);
int  pop(int stack[][STACKSIZE], int proc_id, int sp[], int calledfrom);
void showExit();
void showRegisterData();
void showManPage(char *cmd);
void resetMemory();
//end Methods declaration

// Jordan's method declarations
void concatenate(char *filename);
void listDirectoryContents();
void loadProgram(char *filename);
void removeFile(char *filename);
void saveFile(char *filename);
void showHelp();
// end Jordan's method declarations


int main(int argc, char *argv[])
{
  /* Shell command 
  * Commands: save,del,ls,exit,run,help
  * switch case doesn't support string. 
  * */	
  while(machine_on)
  {
    printf("evm$ ");

    fgets(input,sizeof(input),stdin);

    input[strlen(input)-1] = 0;

        //split the cmd and arg(file name)
    cmd = strtok(input, " ");
    arg1= strtok('\0', " ");

    if(strlen(input)<1)
    {
      continue;//When there is no input string, skiping everything.
    }
    else if(strcmp(cmd, "ls")==0) 
    {
      listDirectoryContents();
    }
    else if(strcmp(cmd, "cat")==0)
    {
      concatenate(arg1);
    }
    else if(strcmp(cmd, "debug")==0)
    {
    }
    else if(strcmp(cmd, "exit")==0)
    {
      showExit();
    }
    else if(strcmp(cmd, "help")==0)
    {
      showHelp();
    }
    else if(strcmp(cmd, "load")==0)
    {
      loadProgram(arg1);
    }
    else if(strcmp(cmd, "man")==0)
    {
    }
    else if(strcmp(cmd, "memdump")==0)
    {
    }
    else if(strcmp(cmd, "rm")==0)
    {
      removeFile(arg1);
    }
    else if(strcmp(cmd, "run")==0)
    {
      executeit();
    }
    else if(strcmp(cmd, "save")==0)
    {
      saveFile(arg1);
    }
    else if(strcmp(cmd, "showBlocks")==0)
    {
    }
    else if(strcmp(cmd, "showGlobalMem")==0)
    {
    }
    else if(strcmp(cmd, "showLRU")==0)
    {
    }
    else if(strcmp(cmd, "showpage")==0)
    {
    }
    else if(strcmp(cmd, "showRegisterData")==0)
    {
    }
    else
		{
      printf("Bad command\n");
    }
  }// end while(machine_on)

  return 0;

  /*  int fi =0;*/
  /*  int coni =0;*/
  /*  int size = 0;*/
  /*  FILE *f;*/

  /*  printf("excecuting %s\n", argv[1]);*/

  /*  f = fopen(argv[1], "r");*/
  /*  if (f == NULL)*/
  /*  {*/
  /*    return -1; // -1 means file opening fail*/
  /*  }*/

  /*  while(fscanf(f, "%d\n", &fi) != EOF)*/
  /*  {*/
  /*    mem[0][coni]=fi;*/
  /*    printf("%d: %d\n", coni, mem[0][coni]);*/
  /*    size++;*/
  /*    coni++;*/
  /*  }*/
  /*  fclose(f);*/

  /*  executeit();*/
}


/*
 * Concatenate the contents of the file specified
 */
void concatenate(char *filename)
{
  printf("Concatenate the file %s\n", filename);
}


/* 
 * List the contents of the current directory
 */
void listDirectoryContents()
{
	printf("Print out the contents of PWD.\n");
}


/* 
 * Invoke the program loader & load into memory
 */
void loadProgram(char *filename)
{
	printf("Load program %s.\n", filename);
}


/* 
 * Delete a file from the disk
 */
void removeFile(char *filename)
{
  printf("Delete file \"%s\" from the filesystem.\n", filename);
}


/* 
 * Save a file to the disk
 */
void saveFile(char *filename)
{
  printf("Save the file \"%s\" to the filesystem.\n", filename);
}


/*
 * Show the exit message and set machine_on to 0 (turn it off)
 */
void showExit()
{
		printf("***********************************************************\n");
		printf("*       Thanks for using the Eagle Virtual Machine!       *\n");
		printf("*                         GOOD-BYE!                       *\n");
		printf("***********************************************************\n");
		machine_on = 0;
}


/*
 * Show the help screen for the shell
 */
void showHelp()
{
  printf("Print out the help screen for the shell\n");
}


/*
 * Begin Isaac execute
 */
void executeit()
{
  int cur_proc,p0=0, msg=-1,m;
  int stack[MAXPRO][STACKSIZE];
  int sp[MAXPRO];
  int next_instruct[MAXPRO];
  int proc_complete[MAXPRO];
  // int reg[MAXPRO][REGISTERSIZE];
  int locked=UNLOCKED;
  int terminate=0;

  memset(stack,0,MAXPRO*STACKSIZE*sizeof(int));
  memset(sp,-1,MAXPRO*sizeof(int));
  // memset(next_instruct,0,MAXPRO*sizeof(int));
  memset(proc_complete,0,MAXPRO*sizeof(int));
  memset(reg,0,10*MAXPRO*sizeof(int));
  srand( time(NULL) );

  next_instruct[0]=10;
  next_instruct[1]=10;
  next_instruct[2]=10;
  next_instruct[3]=10;
  next_instruct[4]=10;
  next_instruct[5]=10;

  /*
  for(i=0;i<pid;i++)
    for(m=0;m<STACKSIZE;m++)
       printf("STACK %d: %d\n",i,stack[i][m]);

  while(1) //used for testing a single process
  {
    cur_proc=2;
    if(next_instruct[cur_proc]<endprog[cur_proc])
    {
      msg=exe(stack,sp,reg,next_instruct,next_instruct,cur_proc);
      // increment next_instruction
      next_instruct[cur_proc]++;
      printf("loop: %d\n",next_instruct[cur_proc]);
    }
    else break;
  }
  */

  keyhit(54);
  cur_proc=0;
  while(1)
  {
    cont:

    keyhit(55);

    if (HALTED==1)
    {
      HALTED=0;
      break;
    }
    if(locked==UNLOCKED)
    {
      // printf("pid=%d\n", pid); //keyhit(8999);
      cur_proc = 0; // only one core 
    }

    if(proc_complete[cur_proc] == 1)
    {
      if (DEBUG!=0)
       printf("----------------------------cur_proc: %d\n",cur_proc);
      goto checkdone;
    }

    if(next_instruct[cur_proc]< 256) // safe guard
    {
      msg=exe(stack,sp,reg,next_instruct,next_instruct,cur_proc, &terminate);
      if(msg==ENDPROCESS || terminate == 1)
      {// Ming
        proc_complete[cur_proc]=1;
        goto checkdone;// Ming
      }// Ming

      // printf("%d %d\n",cur_proc,next_instruct[cur_proc]+1);
      // increment next_instruction
      next_instruct[cur_proc]++;
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
         
#if 0         
      // run p0 in its entirety after a gmem write
      // cur_proc=0;
      // while(msg==p0WRITE || p0running)
      while(1)
      {
        p0running=1; cur_proc=0;
        // printf("p0 started   PC=%d\n", next_instruct[cur_proc]);
        msg=exe(stack,sp,reg,next_instruct,next_instruct,p0);
        // printf("p1, nextPC=%d\n" , next_instruct[1]);

        /*
        if(HALTED==1)
        {
          break;
        }
        */

        next_instruct[cur_proc]++;
        if(p0running == 0)
        {
          msg=NORMAL;
          next_instruct[p0]=10;
          break;
        }
        // printf("branch %d \n",(next_instruct[cur_proc]<endprog[cur_proc]));

        if( next_instruct[p0]>=endprog[p0])
        {
          p0running=0;
          sp[p0]=0;
          next_instruct[p0]=10;
          msg=NORMAL;
          break;
        }
      }
      continue;
#endif

    }
    else
    {
      printf("Process %d complete, terminate=%d\n",cur_proc, terminate);
      proc_complete[cur_proc]=1;
    }
    // check if all processes are done
    checkdone:
    // for(cur_proc=0;cur_proc<pid;cur_proc++)
    if(terminate ==0)
     goto cont;
    break;
  }
  // print_stack(stack,sp); stack should be all 0 and sp at -1
  print_gmem();
  print_register(reg);
}



int exe(int stack[][STACKSIZE],int sp[],int reg[][REGISTERSIZE], int next_instruct[],int next_inst[], int cur_proc, int *terminate)
{
  int i,k, m; //delete these after all accesses renamed, except i
  int tmp,tmp1, tmp2;
  int real_inst;
  char name[11];

  i = next_inst[cur_proc];
  real_inst = i;
  printf("(pid=%d) PC = %d:  ", cur_proc, i);
  printf(" = %d\n", mem[cur_proc][i] );
// gmem[6]=101;

/** the following 3 lines are for debugging user program too **/
#if 0
   print_gmem();
   print_register(reg);
   resetMemory();
   keyhit(343);
#endif

   if (DEBUG!=0)	
		printf("In Main Memory: process %d, program counter %d, content:%d\n\n",cur_proc,i,mem[cur_proc][i]);
   //sleep(1);

   switch (mem[cur_proc][i] )
   {
     /** OPEN, READ, CLOSE, WRITE, SEEK ::  OS services **/
      case OPEN :
          break;
      case READ :
          break;

      case CLOSE :
          break;

      case WRITE :
          break;

      case SEEK :
            tmp = peek(stack,cur_proc,sp, 0) ;
            if (DEBUG!=0) printf("SEEK offset=  0,  data=%d\n", tmp);
            tmp1 = peek(stack,cur_proc,sp, -1) ;
            if (DEBUG!=0)
            {
							printf("SEEK offset= -1,  fd =%d\n", tmp1); 
							printf("OS service call  --- <SEEK> \n");
						}
          break;

      case POPD :
						tmp = mem[cur_proc][i+1];
            tmp1 = pop(stack,cur_proc,sp, 10) ;
						if( DBGCPU ) printf("POPD: popd %d into %d\n", tmp1, tmp);
            if(tmp<230)
            { gmem[tmp]=tmp1; }
            else
            {
              tmp = tmp-230;
              reg[cur_proc][tmp]= tmp1;
            }
            next_inst[cur_proc]++;
          break;

      case POP : 
           tmp1 = pop(stack,cur_proc,sp, 12);
          break;

      case LD :
            tmp = pop(stack,cur_proc,sp, 14);
            tmp1 = gmem[tmp];
            // printf("%04d LD %d %d\n",i,tmp1,tmp);
            push(stack,cur_proc,sp,tmp1, 15);
          break;

      case LA : 
          if(DBGCPU) printf("LA1 %d\n",tmp);
	        if(i==PAGESIZE-1 || i ==PAGESIZE*2-1)//This is the Boundary between every page.
	        {
            // load address of start of array
		        tmp = mem[cur_proc][i];
	        }
	        else
	          tmp = mem[cur_proc][i+1];
          push(stack,cur_proc,sp,tmp, 17);
	        //printf("%04d LA %d %d\n",i,tmp);
          next_inst[cur_proc]++;
        break;

      case LOAD :
          if(DBGCPU)printf("LOAD\n");
	        if(i==PAGESIZE-1 || i ==PAGESIZE*2-1)//This is the Boundary between every page.
	          tmp = mem[cur_proc][i];
	        else
	          tmp = mem[cur_proc][i+1];
          //printf("load 1 %d\n",tmp);
          //printf("load 1 mem[%d][%d]\n",cur_proc, i+1);
          //printf("stack  0= %d\n", peek(stack,cur_proc,sp, 0)) ;
          //printf("stack -1= %d\n", peek(stack,cur_proc,sp, -1)) ;
          if(tmp<230)
            tmp1 = gmem[tmp];
          else
          {
            tmp = tmp-230;
            tmp1 = reg[cur_proc][tmp];
          }
          push(stack,cur_proc,sp,tmp1, 19);
          //printf("%04d load tmp %d %d %d\n",i+1,tmp,tmp1,cur_proc);
          //printf("stack  0= %d\n", peek(stack,cur_proc,sp, 0)) ;
          //printf("stack -1= %d\n", peek(stack,cur_proc,sp, -1)) ;
          next_inst[cur_proc]++;
        break;

      case LOADI : 
			    if(i==PAGESIZE-1 || i ==PAGESIZE*2-1)//This is the Boundary between every page.
			    {
				    push(stack,cur_proc,sp,mem[cur_proc][i], 21);
				    //printf("i == 31 ,LOADI Push:%d\n",mem[cur_proc][i]);
			    }
			    else
			    {
				    push(stack,cur_proc,sp,mem[cur_proc][i+1], 21);
				    //printf("i < 31 ,LOADI Push:%d\n",mem[cur_proc][i+1]);
			    } 
			    //printf("%04d loadi %d\n",i,stack[cur_proc][sp[cur_proc]] );
          next_inst[cur_proc]++;
         break;

      case ADD :
          tmp1 = pop(stack,cur_proc,sp, 16);
          tmp2 = pop(stack,cur_proc,sp, 18);
          tmp1 += tmp2;
          push(stack,cur_proc,sp,tmp1, 23);
          if(DBGCPU) printf("%04d:  ADD %d\n",i, tmp1);
        break;

      case SUB :
          tmp1 = pop(stack,cur_proc,sp, 20);
          tmp2 = pop(stack,cur_proc,sp, 22);
          tmp1 = tmp2-tmp1;
          push(stack,cur_proc,sp,tmp1, 25);
          // printf("%04d:  SUB\n", i); 
        break;

      case MUL :
          tmp1 = pop(stack,cur_proc,sp, 24);
          tmp2 = pop(stack,cur_proc,sp, 26);
          tmp1 *= tmp2;
          push(stack,cur_proc,sp,tmp1, 27);
          //  printf("%04d:  MUL\n", i); 
        break;

      case DIV :
          tmp1 = pop(stack,cur_proc,sp, 28);
          tmp2 = pop(stack,cur_proc,sp, 30);
          tmp1 = tmp2/tmp1;
          push(stack,cur_proc,sp,tmp1, 29);
          //  printf("%04d:  DIV\n", i); 
        break;

      case END : 
          if (DEBUG!=0) printf("Process %d completed normally\n", cur_proc);
          p0running=0;
          *terminate = 1;
          return ENDPROCESS;

      case ENDP :
          //printf("ENDP\n");
        break;

      case AND :
          tmp1 = pop(stack,cur_proc,sp, 32);
          tmp2 = pop(stack,cur_proc,sp, 34);
          tmp1 = tmp1 && tmp2;
          push(stack,cur_proc,sp,tmp1, 31);
          if(DBGCPU) printf("%04d:  AND\n", i); 
        break ;

      case OR:
            tmp1 = pop(stack,cur_proc,sp, 36);
            tmp2 = pop(stack,cur_proc,sp, 38);
            tmp1 = tmp1 || tmp2;
            push(stack,cur_proc,sp,tmp1, 33);
            if(DBGCPU) printf("%04d:  OR\n", i); 
          break ;

      case NOT :
            tmp1 = pop(stack,cur_proc,sp, 40);
            tmp1 = !tmp1;
            push(stack,cur_proc,sp,tmp1, 35);
            if(DBGCPU) printf("%04d:  NOT\n", i); 
          break;

      case LE_OP :
              tmp1 = pop(stack,cur_proc,sp, 42);
              tmp2 = pop(stack,cur_proc,sp, 44);
              tmp = tmp1 >= tmp2;
              push(stack,cur_proc,sp,tmp, 37);
              if(DBGCPU) printf("%04d:  LE_OP %d\n", i, tmp); 
            break;

      case GE_OP :
              tmp1 = pop(stack,cur_proc,sp, 46);
              tmp2 = pop(stack,cur_proc,sp, 48);
              tmp = tmp1 <= tmp2;
              push(stack,cur_proc,sp,tmp, 39);
              // printf("%04d:  GE_OP%d\n", i,tmp); 
            break;

      case LT_OP :
              tmp1 = pop(stack,cur_proc,sp, 50);
              tmp2 = pop(stack,cur_proc,sp, 52);
              tmp = tmp2 < tmp1;
              push(stack,cur_proc,sp,tmp, 41);
              if(DBGCPU) printf("%04d:  LT_OP %d %d %d\n", i,tmp,tmp1,tmp2);
            break;

      case GT_OP :
              tmp1 = pop(stack,cur_proc,sp, 54);
              tmp2 = pop(stack,cur_proc,sp, 56);
              tmp = tmp2 > tmp1;
              push(stack,cur_proc,sp,tmp, 43);
              if(DBGCPU) printf("%04d:  GT_OP %d SP %d %d\n", i,tmp, sp[cur_proc],GT_OP); 
            break;

      case EQ_OP :
              tmp1 = pop(stack,cur_proc,sp, 58);
              //printf("step 2 %d\n",sp[cur_proc]);
              tmp2 = pop(stack,cur_proc,sp, 60);
              if(DBGCPU) printf("EQ? %d %d\n", tmp1, tmp2);
              tmp = tmp1 == tmp2;
              push(stack,cur_proc,sp,tmp, 45);
              if(DBGCPU) printf("%04d:  EQ_OP %d\n", i, tmp); 
            break;

      case NE_OP :
              tmp1 = pop(stack,cur_proc,sp, 62);
              tmp2 = pop(stack,cur_proc,sp, 64);
              tmp = tmp1 != tmp2;
              push(stack,cur_proc,sp,tmp, 47);
              if(DBGCPU) printf("%04d:  NE_OP\n", i); 
            break;

      case STOP :
              printf("STOP called by proccess %d, hit any key to continue\n", cur_proc);
              scanf("%d",&tmp2);
            break;

      case STOR :
              tmp = pop(stack,cur_proc,sp, 68);
              if (i==PAGESIZE-1 || i ==PAGESIZE*2-1)//This is the Boundary between every page.
                tmp1 = mem[cur_proc][i];
              else
                tmp1 = mem[cur_proc][i+1];
              if(tmp1<230)
              {
                gmem[tmp1]=tmp;
                if (DEBUG !=0) printf("Process %d wrote to global mem in index %d, %d\n",cur_proc,tmp1,gmem[tmp1]);
                //printf("returning p0WRITE\n"); keyhit(99);
                next_inst[cur_proc]++;
                return p0WRITE;
              } 
              else
              {
                reg[cur_proc][tmp1-230]=tmp;
                next_inst[cur_proc]++;
              }
            break;

      case ST :
              tmp = pop(stack,cur_proc,sp, 70);//does ST ever need to store in a register?
              //printf("%d\n",tmp);
              tmp1 = pop(stack,cur_proc,sp, 72);
              //printf("%d\n",tmp1);
              gmem[tmp]=tmp1;
              if (DEBUG !=0) printf("process %d wrote to global mem in index %d, %d\n",cur_proc,tmp,gmem[tmp]);
              return p0WRITE;

      case LOCK :
				      if (DEBUG!=0) printf("LOCK called by process %d\n",cur_proc); 
              return LOCKED;

      case UNLOCK : 
    					if (DEBUG!=0) printf("UNLOCK called\n");
              return UNLOCKED;

      case HALT :
              printf("HALT called by process %d\n\n",cur_proc);
              //print_gmem();
              //print_register(reg);
              HALTED=1;
              *terminate = 1;
              //exit(0);
            break;
                  
      case JFALSE :
              tmp=pop(stack, cur_proc,sp, 74);
              if(i==PAGESIZE-1 || i ==PAGESIZE*2-1)//This is the Boundary between every page.
                tmp2=mem[cur_proc][i];	
              else
                tmp2=mem[cur_proc][i+1];
              if(DBGCPU) printf("jfalse %d %d \n", tmp,tmp2-1);
              if(tmp==0)
                next_instruct[cur_proc]=tmp2-1;//sub one for PC in executeit()
              else
                next_inst[cur_proc]++;
            break;

      case JMP: 
              if(i==PAGESIZE-1 || i == PAGESIZE*2-1)//This is the Boundary between every page.
                tmp=mem[cur_proc][i];
              else
                tmp=mem[cur_proc][i+1];
              next_instruct[cur_proc]=tmp-1;//sub one for PC in executeit() 
              if(DBGCPU) printf("%04d:  JMP\t %d\n", i, next_instruct[cur_proc]); 
              // next_inst[cur_proc]++;
            break;

      default:
              printf("illegal instruction mem[%d][%d]\n",cur_proc,i);
              keyhit(127);
              printf("(%04d:   %d)\n", i, mem[cur_proc][i]);  
            break;

   }
   //printf("returning NORMAL\n"); // keyhit(9999);
   return NORMAL;
}


int peek(int stack[][STACKSIZE], int proc_id, int sp[], int offset)
{
  int val= stack[proc_id][sp[proc_id] + offset];
  if (DEBUG !=0)
    printf("peek : %d\n", val);
  return val;
}


int pop(int stack[][STACKSIZE], int proc_id, int sp[], int calledfrom)
{
  int val= stack[proc_id][sp[proc_id]];
  sp[proc_id]--;
  if(sp[proc_id]<-1)
  {
    printf("Stack Underflow: process %d %d\n",proc_id,sp[proc_id]);
    printf("Called from  %d\n", calledfrom);
    exit(-1);
  }
  return val;
}


void push(int stack[][STACKSIZE], int proc_id, int sp[],int data, int calledfrom)
{
  sp[proc_id]++;

  if(sp[proc_id]>STACKSIZE)
  {
    printf("Stack Overflow: process %d %d %d\n",proc_id,sp[proc_id],data);
    printf("PUSH Called from  %d\n", calledfrom);
    exit(-1);
  }
  stack[proc_id][sp[proc_id]]=data;
}


//debug routines
void print_stack(int stack[][STACKSIZE],int sp[])
{
  int i,j;
  for(i=0;i<pid;i++)
  {
    printf("Stack contents for process %d\n", i);
    for(j=0;j<STACKSIZE;j++)
      printf("%d\n",stack[i][j]);
    printf("SP at %d\n\n",sp[i]);
  }
}


void print_gmem()
{
  int i;
  printf("Global memory: size %d\n",MAXGMEM);
  for(i=0;i<MAXGMEM;i++)
  {
    printf("%d  ", gmem[i]);
    //gmem[i]=0;
  }
  printf("\n");
}


void initGmem()
{
  int i;
  //printf("Global memory: size %d\n",MAXGMEM);
  for(i=0;i<MAXGMEM;i++)
  {
    //printf("%d  ", gmem[i]);
    gmem[i]=0;
  }
  //printf("\n");
}


void print_register(int reg[][REGISTERSIZE])
{
  int i,j;
  printf("--------------------------------------------------\n");
  printf("-                Register data                   -\n");
  printf("--------------------------------------------------\n");
  for(i=0;i<pid;i++)
  {
    printf("- Process %d: ",i);
    for(j=0;j<REGISTERSIZE;j++)
    {
      printf("%d  ",reg[i][j]);
      //reg[i][j]=0;
    }
    printf("      -\n");
  }
  printf("--------------------------------------------------\n");
}
/*End Isaac Execute*/


void resetMemory()
{
	int i,j;

	for(i=0;i<pid;i++)
	{
		for(j=0; j < REGISTERSIZE; j++)
			reg[i][j]=0;
	}
	for(i=0; i < MAXGMEM; i++)
		gmem[i]=0;
}



