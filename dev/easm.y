%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


char *yytext;
FILE *yyin;
FILE *outfile;
int yy_flex_debug;
int execute;

void showsymtable();
void showjsym();
int searchvtable( char *str, int *j, int from);
void showmem();
void executeit();

//int nest;

/*
int istack[10];
int istackptr=0;
int ijump=0;
int wstack[10];
int wstackptr=0;
int wjump=0;
int estack[10];
int estackptr=0;
int ejump=0;
*/


typedef struct
{ char labelname[4];
  int  pid;
  int  memloc;
  int  symloc;
} ILABEL;

typedef struct 
{ char label[5];
  int address;
} JSYM;

typedef struct 
{ char varname[11];
  int address;
} VTABLE;

VTABLE vtable[50];  // variable/address name table
int vtablex=0;
int vaddress=0;

ILABEL ilabel[10];  // store mem loc for label
int ilabelindex = 0;
int initdataindex = 0;

int initdatasection[100];
int index2initdata[20];
int index2index=0;

JSYM locjsym[30];
JSYM refjsym[30];
int refex=0;
int locex=0;

typedef struct 
{ char idop[11];
  int type;
} PARSE;

PARSE parse[100];
//PARSE parsecopy[20];
//int copysindex=0;
//int ifstck[10];

/* #define GMAX 30
char glovars[GMAX][11];
char locvars[GMAX][11];
unsigned int nglob;
*/

int yylex(void);
void yyerror(char *);


#define keyhit(a) {printf("hit enter --(%d)", a); getchar();}


/*
char storage[11];
char *gid[11];
char assID[11];
char prevID[11];
char optype[3];
int assIDon = 1;
*/

int i,j,kkkk;

int  sindex = 0;
int  pindex = 0;
int  memloc = 0;

#define MAXPRO 1   //max num of processes
#define MAXMEM 200 //max size of a process
#define STACKSIZE 100 //max size of the stack
#define REGISTERSIZE 10 //size of each process registers
#define MAXGMEM 20 //max size of global memory
#define NORMAL 0 //denotes a normal return from exe()
#define LOCKED 1 //stops process switching until unlock
#define UNLOCKED 2 //remove lock
#define ENDPROCESS 3
#define p0WRITE 4 //tells p0 to run-p0 should only run after a write to gmem
int  gmem[MAXGMEM];   //global var sit here 
int mem[MAXPRO][MAXMEM]; 
int endprog[MAXPRO]; // last instruction of proc
int  jsym[60]; 
int pid = 0;  //process id

  int p0running;


//execute one instruction return 0 if instruction is not lock
//int exe(int **stack,int sp[], int next_inst, int cur_proc);
int exe(int stack[][STACKSIZE],int sp[],int reg[][REGISTERSIZE], int next_instruct[],int next_inst[], int cur_proc);
int pop(int stack[][STACKSIZE], int proc_id, int sp[], int calledfrom);
void push(int stack[][STACKSIZE], int proc_id, int sp[],int data, int calledfrom);
int peek(int stack[][STACKSIZE], int proc_id, int sp[], int offset);

void print_stack(int stack[][STACKSIZE],int sp[]); //debug
void print_register(int reg[][REGISTERSIZE]); //debug
void print_gmem();





int showparse( int beg)
{ int i;
        for (i=beg; i<sindex; i++)
        {  printf("############## i=%d  %s, (type)%d\n",
                                i, parse[i].idop, parse[i].type);
        }
        printf("end show\n");
}

#if 0
int termcopy(int beg)
{ int i, j;
         printf("termcopy: storage loc calc here\n");
         j = 0;        
         strcpy(parsecopy[j].idop, parse[beg].idop);
         parsecopy[j].type = parse[beg].type;
 
printf("%s  ->   %s\n", parse[beg].idop, parsecopy[beg].idop);

         if(parse[beg+1].idop[0] == '[')
         { i= beg+1; j++;
           do { /* append parse[i].idop to tgt */
             strcpy(parsecopy[j].idop, parse[i].idop);
printf("%s  ->   %s\n", parse[i].idop, parsecopy[j].idop);
             parsecopy[j].type = parse[beg].type;
             j++;
           } while (parse[i++].idop[0] != ']');
         }
         copysindex = j;
}
#endif

int showterm( int beg)
{ int i;
        return 0;

        for (i=beg; i<sindex; i++)
        {
        }
}


%}

%union
{int intval;
 char *sptr;
}

%token NUMBER
%token ID
%token LD 
%token LA 
%token LOAD 
%token LOADI 
%token ADD
%token SUB 
%token MUL 
%token DIV 
%token END
%token ENDP
%token PROCESS
%token AND
%token OR
%token NOT
%token LE_OP
%token GE_OP
%token LT_OP
%token GT_OP
%token EQ_OP
%token NE_OP
%token STOP 
%token STOR
%token ST 
%token DATA 
%token IDATA 
%token COMMA 
%token COLON 
%token LOCK 
%token UNLOCK 

%token OPEN 
%token READ 
%token WRITE 
%token CLOSE 
%token SEEK 
%token POPD 
%token POP 

%token DOT 
%token HALT 
%token JFALSE 
%token JMP 

%left  LPREC 
%right RPREC 

%%

programA: program
         {int j,k;
            printf("END of ProgramA\n");
          for(j=0; j<ilabelindex; j++)
          {
           printf("pid = %d\n", ilabel[j].pid );
           printf("memloc = %d\n", ilabel[j].memloc);
           printf("name = %s\n", ilabel[j].labelname);
          }
         }
        ;
program:   program0 
         | varlistend  program0  {; }
         | program  program0  {;}
        ;

varlistend:
          dotdata varlists
          {;
          }
        ;

dotdata:
          DOT DATA 
          { sindex=0;
          }
        ;

varlists:
           varlists varlist
         | varlist
        ;

varlist:   term 
              { int i;
                //for (i=0; i<sindex; i++)
                i=0;
                { 
                   strcpy(vtable[vtablex].varname, parse[i].idop);
                   vtable[vtablex].address=vaddress;
                   vtablex++;
                   vaddress++;

showsymtable();
//keyhit(9);

                }
                gmem[memloc++]=0;
                //showterm(0);
                sindex = 0;
              }
         | term COMMA term 
              { int i,j;

printf("term comma term\n");
showparse(0);
printf("vtablex=%d vaddress:%d\n", vtablex, vaddress);

                strcpy(vtable[vtablex].varname, parse[0].idop);

                vtable[vtablex].address=vaddress;
                sscanf(parse[2].idop, "%d", &i);

printf("vtable[%d].varname=%s, parse[0].idop=%s\n", 
    vtablex, vtable[vtablex].varname,    parse[0].idop);
printf(" vtable[vtablex].address=%d\n", vtablex, vtable[vtablex].address);
 //keyhit(99);

                vaddress += i;
                vtablex++;

showsymtable();
printf("vtablex=%d vaddress:%d\n", vtablex, vaddress);
 //keyhit(10);

                for(j=0; j<i; j++)
                {  gmem[memloc++]=0;
                }
                //showterm(0);
                sindex = 0;
              }
        ;

program0:   program1
          | program1 initdata
        ; 

initdata: dotidata labels
         { //printf("inidata\n");
           //showparse(0);
         }
          |
          dotidata
         {
         }
        ;
dotidata: DOT IDATA
         { sindex=0;
         }
        ;

labels:   labels labl collabeldata
          | labl collabeldata
         {// printf("labels\n");
           //showparse(0);
         }
        ;

labl: term
         { //printf("labl\n");
           //showparse(0);
         }
        ;

collabeldata: COLON term
         { int i, j, k;
           int ch; 
           char *chptr;

           printf("collabeldata\n");
           showparse(0);
  printf("==================================\n");
   printf("parse[0].idop=%s\n", parse[0].idop);
   printf("parse[2].idop=%s\n", parse[2].idop);

          for(j=0; j<ilabelindex; j++)
          {
           printf("pid = %d\n", ilabel[j].pid );
           printf("memloc = %d\n", ilabel[j].memloc);
           printf("name = %s\n", ilabel[j].labelname);

           if( ilabel[j].memloc != -999)
           { printf("compare ilabel's %s  and label parsed %s\n",
             ilabel[j].labelname,  parse[0].idop); 
             if( strcmp(ilabel[j].labelname ,  parse[0].idop)==0)
             { printf("label found, endprog is at  %d\n",
                  endprog[ ilabel[j].pid] );

               chptr =  parse[2].idop;
               for( ; *chptr; chptr++) 
               { printf("%c", *chptr);
               }
                printf("\n");

               chptr =  parse[2].idop;

               /* location of label */
               mem[ ilabel[j].pid][ ilabel[j].memloc ] = 
                                                  endprog[ilabel[j].pid];
               ilabel[j].symloc  = endprog[ilabel[j].pid];

               for( ; *chptr; chptr++) 
               { printf("at %d: locating '%c'\n", 
                  endprog[ilabel[j].pid], *chptr);
                  mem[ilabel[j].pid][endprog[ilabel[j].pid]] = *chptr;
                  endprog[ilabel[j].pid]++;
               }
               mem[ ilabel[j].pid][endprog[ilabel[j].pid]] = 0;
               endprog[ilabel[j].pid]++;
               ilabel[j].memloc = -999;
             } 
           }

          }


  printf("==================================\n");
  printf("==================================\n");
for(j=0; j<ilabelindex; j++)
printf("pid = %d  end lo = %d\n", ilabel[j].pid, endprog[ilabel[j].pid]);
  printf("==================================\n");

   // allocate parse[2].idop to initdatasection[]
   // and backpatch Li labels. 
//ILABEL ilabel[10];  // store mem loc for label
//int ilabelindex = 0;
/*typedef struct
{ char labelname[4];
  int  pid;
  int  memloc;
} ILABEL;
*/      

       printf("index2initdata[%d] =  %d\n", index2index, initdataindex);       
       index2initdata[index2index]=initdataindex;       
       for (k=0; ch = *((char *)(parse[2].idop)+k); k++)
        initdatasection[initdataindex++]= ch;
       index2index++;
 
        for(k=0; k<initdataindex; k++)
             printf("%c", initdatasection[k]);
        printf("\n");
        for(k=0; k<index2index; k++)
             printf("%d  ", index2initdata[k]);
  printf("\n==================================\n");
            sindex = 0;
         }
        ;


program1:   process0 code0 END 
               {
                 mem[pid][memloc++]=END;
                 sindex=0;
                 endprog[pid]=memloc; // last instruction location+1
printf("at END refex=%d locex=%d\n", refex, locex);
                 showjsym(); 
                 refex = locex = 0;
                 pid++;
               }
        ;

process0:
           PROCESS term 
              {
                //printf("PROC\n");
                showterm(0);
                sindex = 0; 
                memloc = 10;
                if(pid>MAXPRO) 
                   {fprintf(stderr,"max num of proccesses currently 6\n");
                    exit(1);
                   }
              }

        ;

code0:
         code0 code
         | code
      ;

code:    LOAD term
           {int i, j;
            mem[pid][memloc++]=LOAD;
             i=searchvtable(parse[0].idop, &j, 1);
             if( i>=230 )
             {
               mem[pid][memloc++]=i;
             }
             else
             {
       printf("vtable[%d].address=%d\n",i, vtable[i].address);
    //   keyhit(543);
               mem[pid][memloc++]=i; //kosuke 3210-a
             }
             showterm(0);
             sindex = 0;
           }
       | LOADI term 
           {int i;
            mem[pid][memloc++]=LOADI;
             sscanf(parse[0].idop, "%d", &i);
            mem[pid][memloc++]=i;
             showterm(0);
             sindex = 0;
           }
       | LA term 
           { int i, j;
            mem[pid][memloc++]=LA;
printf("LA term parse[0].idop = %s\n",parse[0].idop); 
             i=searchvtable(parse[0].idop, &j, 2);
printf("LA term searchvtable ret val= %d\n", i); 
//keyhit(645);
             if( i == -999)
             { ilabel[ilabelindex].pid = pid;
               ilabel[ilabelindex].memloc = memloc;
               sprintf(ilabel[ilabelindex].labelname,
                 "%s",  parse[0].idop);
               ilabelindex++;
               mem[pid][memloc++]= -999;
printf("wrote mem[%d][%d] =  %d\n", pid, memloc-1, mem[pid][memloc-1]); 
/**
{ int i,j;
  printf("ilabelindex=%d\n",ilabelindex);
          for(j=0; j<ilabelindex; j++)
          {
           printf("pid = %d\n", ilabel[j].pid );
           printf("memloc = %d\n", ilabel[j].memloc);
           printf("name = %s\n", ilabel[j].labelname);
          }
          keyhit(645);
}
**/
             }
             else
             {
               //mem[pid][memloc++]=j;
               mem[pid][memloc++]=i;
             }
             showterm(0);
             sindex = 0;
           }
       | OPEN
          {
            mem[pid][memloc++]=OPEN;
                showterm(0);
                sindex = 0;
          }
       | READ
          {
            mem[pid][memloc++]=READ;
                showterm(0);
                sindex = 0;
          }
       | WRITE
          {
            mem[pid][memloc++]=WRITE;
                showterm(0);
                sindex = 0;
          }
       | CLOSE
          {
            mem[pid][memloc++]=CLOSE;
                showterm(0);
                sindex = 0;
          }
       | SEEK
          {
            mem[pid][memloc++]=SEEK;
                showterm(0);
                sindex = 0;
          }
       | POPD term
           {int i, j;
                showterm(0);
            mem[pid][memloc++]=POPD;
             i=searchvtable(parse[0].idop, &j, 31);
             if( i>=230 )
             { // 230 and up is local var r?
               mem[pid][memloc++]=i;
             }
             else
             {
               mem[pid][memloc++]=vtable[i].address;
             }

                showterm(0);
                sindex = 0;
           }
       | POP
          {
            mem[pid][memloc++]=POP;
                showterm(0);
                sindex = 0;
          }

       | JFALSE term 
           { 
            mem[pid][memloc++]=JFALSE;
            mem[pid][memloc]=-1;
             //printf("label =  %s  %d\n", yylval.sptr, sindex);  
                refjsym[refex].address = memloc++;
                sprintf( refjsym[refex].label, "%s", parse[0].idop);
                refex++;
             showterm(0);
             sindex = 0;
           }
       | JMP term 
           {
            mem[pid][memloc++]=JMP;
            // printf("label =  %s  %d\n", yylval.sptr, sindex);  
            mem[pid][memloc]=-1;
                refjsym[refex].address = memloc++;
                sprintf( refjsym[refex].label, "%s", parse[0].idop);
                refex++;
                showterm(0);
                sindex = 0;
           }
       | LOCK  
           {
            mem[pid][memloc++]=LOCK;
                showterm(0);
                sindex = 0;
           }
       | UNLOCK  
           {
            mem[pid][memloc++]=UNLOCK;
                showterm(0);
                sindex = 0;
           }
       | LD  
           {
            mem[pid][memloc++]=LD;
                showterm(0);
                sindex = 0;
           }
       | ST  
           {
            mem[pid][memloc++]=ST;
                showterm(0);
                sindex = 0;
           }
       | STOR term  
           {int i, j;
                showterm(0);
            mem[pid][memloc++]=STOR;
             i=searchvtable(parse[0].idop, &j, 3);
//printf("STOR %d\n", i); keyhit(89);
             if( i>=230 )
             { // 230 and up is local var r?
               mem[pid][memloc++]=i;
             }
             else
             {
               //mem[pid][memloc++]=vtable[i].address;
               mem[pid][memloc++]=i;
             }

                showterm(0);
                sindex = 0;
           }
       | DIV 
           {
            mem[pid][memloc++]=DIV;
                showterm(0);
                sindex = 0;
           }
       | MUL 
           {
            mem[pid][memloc++]=MUL;
                showterm(0);
                sindex = 0;
           }
       | NOT 
           {
            mem[pid][memloc++]=NOT;
                showterm(0);
                sindex = 0;
           }
       | NE_OP 
           {
            mem[pid][memloc++]=NE_OP;
                showterm(0);
                sindex = 0;
           }
       | EQ_OP 
           {
            mem[pid][memloc++]=EQ_OP;
                showterm(0);
                sindex = 0;
           }
       | GT_OP 
           { 
            mem[pid][memloc++]=GT_OP;
                showterm(0);
                sindex = 0;
           }
       | GE_OP 
           {
            mem[pid][memloc++]=GE_OP;
                showterm(0);
                sindex = 0;
           }
       | LT_OP 
           {
            mem[pid][memloc++]=LT_OP;
                showterm(0);
                sindex = 0;
           }
       | LE_OP 
           {
            mem[pid][memloc++]=LE_OP;
                showterm(0);
                sindex = 0;
           }
       | ADD 
           {
            mem[pid][memloc++]=ADD;
                showterm(0);
                sindex = 0;
           }
       | SUB 
           {
            mem[pid][memloc++]=SUB;
                showterm(0);
                sindex = 0;
           }
       | AND 
           {
            mem[pid][memloc++]=AND;
                showterm(0);
                sindex = 0;
           }
       | OR 
           {
            mem[pid][memloc++]=OR;
                showterm(0);
                sindex = 0;
           }
       | HALT 
           {
            mem[pid][memloc++]=HALT;
                showterm(0);
                sindex = 0;
           }
       | term COLON 
           {
                locjsym[locex].address = memloc;
                sprintf( locjsym[locex].label, "%s", parse[0].idop);
                locex++;
                showterm(0);
                sindex = 0;
           }
      ;


term:   
         ID
            {
              sprintf( parse[sindex].idop, "%s", yylval.sptr);
              parse[sindex].type = ID;
              sindex++;


            }
        | NUMBER 
            {
              sprintf( parse[sindex].idop, "%s", yylval.sptr);
              parse[sindex].type = NUMBER;
              sindex++;
            }

        ;



%%

void yyerror(char *s) {
 extern unsigned int linenumber;
    fprintf(stderr, "%s\n", s);
    fprintf(stderr, "line %d:  %s\n", linenumber+1, yytext);
    return;
}

int main(int argc, char **argv )
{
    execute=0;
    
     //if( argc != 3 )
    //{ fprintf(stderr, "bad command\n");
    //  exit(0);  //  yyin = stdin;
    //}

    if (argc==3)
    {
		outfile = fopen( argv[2], "w");
	}else
	{
		printf("bad command?\n");
    }

    //if(*argv[2]== 'e') execute=1;
    //else if(*argv[2]== 'm') execute=0;
    //else 
    //{ fprintf(stderr, "bad command\n");
    //  exit(0);  //  yyin = stdin;
    //}

    yyin = fopen( argv[1], "r" );

    yyparse();

 fclose(yyin);

  //if( execute==0 )
  //{   showjsym(); 
    //  showsymtable();
     showmem();
  //}
  //else
  //{   showjsym(); 
  //    executeit();
     // showmem();
  //}
  fclose(outfile);
  return 0;
}


void showsymtable()
{int i;
     for (i=0; i<vtablex; i++)
     {  printf("%d:  %s\n",
        vtable[i].address,  vtable[i].varname);
     }
}

int searchvtable( char *str, int *j, int from)
{int i, found;


printf("--------search vtable for %s  from=%d\n", str, from);
printf("%c %c\n", *str, *(str+1));
//keyhit(888);

           if( *str =='r' &&
              ( '0' <= *(str+1) && *(str+1) <= '9' ))
           { 
printf("this is local var\n");
                 // 230 or above is locar var, r?
                 return( (*(str+1) - '0')+230 ); 

           }

             for(i=found=0; i<vtablex; i++)
             {
//printf("comp against : %s\n", vtable[i].varname);
               if(strcmp(vtable[i].varname, str)==0)
               { found =1; *j = i; break;}
             }
             if(found)
             {;
             }
             else
             {
              // fprintf(stderr, "%d: var %s not found\n", from, str );
               return (-999);
             }
printf("---------search vtable for %s ended\n", str);
printf("----------returning %d   and  j= %d\n", vtable[i].address, *j);
   return( vtable[i].address);
}

void showjsym()
{ int i, j;
printf("SHOWSYM %d\n", execute);
  for(i=0; i<locex; i++)
  {if(execute==0) printf("%s  %d\n",
        locjsym[i].label, locjsym[i].address );
  }

printf("SHOWSYM refex %d\n", refex);
  for(i=0; i<refex; i++)
  {if(execute==0) printf("%s  %d", refjsym[i].label, refjsym[i].address );
   if(execute==0) printf(" --  %d\n", mem[pid][ refjsym[i].address] );
  }
    /* search refjsym in locjsym, fill with locjsym.address */  
  for(i=0; i<refex; i++)
  {
    printf("%s  %d", refjsym[i].label, refjsym[i].address );
    printf(" --  %d\n", mem[pid][ refjsym[i].address] );
    /* search refjsym in locjsym, fill with locjsym.address */  
    if( mem[pid][ refjsym[i].address] != -1)
    if(execute==0)   printf("jump lable wrong %s\n", refjsym[i].label);
    for(j=0; j<locex; j++)
   {
   //   printf("loc:%s  ref:%s\n",  locjsym[j].label,  refjsym[i].label);

      if( strcmp(locjsym[j].label,  refjsym[i].label)== 0 )
      {// printf("patch = %d\n", locjsym[j].address);
        mem[pid][ refjsym[i].address] = locjsym[j].address; 
        break;
      }
    }
    if(j>=locex)
     if(execute==0) fprintf(stderr,"lable not found: %s\n", refjsym[i].label);
  }
printf("DONE showsym\n");  
}

int searchsym( char *str, int j, int where)
{int i;

#if 0
          { int i, k;
            printf("look for loc %s j=%d\n", str, j);
            for (i=0; i<vtablex; i++)
            {  printf("vtable[%d].addres= %d .EQ. j=%d , (%s)\n",
                    i, vtable[i].address, j,  vtable[i].varname);
            }
          }

     printf("that's end of vtable\n");          
     printf("where = %d\n", where);
     keyhit(1122);
#endif

             for(i=0; i<vtablex; i++)
             {
//printf("searchsym::: comp %d against : %d\n",  j, vtable[i].address);
               if(vtable[i].address == j )
               { sscanf(vtable[i].varname, "%s", str);
                 return 0; 
               }
             }
            /***
             { fprintf(stderr, " symbol %s not found, \n", str);
               fprintf(stderr, " this may be initdata label.\n");
             }
            ***/

     { int i;
       //printf("ilabelindex=%d\n",ilabelindex);
          for(i=0; i<ilabelindex; i++)
          {
          /**
           printf("pid = %d\n", ilabel[i].pid );
           printf("memloc = %d\n", ilabel[i].memloc);
           printf("symloc = %d ? %d\n", ilabel[i].symloc, j);
           printf("name = %s\n", ilabel[i].labelname);
         /**/
            if(ilabel[i].symloc == j )
             {
               sscanf( ilabel[i].labelname,"%s",str);
              // printf("(%s) found!\n", str); 
               return 0;
             }
          }
         // keyhit(645);
     }


fprintf(stderr, "--- symbol/label '%s' not found, ERROR!!\n", str);
    return -1;
}


void showmem()
{ int i, k; char name[11];
  int progid;
  

for(k=0; k<pid; k++)
{
printf("############### pid %d ############\n", k);
printf("############### endprog %d ############\n", endprog[k]);
//fprintf(outfile,"%d\n", k);
//fprintf(outfile,"%d\n", endprog[k]); //Printing out how big a process is.
            // endprog[] contains last instruction addrs of proc
  for(i=0; i<endprog[k]; i++)
  { // printf("%04d:    %d\n", i,   mem[k][i]);
    switch (mem[k][i] )
    {
/*
case NUMBER:
     printf("  %d\n",  mem[k][i]); 
     break;
case ID:
     printf("  %d\n",  mem[k][i]); 
     break;
*/
case POPD :
     if(mem[k][i+1]<230)
     { if(searchsym(name, mem[k][i+1], 0)== -1)
             printf("ERROR, lable not found!\n");
       printf("%04d:  POPD\t %d  (%s)\n", i, mem[k][i+1], name);
		fprintf(outfile,"%d\n%d\n", POPD ,mem[k][i+1]);
	 }
     else{
       printf("%04d:  POPD\t %d\n", i, mem[k][i+1]); 
	   fprintf(outfile,"%d\n%d\n", POPD ,mem[k][i+1]);
	 }
     i++;
     break;
case POP :
     printf("%04d:  POP\n", i); 
	 fprintf(outfile,"%d\n", POP);
     break;
case OPEN :
     printf("%04d:  OPEN\n", i); 
	 fprintf(outfile,"%d\n", OPEN);
     break;
case READ :
     printf("%04d:  READ\n", i); 
	 fprintf(outfile,"%d\n", READ);
     break;
case WRITE :
     printf("%04d:  WRITE\n", i);
	 fprintf(outfile,"%d\n", WRITE);
     break;
case SEEK :
     printf("%04d:  SEEK\n", i); 
	 fprintf(outfile,"%d\n", SEEK);
     break;
case CLOSE :
     printf("%04d:  CLOSE\n", i); 
	 fprintf(outfile,"%d\n", CLOSE);
     break;
case LD :
     printf("%04d:  LD\n", i); 
	 fprintf(outfile,"%d\n", LD);
     break;
case LA :
     if(searchsym(name, mem[k][i+1], 1)== -1)
             printf("ERROR, lable not found!\n");
     printf("%04d:  LA\t %d (%s)\n", i, mem[k][i+1], name); 
     fprintf(outfile,"%d\n%d\n", LA ,mem[k][i+1]);
	 i++;
	 break;
case LOAD :
     if(mem[k][i+1]<230)
     { 
       if(searchsym(name, mem[k][i+1], 2)== -1)
             printf("ERROR, lable not found!\n");
       printf("%04d:  LOAD\t %d  (%s)\n", i, mem[k][i+1], name);
	   fprintf(outfile,"%d\n%d\n", LOAD ,mem[k][i+1]);// it should not comment out, but it has some problem
	 }
     else
	 {
       printf("%04d:  LOAD\t %d\n", i, mem[k][i+1]);
	   fprintf(outfile,"%d\n%d\n",LOAD, mem[k][i+1]);
	 }
     i++;
     break;
case LOADI :
     printf("%04d:  LOADI\t %d\n", i, mem[k][i+1]); 
     fprintf(outfile,"%d\n%d\n", LOADI ,mem[k][i+1]);
     i++;
      break;
case ADD:
     printf("%04d:  ADD\n", i); 
     fprintf(outfile,"%d\n", ADD);
     break;
case SUB :
     printf("%04d:  SUB\n", i); 
     fprintf(outfile,"%d\n",SUB);
     break;
case MUL :
     printf("%04d:  MUL\n", i); 
     fprintf(outfile,"%d\n",MUL); 
     break;
case DIV :
     printf("%04d:  DIV\n", i); 
     fprintf(outfile,"%d\n",DIV); 
     break;
case END:
     printf("%04d:  END\n", i); 
     fprintf(outfile,"%d\n", END); 
     break;
case ENDP:
     printf("%04d:  ENDP\n", i); 
     fprintf(outfile,"%d\n", ENDP);
     break;
case AND:
     printf("%04d:  AND\n", i); 
     fprintf(outfile,"%d\n", AND); 
     break;
case OR:
     printf("%04d:  OR\n", i); 
     fprintf(outfile,"%d\n", OR); 
     break;
case NOT:
     printf("%04d:  NOT\n", i); 
     fprintf(outfile,"%d\n", NOT);
     break;
case LE_OP:
     printf("%04d:  LE_OP\n", i); 
     fprintf(outfile,"%d\n", LE_OP); 
     break;
case GE_OP:
     printf("%04d:  GE_OP\n", i); 
     fprintf(outfile,"%d\n", GE_OP); 
     break;
case LT_OP:
     printf("%04d:  LT_OP\n", i); 
     fprintf(outfile,"%d\n", LT_OP); 
     break;
case GT_OP:
     printf("%04d:  GT_OP\n", i); 
     fprintf(outfile,"%d\n", GT_OP); 
     break;
case EQ_OP:
     printf("%04d:  EQ_OP\n", i); 
     fprintf(outfile,"%d\n", EQ_OP); 
     break;
case NE_OP:
     printf("%04d:  NE_OP\n", i); 
     fprintf(outfile,"%d\n", NE_OP); 
     break;
case STOP :
     printf("%04d:  STOP\n", i); 
     fprintf(outfile,"%d\n",STOP); 
     break;
case STOR:
     if(mem[k][i+1]<230)
     { if(searchsym(name, mem[k][i+1], 3)== -1)
             printf("ERROR, lable not found!\n");
       printf("%04d:  STOR\t %d  (%s)\n", i, mem[k][i+1], name);
       fprintf(outfile,"%d\n%d\n", STOR,mem[k][i+1]);
     }
     else {
       printf("%04d:  STOR\t %d\n", i, mem[k][i+1]); 
       fprintf(outfile,"%d\n%d\n", STOR ,mem[k][i+1]); }
     i++;
     break;
case ST :
     printf("%04d:  ST\n", i); 
     fprintf(outfile,"%d\n",ST); 
     break;
case LOCK :
     printf("%04d:  LOCK\n", i); 
     fprintf(outfile,"%d\n", LOCK); 
     break;
case UNLOCK :
     printf("%04d:  UNLOCK\n", i); 
     fprintf(outfile,"%d\n",UNLOCK); 
     break;
case HALT :
     printf("%04d:  HALT\n", i); 
     fprintf(outfile,"%d\n",HALT); 
     break;
case JFALSE :
     printf("%04d:  JFALSE\t %d\n",  i, mem[k][i+1]); 
     fprintf(outfile,"%d\n%d\n", JFALSE ,mem[k][i+1]);
     i++; 
     break;
case JMP: 
     printf("%04d:  JMP\t %d\n", i, mem[k][i+1]); 
     fprintf(outfile,"%d\n%d\n",JMP, mem[k][i+1]);
     i++; 
     break;
default:
     printf("(%04d:   %d)\n", i, mem[k][i]);  //I commented these two lines out...
     fprintf(outfile,"%d\n", mem[k][i]);
     break;
     }
  }
} // end of printing each proc memory

}

void executeit()
{
  int cur_proc,p0=0, msg=-1,m;
  int stack[MAXPRO][STACKSIZE];
  int sp[MAXPRO];
  int next_instruct[MAXPRO];
  int proc_complete[MAXPRO];
  int reg[MAXPRO][REGISTERSIZE];
  int locked=UNLOCKED;

  memset(stack,0,MAXPRO*STACKSIZE*sizeof(int));
  memset(sp,-1,MAXPRO*sizeof(int));
 // memset(next_instruct,0,MAXPRO*sizeof(int));
  memset(proc_complete,0,MAXPRO*sizeof(int));
  memset(reg,0,10*MAXPRO*sizeof(int));
  srand( time(NULL) );

  next_instruct[0]=10;

  /*for(i=0;i<pid;i++)
   for(m=0;m<STACKSIZE;m++)
      printf("STACK %d: %d\n",i,stack[i][m]);*/

/*while(1) //used for testing a single process
{
   cur_proc=2;
   if(next_instruct[cur_proc]<endprog[cur_proc])
   {
       msg=exe(stack,sp,reg,next_instruct,next_instruct,cur_proc);
       //increment next_instruction
      next_instruct[cur_proc]++;
      printf("loop: %d\n",next_instruct[cur_proc]);
   }
   else break;
}*/

   while(1)
   {
cont:
      if(locked==UNLOCKED)
       {// printf("pid=%d\n", pid); //keyhit(8999);
           cur_proc = (pid==1)? 0:(rand()%(pid-1))+1;
       }

      if(proc_complete[cur_proc] == 1)
       {
         printf("----------------------------cur_proc: %d\n",cur_proc);
             goto checkdone;
       }

      if(next_instruct[cur_proc]<endprog[cur_proc])
      {
         msg=exe(stack,sp,reg,next_instruct,next_instruct,cur_proc);
         if(msg==ENDPROCESS)
            proc_complete[cur_proc]=1;

    //    printf("%d %d\n",cur_proc,next_instruct[cur_proc]+1);
         //increment next_instruction
       else
         next_instruct[cur_proc]++;

         if(msg==UNLOCKED)
         {
//printf("unlock\n");
            locked=UNLOCKED;
         }
         else if(msg==LOCKED || locked==LOCKED)
         {//printf("locked\n");
            locked=LOCKED;
         }
         

        //run p0 in its entirety after a gmem write
         //cur_proc=0;
         while(msg==p0WRITE || p0running)
         {
            p0running=1; cur_proc=0;
//printf("p0 started   PC=%d\n", next_instruct[cur_proc]);
            msg=exe(stack,sp,reg,next_instruct,next_instruct,p0);
//   printf("p1, nextPC=%d\n" , next_instruct[1]);

            next_instruct[cur_proc]++;
            if(p0running == 0)
            {  msg=NORMAL;
               next_instruct[p0]=10;
               break;
            }
//printf("branch %d \n",(next_instruct[cur_proc]<endprog[cur_proc]));
            if( next_instruct[p0]>=endprog[p0])
            {  p0running=0;
               sp[p0]=0;
               next_instruct[p0]=10;
               msg=NORMAL;
               break;
            }
         }
         continue;
      }
      else
      {
           //printf("Process %d complete\n",cur_proc);
           proc_complete[cur_proc]=1;
      }
      //check if all processes are done
checkdone:
    for(cur_proc=1;cur_proc<pid;cur_proc++)
      if(proc_complete[cur_proc]==0)
         goto cont;
     break;
   }
  // print_stack(stack,sp); stack should be all 0 and sp at -1
   print_gmem();
   print_register(reg);
}

int exe(int stack[][STACKSIZE],int sp[],int reg[][REGISTERSIZE], int next_instruct[],int next_inst[], int cur_proc)
{
   int i,k, m; //delete these after all accesses renamed, except i
   int tmp,tmp1, tmp2;
   char name[11];

   i=next_inst[cur_proc];
//printf("ENTER exe    ---  (pid=%d) PC = %d\n", cur_proc, i);
//printf("Instruction = %d\n", mem[cur_proc][i] );
//gmem[6]=101;

/** the following 3 lines are for debugging user program too **/
#if 0
   print_gmem();
   print_register(reg);
   keyhit(343);
#endif

   switch (mem[cur_proc][i] )
   {
     /** OPEN, READ, CLOSE, WRITE, SEEK ::  OS services **/
      case OPEN :
                  tmp = peek(stack,cur_proc,sp, 0) ;
                  printf("OPEN offset=  0,  data=%d\n", tmp); 
                  tmp1 = peek(stack,cur_proc,sp, -1) ;
                  printf("OPEN offset= -1,  data=%d\n", tmp1); 

                 { int i;
                 i=0;
                 while ( name[i] =  mem[cur_proc][tmp1+i++] );
    printf("filename passed = %s\n", name);
    printf("OS service call  --- <OPEN>  return file descriptor!(987 is fake)\n");

                 push(stack,cur_proc,sp, 987, 11); // dummy fd =987 
                 }
 //keyhit(91);
                break;
      case READ :
                  tmp = peek(stack,cur_proc,sp, 0) ;
                  printf("READ,  file descriptor=%d\n", tmp); 
    printf("OS service call  --- <READ> return int read (777 is fake)\n");
                 push(stack,cur_proc,sp, 777, 13); // dummy fd =777 

                break;

      case CLOSE :
                  tmp = peek(stack,cur_proc,sp, 0) ;
                  printf("CLOSE,  file descriptor=%d\n", tmp); 
    printf("OS service call  --- <CLOSE> close file\n");

                break;
      case WRITE :
                  tmp = peek(stack,cur_proc,sp, 0) ;
                  printf("WRITE offset=  0,  data=%d\n", tmp); 
                  tmp1 = peek(stack,cur_proc,sp, -1) ;
                  printf("WRITE offset= -1,  fd =%d\n", tmp1); 
    printf("OS service call  --- <WRITE> \n");
    /* tmp = data ; tmp1 = file descriptor */
    
                break;

      case SEEK :
                  tmp = peek(stack,cur_proc,sp, 0) ;
                  printf("SEEK offset=  0,  data=%d\n", tmp); 
                  tmp1 = peek(stack,cur_proc,sp, -1) ;
                  printf("SEEK offset= -1,  fd =%d\n", tmp1); 
    printf("OS service call  --- <SEEK> \n");
    /* tmp = seek ofset ; tmp1 = file descriptor */
    
                break;



      case POPD : tmp = mem[cur_proc][i+1];
                  tmp1 = pop(stack,cur_proc,sp, 10) ;
//printf("POPD: popd %d into %d\n", tmp1, tmp);
                  if(tmp<230)
                  {   gmem[tmp]=tmp1;
                  }
                  else
                  {  tmp = tmp-230;
                     reg[cur_proc][tmp]= tmp1;
                  }
                  next_inst[cur_proc]++;
                  break;

      case POP : 
                  tmp1 = pop(stack,cur_proc,sp, 12) ;
                  break;

      case LD : tmp = pop(stack,cur_proc,sp, 14) ;
                tmp1 = gmem[tmp];
//printf("%04d LD %d %d\n",i,tmp1,tmp);
                push(stack,cur_proc,sp,tmp1, 15);
                break;

      case LA : tmp = mem[cur_proc][i+1];//load address of start of array
//printf("LA1 %d\n",tmp);
                push(stack,cur_proc,sp,tmp, 17);
//printf("%04d LA %d %d\n",i,tmp);
                  next_inst[cur_proc]++;
                break; 
      case LOAD : tmp = mem[cur_proc][i+1];
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
      case LOADI : push(stack,cur_proc,sp,mem[cur_proc][i+1], 21); 
//printf("%04d loadi %d\n",i,stack[cur_proc][sp[cur_proc]] );
                  next_inst[cur_proc]++;
                   break;
      case ADD: tmp1 = pop(stack,cur_proc,sp, 16);
                tmp2 = pop(stack,cur_proc,sp, 18);
                tmp1 += tmp2;
                push(stack,cur_proc,sp,tmp1, 23);
// printf("%04d:  ADD %d\n",i, tmp1); 
                break;
      case SUB : tmp1 = pop(stack,cur_proc,sp, 20);
                 tmp2 = pop(stack,cur_proc,sp, 22);
                 tmp1 = tmp2-tmp1;
                 push(stack,cur_proc,sp,tmp1, 25);
// printf("%04d:  SUB\n", i); 
                 break;
      case MUL: tmp1 = pop(stack,cur_proc,sp, 24);
                tmp2 = pop(stack,cur_proc,sp, 26);
                tmp1 *= tmp2;
                push(stack,cur_proc,sp,tmp1, 27);
   //  printf("%04d:  MUL\n", i); 
                break;
      case DIV : tmp1 = pop(stack,cur_proc,sp, 28);
                 tmp2 = pop(stack,cur_proc,sp, 30);
                 tmp1 = tmp2/tmp1;
                 push(stack,cur_proc,sp,tmp1, 29);
   //  printf("%04d:  DIV\n", i); 
                 break;
      case END: printf("Process %d completed normally\n", cur_proc);
                p0running=0;
                return ENDPROCESS;
     
case ENDP: printf("ENDP\n");
     break;

      case AND: tmp1 = pop(stack,cur_proc,sp, 32);
                tmp2 = pop(stack,cur_proc,sp, 34);
                tmp1 = tmp1 && tmp2;
                push(stack,cur_proc,sp,tmp1, 31);
 //  printf("%04d:  AND\n", i); 
                break;
      case OR: tmp1 = pop(stack,cur_proc,sp, 36);
               tmp2 = pop(stack,cur_proc,sp, 38);
               tmp1 = tmp1 || tmp2;
               push(stack,cur_proc,sp,tmp1, 33);
 //  printf("%04d:  OR\n", i); 
               break;
      case NOT: tmp1 = pop(stack,cur_proc,sp, 40);
                tmp1 = !tmp1;
                push(stack,cur_proc,sp,tmp1, 35);
 //  printf("%04d:  NOT\n", i); 
                break; 
      case LE_OP: tmp1 = pop(stack,cur_proc,sp, 42);
                  tmp2 = pop(stack,cur_proc,sp, 44);
                  tmp = tmp1 >= tmp2;
                  push(stack,cur_proc,sp,tmp, 37);
 //        printf("%04d:  LE_OP %d\n", i, tmp); 
                  break; 
      case GE_OP: tmp1 = pop(stack,cur_proc,sp, 46);
                  tmp2 = pop(stack,cur_proc,sp, 48);
                  tmp = tmp1 <= tmp2;
                  push(stack,cur_proc,sp,tmp, 39);
//    printf("%04d:  GE_OP%d\n", i,tmp); 
                  break; 
      case LT_OP:tmp1 = pop(stack,cur_proc,sp, 50);
                 tmp2 = pop(stack,cur_proc,sp, 52);
                 tmp = tmp2 < tmp1;
                 push(stack,cur_proc,sp,tmp, 41);
//    printf("%04d:  LT_OP %d %d %d\n", i,tmp,tmp1,tmp2); 
                 break;
      case GT_OP: tmp1 = pop(stack,cur_proc,sp, 54);
                  tmp2 = pop(stack,cur_proc,sp, 56);
                  tmp = tmp2 > tmp1;
                  push(stack,cur_proc,sp,tmp, 43);
//    printf("%04d:  GT_OP %d SP %d %d\n", i,tmp, sp[cur_proc],GT_OP); 
                  break;
      case EQ_OP: tmp1 = pop(stack,cur_proc,sp, 58);
//printf("step 2 %d\n",sp[cur_proc]);
                  tmp2 = pop(stack,cur_proc,sp, 60);
//printf("EQ? %d %d\n", tmp1, tmp2);
                  tmp = tmp1 == tmp2;
                  push(stack,cur_proc,sp,tmp, 45);
//     printf("%04d:  EQ_OP %d\n", i, tmp); 
                  break;
      case NE_OP: tmp1 = pop(stack,cur_proc,sp, 62);
                  tmp2 = pop(stack,cur_proc,sp, 64);
                  tmp = tmp1 != tmp2;
                  push(stack,cur_proc,sp,tmp, 47);
//     printf("%04d:  NE_OP\n", i); 
                  break; 
      case STOP : printf("STOP called by proccess %d, hit any key to continue\n", cur_proc);
                  scanf("%d",&tmp2);
                  break;
      case STOR: tmp = pop(stack,cur_proc,sp, 68);
                 tmp1 = mem[cur_proc][i+1];
                 if(tmp1<230)
                 {
                     gmem[tmp1]=tmp;
                     printf("Process %d wrote to global mem in index %d, %d\n",cur_proc,tmp1,gmem[tmp1]);
//printf("returning p0WRITE\n"); keyhit(99);
                  next_inst[cur_proc]++;
                     return p0WRITE;
                 } 
                 else
                 {    reg[cur_proc][tmp1-230]=tmp;
                 
                  next_inst[cur_proc]++;
                 }
                 break;
      case ST : tmp = pop(stack,cur_proc,sp, 70);//does ST ever need to store in a register?
//printf("%d\n",tmp);
                tmp1 = pop(stack,cur_proc,sp, 72);
//printf("%d\n",tmp1);
                gmem[tmp]=tmp1;
               printf("process %d wrote to global mem in index %d, %d\n",cur_proc,tmp,gmem[tmp]);
                return p0WRITE;
      case LOCK : printf("LOCK called by process %d\n",cur_proc); 
                  return LOCKED;
      case UNLOCK : printf("UNLOCK called\n");
                    return UNLOCKED;
      case HALT : printf("HALT called by process %d\n\n",cur_proc);
                  print_gmem();
                  print_register(reg);

                  exit(0); 
      case JFALSE : tmp=pop(stack, cur_proc,sp, 74);
                    tmp2=mem[cur_proc][i+1];
//printf("jfalse %d %d \n", tmp,tmp2-1);
                    if(tmp==0)
                        next_instruct[cur_proc]=tmp2-1;//sub one for PC in executeit()
                    else next_inst[cur_proc]++;
                    break;
      case JMP: tmp=mem[cur_proc][i+1];
                next_instruct[cur_proc]=tmp-1;//sub one for PC in executeit() 
 //    printf("%04d:  JMP\t %d\n", i, next_instruct[cur_proc]); 
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
/* show stack 
   { int i, j;

     printf("pid=%d: POP Called from  %d\n", proc_id,  calledfrom);
     printf("tos = %d\n", sp[proc_id]);
     for(i=0; i<=sp[proc_id]; i++)
      printf("(%d) %d\n", i, stack[proc_id][i ]);
     printf("returning %d\n", val);
keyhit(10);
   }
*/
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

/* show stack 
   { int i, j;
     printf("pid=%d: PUSH Called from  %d\n", proc_id,  calledfrom);
     printf("tos = %d\n", sp[proc_id]);
     for(i=0; i<=sp[proc_id]; i++)
      printf("(%d) %d\n", i, stack[proc_id][i]);
keyhit(11);
   }
*/
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
      printf("%d  ", gmem[i]);
   printf("\n");
}

void print_register(int reg[][REGISTERSIZE])
{
   int i,j;
   printf("Register data\n");
   for(i=0;i<pid;i++)
   {
      printf("Process %d: ",i);
      for(j=0;j<REGISTERSIZE;j++)
         printf("%d  ",reg[i][j]);
      printf("\n");

   }
}




