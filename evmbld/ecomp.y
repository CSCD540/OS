%{
#include <stdio.h>
#include <string.h>
#include "ecomp.tab.h"

#if 1
#define keyhit(d) {int x; printf("hit enter to continue  (%d)",d); getchar();}
#else
#define keyhit ; 
#endif

#define pause {int x; printf("enter 0 to continue "); scanf("%d", &x); }

char *yytext;
FILE *yyin;
FILE *outfile;
int yy_flex_debug;
//int nest;

int istack[10];
int istackptr=0;
int ijump=0;
int wstack[10];
int wstackptr=0;
int wjump=0;
int estack[10];
int estackptr=0;
int ejump=0;

int thisIsNot =0;


typedef struct 
{ char idop[11];
  int type;
} PARSE;

PARSE parse[500];
PARSE parsecopy[20];
int copysindex;
int ifstck[10];

#define GMAX 30
#define SMAX 20
#define INITMAX 100

#define GLOBALVAR 0
#define SUBVAR    1
#define REGISTER   2 
int checkvartype;

char glovars[GMAX][11];
//char locvars[GMAX][11];
char initialized[INITMAX];
int  initializedindex = 0; 
char  countinitialized = 0; 
char subvars[SMAX][11];
unsigned int nglob;

int yylex(void);
void yyerror(char *);

char storage[11];
char *gid[11];
char assID[11];
char prevID[11];
char optype[3];
int assIDon = 1;
int i,j,k;

int  sindex = 0;
int  pindex = 0;
int  memloc = 0;
int psub =1;
int loadcount =0;
int smemloc = 0;

int symloc=0;

int showparse( int beg)
{ int i;
        for (i=beg; i<sindex; i++)
        {  printf("############## parse[%d].idop= %s, (type)%d\n",
                                i, parse[i].idop, parse[i].type);
        }
        printf("end show\n");
}

int termcopy(int beg)
{ int i, j;
         printf("termcopy: storage loc calc here\n");
         //fprintf(outfile,"storage loc calc here\n");
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

int printinitialized()
{ int i;

   fprintf(outfile, ".IDATA\n");
   for(i=0; initialized[i]; i++)
   { if(initialized[i] == -1)
     { fprintf(outfile, "\n");
       //printf("\n");
     }
     else
     { fprintf(outfile, "%c", initialized[i]);
       // printf("%c", initialized[i]);
     }
   } 
}

int showterm( int beg)
{ int i;
//fprintf(outfile, "sindex=%d\n", sindex);
        for (i=beg; i<sindex; i++)
        {  fprintf(outfile, "  %s",  parse[i].idop);
           printf("    %s",  parse[i].idop);
           if( ((i+1)<sindex) && (parse[i+1].idop[0] == '['))
           { i= i+1;
             do { /* append parse[i].idop to tgt */
               fprintf(outfile, "%s",  parse[i].idop);
               printf("%s",  parse[i].idop);
             } while (parse[i++].idop[0] != ']');
             i--;
           }
        }
        fprintf(outfile, "\n");
}

int declr_alloc(int loc)
{ int i;
printf("declr_alloc called from %d\n", loc);
//showparse(0);
//showterm(0);
//keyhit(445);
//printf("END of showparse showterm\n");

  if( parse[0].type != ID )
  {  printf("declare Identifier err: %s\n", parse[0].idop ); 
  
  } 
  printf("declr_alloc():   loc:%d\n",  memloc);

  if( parse[1].type != (char)LBRCT )
  {   
      return;
  }
  else
  {    printf("declr_alloc():  %s", parse[2].idop);
      return;
  }
}

%}

%union
{int intval;
 char *sptr;
}

%token SUBRTN
%token NUMBER
%token ID
%token IF
%token ELSE
%token WHILE
%token EQ
%token PLUS
%token MINUS 
%token MUL 
%token DIV 
%token END
%token PROCESS
%token RP
%token LP
%token RB
%token LB
%token RBRCT
%token LBRCT
%token AND
%token OR
%token NOT
%token LE_OP
%token GE_OP
%token LT_OP
%token GT_OP
%token EQ_OP
%token NE_OP
%token HALT 
%token OPEN 
%token READ 
%token WRITE 
%token CLOSE 
%token SEEK 
%token SEMICOL 
%token COMMA 
%token LOCK
%token UNLOCK
%token DBLQT
%token CHARSTR

%token ARRAY 

%left  LPREC 
%right RPREC 

%%

program: programx
         {
          printinitialized();
          printf("DONE\n");
         }
         ;
programx:   program0 
         { printf("END PARSING 1\n"); 
         }
         | varlistend  program0 
         { fprintf(outfile, "\n"); printf("END PARSING 2\n");
         }
         | programx  program0  
         { fprintf(outfile, "\n"); printf("END PARSING 3\n");
         }
        ;


varlistend:
           varlistend varlists
         | varlists
        ;

varlists:
          varlist SEMICOL
              { int i,j;
                j=0;
                fprintf(outfile,".DATA\n");
                for (i=0; i<sindex; i++)
                {
                   if( parse[i].type == ID)
                   { strcpy(glovars[j++], parse[i].idop);
                     fprintf(outfile,"%s", parse[i].idop);
                     //checkvar( parse[i].idop, 0 );
                   }
                   else if( parse[i].type == COMMA || parse[i].type == SEMICOL)
                   { continue;
                   }
                   if(parse[i+1].idop[0] == '[')
                   { fprintf(outfile, ",");
                     fprintf(outfile, "%s",  parse[i+2].idop);
                     if(parse[i+3].idop[0] != ']')
                         fprintf(outfile,"err var name array?\n");
                     i=i+3;
                   }
                   fprintf(outfile,"\n");
                }
                nglob=j;
                fprintf(outfile,"\n");
                //for(i=0; i<nglob; i++) fprintf(outfile, "(%s)\n", glovars[i]);
                //showterm(0);
                //fprintf(outfile, "varlistend\n");
                sindex=0;
              }
        ;

varlist:   declr 
              { //fprintf(outfile, "global ID=%s $$ ALLOCATE\n", yylval.sptr); 
              }
         | varlist COMMA declr 
              { //fprintf(outfile, "global IDs=%s $$ ALLOCATE\n", yylval.sptr ); 
              }
        ;

declr:
          term { declr_alloc(4);
               }
        | arrayterm 
               { declr_alloc(5);
               }
        ;

arrayterm:
           term LBRCT expr RBRCT 
               { printf("term [exp]\n");
                 //showterm(0);
                printf("TERM [ EXPR ]  code?\n");
               }
        ;

program0:
          process0 statements RB
               { fprintf(outfile, "END\n");  sindex=0;}
        | process0  RB 
               { fprintf(outfile, "END\n");  sindex=0;}
        | process1 LB statements RB 
               { fprintf(outfile, "END\n");  sindex=0; }
        | process1 LB  RB 
               { fprintf(outfile, "END\n");  sindex=0; }
        | SUBRTN subroutines program0 
               { sindex=0;  }
        ;

subroutines: subnidarglist subbody
               { fprintf(outfile, "RET\n\n");  sindex=0;}
        ;

subbody :  LB statements RB
         | LB RB
        ;

subnidarglist: subname LP arglist RP 
          {
             smemloc=0;
             /*printf("arglist\n");
             for (i=0; i<sindex; i++)
             { if(parse[i].type == ID )
               { strcpy(subvars[smemloc++], parse[i].idop);
                 printf("arg sym %s\n", parse[i].idop);
                   { int i;
                     for(i=0; i<smemloc; i++)
                     { printf("subnidarglist: %s\n", subvars[i]);
                     }
                   }
               }
             }
             //sindex = 0;
             */
          }
        ;

subname:  ID
          {
               printf("subroutine declared %s\n", yylval.sptr);
               fprintf(outfile,"%s:\n", yylval.sptr);
          }

process0:
          process1 LB SEMICOL 
              { // process1 LB  varlist SEMICOL 
                // activate the above if local variables are wanted

                showparse(0);
                if( parse[0].type != ID )
                { printf("declare Process Identifier err\n"); 
                }  
                fprintf(outfile, ".TEXT-%d\n", pindex);
                pindex++;
                showterm(0);
 
               //sindex = 0;
                psub = 0;
                printf("process0 term  code?? psub=%d\n",psub);
              }
        ;

process1:
           PROCESS term 
              {
                fprintf(outfile, "PROC ");
                showterm(0);
                sindex = 0;
                psub = 0;
                printf("process1  code?? psub=%d\n",psub);
                //fprintf(outfile, "process ID end sindex=%d\n", sindex);
              }

        ;

statements:
          statement 
              { printf("statement  code??\n");
              }
        | statements statement 
              { printf("statements statement code??\n");
              }
        ;

statement:
          ifstatement  
              { printf("---statement:= ifstatement end\n"); 
                istackptr--;
                fprintf(outfile, "I%d:\n", istack[istackptr]);
              }
        | ifstatement ELSE elsestatement  
              { printf("---statement:= ifstatement elsestatement end\n"); 
                estackptr--;
                fprintf(outfile, "E%d:\n", estack[estackptr]);
              }
        | whilestatement  
              { printf("---statement:= ifstatement\n"); 
              }
        | assignment 
              { printf("===statement:= assignment\n");
              }
        | lockstatement 
              { printf("===lock statement:\n");
              }
        | haltstatement 
              { printf("===halt statement:\n");
              }
        | seekstatement 
              { printf("===open statement:\n");
              }
        | openstatement 
              { printf("===open statement:\n");
              }
        | readstatement 
              { printf("===read statement:\n");
              }
        | writestatement 
              { printf("===write statement:\n");
              }
        | closestatement 
              { printf("===close statement:\n");
              }
        | subroutine
              { printf("=== subroutine statement:\n"); }
        | locdeclare
              { printf("=== locdeclare:\n"); }
        ;

openstatement: openfirst opensecond

openfirst:    declrEQ OPEN LP arglist   
             // declrEQ OPEN LP term DBLQT
              { int i;
                char label[10];

                showparse(0);
                fprintf(outfile,"LA L%d\n", countinitialized);
                fprintf(stderr,"LA L%d\n", countinitialized);
                sprintf( label, "%d", countinitialized);
                initialized[initializedindex++]='L';
                for(i=0; label[i]; i++)
                initialized[initializedindex++]=label[i];
                initialized[initializedindex++]=':';
                
                for(i=0; parse[2].idop[i]; i++)
                initialized[initializedindex++]=parse[2].idop[i];
                initialized[initializedindex++]=-1;

                countinitialized++;

                if(parse[5].type == NUMBER)
                { fprintf(outfile, "LOADI  %s\n",  parse[5].idop);
                  fprintf(stderr, "LOADI %s\n",  parse[5].idop);
                }
                else
                { printf("open(): syntax error\n");
                  keyhit(9);
                }

                sindex=0;

                fprintf(outfile, "OPEN\n");
                fprintf(stderr, "OPEN\n");

                {  checkvar( parsecopy[0].idop , 47);
                   fprintf(outfile, "POPD %s\n", parsecopy[0].idop);
                   fprintf(outfile, "POP\n");
                   fprintf(outfile, "POP\n");
                }
              }
         ;
opensecond:    RP SEMICOL
              { sindex=0; 
              }
        ;

seekstatement:
          SEEK LP arglist RP SEMICOL
              { int i, lcount;
                showparse(0);
printf("seek()\n");
                for (i=lcount=0; i<sindex; i++)
                { if(parse[i].type == ID)
                   { fprintf(outfile, "LOAD %s\n",  parse[i].idop);
                     fprintf(stderr, "LOAD %s\n",  parse[i].idop);
                     checkvar( parse[i].idop,  10 );
                     lcount++;
                   }
                   else if(parse[i].type == NUMBER)
                   { fprintf(outfile, "LOADI %s\n",  parse[i].idop);
                     fprintf(stderr, "LOADI %s\n",  parse[i].idop);
                     lcount++;
                   }

                }

                fprintf(outfile, "SEEK\n");
                for(i=0; i<lcount; i++) 
                   fprintf(outfile, "POP\n");
                sindex=0;
              }
        ;

readstatement:
          READ LP arglist RP SEMICOL
              { int i, lcount;
                showparse(0);

                checkvar( parse[1].idop,  10 );
                fprintf(outfile, "LOAD  %s\n",  parse[1].idop);
                fprintf(outfile, "READ\n");
                checkvar( parse[3].idop,  10 );
                fprintf(outfile, "POPD %s\n",  parse[3].idop);
                fprintf(outfile, "POP\n");
                sindex=0;
//keyhit(99);
              }
        ;

writestatement:
          WRITE LP arglist RP SEMICOL
              { int i, lcount;
                showparse(0);
                for (i=lcount=0; i<sindex; i++)
                { if(parse[i].type == ID)
                   { fprintf(outfile, "LOAD %s\n",  parse[i].idop);
                     fprintf(stderr, "LOAD %s\n",  parse[i].idop);
                     checkvar( parse[i].idop,  10 );
                     lcount++;
                   }
                }
                fprintf(outfile, "WRITE\n");
                for(i=0; i<lcount; i++)
                     fprintf(outfile, "POP\n");
                sindex=0;
              }
        ;

closestatement:
          CLOSE LP arglist RP SEMICOL
              {int i,lcount;
                 showparse(0); 
                for (i=lcount=0; i<sindex; i++)
                { if(parse[i].type == ID)
                   { fprintf(outfile, "LOAD %s\n",  parse[i].idop);
                     fprintf(stderr, "LOAD %s\n",  parse[i].idop);
                     checkvar( parse[i].idop,  10 );
                     lcount++;
                   }
                }
                sindex=0;
                fprintf(outfile, "CLOSE\n");
                for(i=0; i<lcount; i++)
                     fprintf(outfile, "POP\n");
              }
        ;

/* openparam: arglist 
        ; 
*/

locdeclare : arglist SEMICOL 
             {int i;
               showparse(0);

             for (i=0; i<sindex; i++)
             { if(parse[i].type == ID )
               { strcpy(subvars[smemloc++], parse[i].idop);
                 printf("arg sym %s\n", parse[i].idop);
                   { int i;
                     for(i=0; i<smemloc; i++)
                     { printf("locdelare: %s\n", subvars[i]);
                     }
                   }
               }
             }
              sindex=0;
             
             }
        ;

whilestatement:
          whilepart LB rest 
              { printf("whilestatement\n"); 
                wstackptr--;
                fprintf(outfile, "J H%d\n", wstack[wstackptr]);
                //fprintf(outfile, "whilestatement JUMP here on while() false\n"); 
                fprintf(outfile, "W%d:\n", wstack[wstackptr]);
              }
        ;

subroutine:
          subrtnid subrest
              {int i;
                printf("SUBROUTINE sindex=%d\n", sindex);
     showparse(0);
                if( sindex==0 ) i= 0;
                else i=sindex-1;
                loadcount=0;
printf(" i=%d\n", i);
                  printf("SETFP\n"); 
                  fprintf(outfile, "SETFP\n"); 
                while (i>0)
                { 
                  if(parse[i].type == ID || parse[i].type==NUMBER )
                  {  printf("LOAD %s\n",  parse[i].idop); 
                     fprintf(outfile, "LOAD %s\n",  parse[i].idop); 
                     loadcount++;
                  } 
                  i--;
                }

                 printf("CALL %s\n",  parse[i].idop); 
                 fprintf(outfile, "CALL %s\n",  parse[i].idop); 
                 for(loadcount; loadcount; loadcount--)
                  { printf("POP\n"); 
                    fprintf(outfile, "POP\n"); 
                  }
                sindex = 0; loadcount=0;
printf("------> sindex=%d\n", sindex);
                printf("SUBROUTINE END HERE sindex=%d\n", sindex);
              }
        ;

subrtnid:
          term LP 
          {

              printf( " SUBRTNID: psub=%d\n", psub);
              showparse(0);

              if (psub == 1)
              { printf("%s:\n",  parse[i].idop); 
                fprintf(outfile, "%s:\n",  parse[i].idop); 
              }
          }
        ;

subrest:
          arglist RP SEMICOL 
            {
              printf( " SUBrest:\n");
              showparse(0);
              // sindex=0; /*ko2*/
printf("subrest------> sindex=%d\n", sindex);
              printf( " SUBrest end sindex=%d:\n", sindex);
            }
        ;

arglist: term 
         {
          printf("%s", yylval.sptr);
         }
        |arglist COMMA term 
         {
          printf("%s", yylval.sptr);
         }
        ;

whilepart:
          whilehead LP cond RP  
              { //fprintf(outfile, "while cond := WHILE LP LB RB\n"); 
                printf("GEN_CODE while head\n");
                showparse(0);
                fprintf(outfile, "H%d:\n", wjump);
                logic(0, sindex);
                sindex = 0;

                fprintf(outfile, "JFALSE W%d\n", wjump);
                printf("JFALSE W%d\n", wjump);
                wstack[wstackptr]=wjump;
                wstackptr++;
                wjump++;
              }
        ;

whilehead:
          WHILE   
              {
                //fprintf(outfile, "whilehead label:\n");
              }
        ; 

ifstatement: 
          ifpart rest 
              { //fprintf(outfile, "ifstatement done label ##   if false JUMP here\n");
                printf("ifstatement done label ##   if false JUMP here\n");
              }
        ;

ifpart:
          IF LP cond RP LB 
              { //fprintf(outfile, "if cond := IF LP LB RB\n"); 
                printf("if cond := IF LP LB RB\n"); 
//showparse(0);
//keyhit(111);
               //nest=0;
                logic(0, sindex);
                //fprintf(outfile, "IF LP cond RP  code??\n");
                printf("if cond := JUMPonFalse ##\n"); 
                sindex = 0;

                fprintf(outfile, "JFALSE I%d\n", ijump);
                printf("JFALSE I%d\n", ijump);
                istack[istackptr]=ijump;
                istackptr++;
                ijump++;
              }
        ;

rest:     
          statements RB 
             {  //fprintf(outfile, "   end of rest RB:  if false JUMP here\n");
             }
        ;

elsestatement: 
           elsepart statements RB
             { //fprintf(outfile, "if clause end JUMP here to skip else clause\n");
             }
        ;



elsepart:  LB
             { //fprintf(outfile, " true clause, JUMP over else clause\n");
                fprintf(outfile, "J E%d\n", ejump);
                printf("J E%d\n", ejump);
                estack[estackptr]=ejump;
                estackptr++;
                ejump++;

                istackptr--;
                fprintf(outfile, "I%d:\n", istack[istackptr]);
             }
        ;

cond:
         andcond      
              { printf("cond:= andcond ---------------\n"); 
  //showparse(0);
  //keyhit(444);
                printf("andcond  code? ---------------\n");
              }
        | LP cond RP OR LP cond RP
              { printf("cond:= cond OR andcond==============\n"); 
              //sprintf( parse[sindex].idop, "%s", "OR");
              //parse[sindex].type = OR;
              //sindex++;
              //  showparse(0);
                printf("cond OR andcond code? =============\n");
              }
        ;
        | LP cond RP AND LP cond RP
              { printf("cond:= cond OR andcond==============\n"); 
              //sprintf( parse[sindex].idop, "%s", "OR");
              //parse[sindex].type = OR;
              //sindex++;
               // showparse(0);
                printf("cond OR andcond code? =============\n");
              }
        ;

andcond:
          expr           
              { printf("andcond:= expr   expr process\n");
                //showparse(0);
                printf("expr  code??\n");
              }
        | expr AND expr 
              { printf("andcond:= expr AND expr\n");
                // showparse(0);
                printf("expr AND expr code??\n");
              }
        ;
        | expr OR expr 
              { printf("andcond:= expr AND expr\n");
                // showparse(0);
                printf("expr AND expr code??\n");
              }
        | NOT LP cond RP
              {
                showparse(0);
                thisIsNot = 1;
              }
        ;

lockstatement:
           lockhead statements UNLOCK SEMICOL
             { fprintf(outfile, "UNLOCK\n");
               sindex = 0;
             }
        ;

lockhead:
           LOCK SEMICOL
             { fprintf(outfile, "LOCK\n");
               sindex = 0;
             }
        ;

haltstatement:
           HALT SEMICOL 
             { fprintf(outfile, "HALT\n");
               sindex = 0;
             }
        ;


assignment:
          declrEQ assexpr SEMICOL 
              { printf("assignment:= var =\n");
                printf("declr EQ assexpr  code??\n");
printf("assignment------> sindex=%d\n", sindex);
//showparse(0);
//keyhit(0);

                if(parsecopy[1].idop[0] == '[' && copysindex>1 )
                { checkvar( parsecopy[0].idop,  1);
                  fprintf(outfile, "LA   %s\n", parsecopy[0].idop);
                  fprintf(stderr, "LA   %s\n", parsecopy[0].idop);
                  if(parse[2].type == ID)
                  { 
                   if(psub)
                    fprintf(outfile,"LDFP %d\n",symloc);
                   else
                    fprintf(outfile, "LOAD %s\n",  parsecopy[2].idop);

                   fprintf(stderr, "LOAD %s\n",  parsecopy[2].idop);
                   printf("LOAD %s\n",  parsecopy[2].idop);
                   checkvar( parsecopy[2].idop, 3 );
                  }
                  else if(parse[2].type == NUMBER)
                  { fprintf(outfile, "LOADI %s\n",  parsecopy[2].idop);
                    fprintf(stderr, "LOADI %s\n",  parsecopy[2].idop);
                  }
                  if(parse[3].idop[0] != ']') 
                  { fprintf(outfile, "ERR; index in assignment\n");
                    fprintf(stderr, "ERR; index in assignment\n");
                  }
                  fprintf(outfile, "ADD\n");
                  fprintf(outfile, "ST\n");
                  //i=i+3;
                }
                else
                {  
                    checkvar( parsecopy[0].idop , 45);
       // printf("checkvartype=%d symloc=%d\n", checkvartype, symloc);
       //              keyhit(776);
                   if(checkvartype==REGISTER) 
                   {  fprintf(outfile, "STOR %s\n", parsecopy[0].idop);
                   }
                   else if(checkvartype==SUBVAR) 
                   {  fprintf(outfile, "STFP %d\n", symloc);
                   }
                   else
                   {  fprintf(outfile, "STOR %s\n", parsecopy[0].idop);
                   }
                }
printf("assignment END------> sindex=%d\n", sindex);
 
              }
        ;

declrEQ:
          declr EQ  
              { printf("declrEQ:= declr EQ\n");
printf("declrEQ------> sindex=%d\n", sindex);
                //printf("GEN_CODE  calc store location\n");
                //fprintf(outfile, "GEN_CODE store rval to location\n");
                termcopy(0);
                showparse(0);
                sindex=0;
printf("declrEQ------> sindex=%d\n", sindex);
              }
        ;

assexpr: expr
              { printf("assexpr:= expr POSTFIX here!!!\n");
                showparse(0);
                gen_expr(0);
                sindex=0;
printf("assexpr------> sindex=%d\n", sindex);
              }
        ;

expr:    expr0
              { printf(" expr:= expr0\n");
              }
        ;
expr0:   
          mulexpr
              { printf("expr:= mulexpr\n");
                // showparse(0);
                printf("mulexpr  code??\n");
              }
        | expr0 PLUS  mulexpr
              { printf("expr:= expr PLUS mulexpr\n");
                // showparse(0);
                printf("expr PLUS mulexpr  code??\n");
              }
        | expr0 MINUS mulexpr
              { printf("expr:= expr MINUS mulexpr\n"); 
                // showparse(0);
                printf("expr MINUS mulexpr  code??\n");
              }
        | expr0 GE_OP mulexpr 
              { printf("expr:= expr GE_OP mulexpr\n"); 
                // showparse(0);
                printf("expr GE_OP mulexpr  code??\n");
              }
        | expr0 GT_OP mulexpr
              { printf("expr:= expr GT_OP mulexpr\n"); 
                // showparse(0);
                printf("expr GT_OP mulexpr  code??\n");
              }
        | expr0 LE_OP mulexpr
              { printf("expr:= expr LE_OP mulexpr\n"); 
                // showparse(0);
                printf("expr LE_OP mulexpr  code??\n");
              }
        | expr0 LT_OP mulexpr 
              { //printf("expr:= expr LT_OP mulexpr\n"); 
                // showparse(0);
                printf("expr LT_OP mulexpr  code??\n");
              }
        | expr0 EQ_OP mulexpr 
              { printf("expr:= expr EQ_OP mulexpr\n"); 
                // showparse(0);
                printf("MINUS mulexpr  code??\n");
              }
        | expr0 NE_OP mulexpr 
              { printf("expr:= expr NE_OP mulexpr\n"); 
                // showparse(0);
                printf("MINUS mulexpr  code??\n");
              }
        ;

mulexpr:  declr 
              { printf("mulexpr:= term\n");
              }
        | MINUS declr
              { //printf("mulexpr:= MINUS term\n"); 
                // showparse(0);
              }
        | mulexpr DIV declr 
              { //printf("mulexpr:= mulexpr DIV term\n"); 
                // showparse(0);
              }
        | mulexpr MUL declr 
              { //printf("mulexpr:= mulexpr MUL term\n"); 
                // showparse(0);
              }
        ;   

term:    dblqtid DBLQT 
            { //fprintf(outfile, "term:= ID   %s  %d\n", yylval.sptr, sindex);  
printf("termDBLQT------> %s sindex=%d\n", yylval.sptr, sindex);
             // sprintf( parse[sindex].idop, "%s", yylval.sptr);
             // parse[sindex].type = ID;
             // sindex++;
            }
        ;
        |
         ID
            { //fprintf(outfile, "term:= ID   %s  %d\n", yylval.sptr, sindex);  
printf("termID------> %s sindex=%d\n", yylval.sptr, sindex);
              sprintf( parse[sindex].idop, "%s", yylval.sptr);
              parse[sindex].type = ID;
              sindex++;
            }
        | NUMBER 
            { //fprintf(outfile, "term:= NUMBER   %s  %d\n", yylval.sptr, sindex);  
printf("termNUMBER------> sindex=%d\n", sindex);
              sprintf( parse[sindex].idop, "%s", yylval.sptr);
              parse[sindex].type = NUMBER;
              sindex++;
            }
        ;

dblqtid: DBLQT ID
            {
printf("dblqt------> %s sindex=%d\n", yylval.sptr, sindex);
              sprintf( parse[sindex].idop, "%s", yylval.sptr);
              parse[sindex].type = ID;
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

main(int argc, char **argv )
{
    ++argv, --argc;  /* skip over program name */
    if ( argc == 2 )
    {        yyin = fopen( argv[0], "r" );
             outfile = fopen( argv[1], "w");
    }
    else
            yyin = stdin;


    yyparse();

    fclose(outfile);
    return 0;

}

int checkvar(char *var, int loc)
{ int i,j;

 printf("var=%s   called from %d\n", var,  loc);

  if( psub == 1)
  {
   for(i=0; i<smemloc; i++)
   {
    if ( *var == 'r' && (  *(var+1) >= '0' &&  *(var+1) <= '9')  )
    {
printf("register symloc= %d\n", symloc);
//keyhit(90);
     checkvartype=REGISTER;
     return (0);
    }
     j=strcmp(subvars[i], var);
     printf("%s [%d],   var=%s  comp_result=%d  called from %d\n", 
                subvars[i], i, var,  j, loc);
     if ( j == 0 )
       { symloc = i; 
         checkvartype=SUBVAR;
printf("SUBVAR var loc = %d\n", symloc);
//keyhit(91);
         return (0);
       }
   } /*** End for(i=0; i<smemloc; i++) ***/
  }

  for(i=0; i<nglob; i++)
  {
     j=strcmp(glovars[i], var);
    //fprintf(outfile, "%s %s  [%d] %d  loc=%d\n", glovars[i], var, i, j , loc);
     if ( j == 0 )
       { symloc = i; 
         checkvartype=GLOBALVAR;
         return (0);
       }
  }
  if ( *var == 'r' && (  *(var+1) >= '0' &&  *(var+1) <= '9')  )
  { checkvartype = REGISTER; 
   return (0);
  }

  fprintf(outfile, "var not declared :: %s\n", var);
  fprintf(stderr, "var not declared :: %s\n", var);
  return (1);
}

int gen_expr(int beg)
{int i, stckptr;
 int stck[20];

//for(i=0; i<sindex; i++)
//  fprintf(outfile, "assignment %s     %d\n", parsecopy[i].idop,parsecopy[i].type);


 showparse(0);

 i = beg;
 stckptr = 0;
   while( parse[i].type != SEMICOL )
   {
    if( parse[i].type == ID )
     {   
printf("-->parse[%d].idop = %s\n", i, parse[i].idop);
         checkvar( parse[i].idop , 7);
         if(parse[i+1].idop[0] == '[' && sindex > i)
         //if(parse[i+1].idop[0] == '[' )
         {
           fprintf(outfile, "LA   %s\n", parse[i].idop);
           fprintf(stderr, "LA   %s\n", parse[i].idop);
           if(parse[i+2].type == ID)
           { 
             fprintf(stderr, "LOAD %s\n",  parse[i+2].idop);
             printf("LOAD %s dbg776\n",  parse[i+2].idop);
              if(psub)
                fprintf(outfile, "LDFP %d\n", symloc);
              else
                { fprintf(outfile, "LOAD %s\n",  parse[i+2].idop);
                printf("LOAD %s   dbg777\n",  parse[i+2].idop);
         //keyhit(7777);
                }
             checkvar( parse[i+2].idop ,  8);
             i=i+2;
           }
           else if(parse[i+2].type == NUMBER)
           { fprintf(outfile, "LOADI %s\n",  parse[i+2].idop);
             fprintf(stderr, "LOADI %s\n",  parse[i+2].idop);
           i=i+2;
           }
           if(parse[i+1].idop[0] != ']') 
           { fprintf(outfile, "ERR; index\n");
             fprintf(stderr, "ERR; index\n");

printf("-->parse[%d].idop = %s\n", i, parse[i].idop);
showparse(0);
keyhit(81);

           i=i+3;
           }
           fprintf(outfile, "ADD\n");
           fprintf(outfile, "LD\n");
         }
         else
         {
 printf("register type\n");
           if( checkvartype==REGISTER )
            fprintf(outfile,"LOAD %s\n", parse[i].idop);
           else if(psub  && (checkvartype==SUBVAR))
            fprintf(outfile,"LDFP %d\n",symloc);
           else
            { fprintf(outfile,"LOAD %s\n",  parse[i].idop);
         //printf("LOAD 88 %s\n",  parse[i].idop);
            }
         }
     }
    else if( parse[i].type == NUMBER )
     {  fprintf(outfile, "LOADI %s\n", parse[i].idop);
     }
    else if( (parse[i].type == PLUS) || (parse[i].type == MINUS) )
     {
        if (stckptr == 0 && i != 0) 
          {  stck[stckptr++] = parse[i].type;
          }
        else if(( stck[stckptr-1] == MUL) || stck[stckptr-1] == DIV)
          { fprintf(outfile, "MUL DIV---\n");
            stck[stckptr-1] = parse[i].type;
          }
        else if (i == 0 )  /* unary + - */
          {  fprintf(outfile, "LOADI 0\n");
             stck[stckptr++] = parse[i].type;
          }
        else
          { //stck[--stckptr]=str[i];
            if( stck[stckptr-1] == PLUS )
               { fprintf(outfile, "ADD\n");
               }
            else if (stck[stckptr-1] == MINUS )
               { fprintf(outfile, "SUB\n");
               }
            stck[ stckptr-1] = parse[i].type;
          }
     }
    else   /*   it is '*' ||  '/'    */
     {    if( stckptr ==0  )
          {  stck[stckptr++] = parse[i].type;
          }
          else if( stck[stckptr-1] == PLUS || stck[stckptr-1] == MINUS )
          {  stck[stckptr++] = parse[i].type;
          }
          else
          { if( stck[stckptr-1] == MUL) fprintf(outfile, "MUL..\n");
            if( stck[stckptr-1] == DIV) fprintf(outfile, "DIV..\n");
            stck[stckptr-1] = parse[i].type;
          }
     }
     i++;

   }
  printf(" stckptr=%d\n", stckptr);
  for (i=0; i<stckptr; i++)
  { printf("%d\n", stck[i]);
  }
  printf("\n");
   while ( --stckptr >= 0 )
   { if (stck[stckptr] == PLUS)  fprintf(outfile, "ADD\n");
     else if (stck[stckptr] == MUL)   fprintf(outfile, "MUL\n");
     else if (stck[stckptr] == MINUS) fprintf(outfile, "SUB\n");
     else if (stck[stckptr] == DIV)   fprintf(outfile, "DIV\n");
   }
}

int showstck(int *stck, int end)
{ int i;
  printf("show stack ------------------------------------\n");
  for(i=0; i< end; i++)
  { printf("%d\n", *(stck+i));
  }
  printf("end stack----------------------------------------\n");
}


int logic(int beg, int end)
{int i, j, k, stckptr; int stck[20];

 i=stckptr = 0;
 for (i=beg; i<sindex; )
 {  printf("i=%d  %s, (type)%d\n", i, parse[i].idop, parse[i].type);
    if( (parse[i].type == AND) || (parse[i].type ==  OR) || 
     (parse[i].type ==  NOT) || (parse[i].type ==  LP)   ) 
    {  stck[stckptr++] = parse[i].type;
showstck(stck,stckptr);
//keyhit(909);
    }
    else if( parse[i].type == ID  )
    {  checkvar( parse[i].idop, 9 );
         if(parse[i+1].idop[0] == '[' && sindex > i)
         // if(parse[i+1].idop[0] == '[' )
         { fprintf(outfile, "LA   %s\n", parse[i].idop);
           fprintf(stderr, "LA   %s\n", parse[i].idop);
           if(parse[i+2].type == ID)
           { fprintf(outfile, "LOAD %s\n",  parse[i+2].idop);
             fprintf(stderr, "LOAD %s\n",  parse[i+2].idop);
             checkvar( parse[i+2].idop,  10 );
           }
           else if(parse[i+2].type == NUMBER)
           { fprintf(outfile, "LOADI %s\n",  parse[i+2].idop);
             fprintf(stderr, "LOADI %s\n",  parse[i+2].idop);
           }
           if(parse[i+3].idop[0] != ']') 
           { fprintf(outfile, "ERR; index in logic\n");
             fprintf(stderr, "ERR; index in logic\n");
           }
           fprintf(outfile, "ADD\n");
           fprintf(outfile, "LD\n");
           i=i+3;
         }
         else
         {
           fprintf(outfile, "LOAD %s\n", parse[i].idop);
         }
    }
    else if( parse[i].type == NUMBER )
    { fprintf(outfile, "LOADI %s\n", parse[i].idop);
    }
    else if( (parse[i].type == LE_OP) || (parse[i].type ==  GE_OP) || 
     (parse[i].type ==  LT_OP) || (parse[i].type ==  GT_OP) ||
     (parse[i].type ==  EQ_OP) || (parse[i].type ==  NE_OP)   ) 
    { stck[stckptr++] = parse[i].type;
showstck(stck,stckptr);
    }
    else if( parse[i].type ==  RP ) 
    {
printf("RP hit\n");
       do{
showstck(stck,stckptr);
          stckptr--;
printf("stck[%d]=%d\n", stckptr, stck[stckptr]);
//keyhit(98);

          if( stck[stckptr]== LP || stckptr==0 )
          { break;
          }
          else if(stck[stckptr] == LE_OP )
          { fprintf(outfile, "LE\n");
          printf("LE\n");
          }
          else if(stck[stckptr] ==  GE_OP)  
          { fprintf(outfile, "GE\n");
          printf("GE lable\n");
          }
          else if(stck[stckptr] ==  LT_OP) 
          { fprintf(outfile, "LT\n");
          printf("LT\n");
          }
          else if(stck[stckptr] ==  GT_OP) 
          { fprintf(outfile, "GT\n");
          printf("GT\n");
          }
          else if(stck[stckptr] ==  EQ_OP) 
          { fprintf(outfile, "EQ\n");
          printf("EQ\n");
          }
          else if(stck[stckptr] ==  NE_OP) 
          { fprintf(outfile, "NE\n");
          printf("NE\n");
//keyhit(91);
          }
          else if( stck[stckptr]== AND )
          { fprintf(outfile, "AND\n");
          printf("AND\n");
          }
          else if( stck[stckptr]== OR )
          { fprintf(outfile, "OR\n");
           printf("OR\n");
          }
          else if( stck[stckptr]== NOT )
          { fprintf(outfile, "NOT\n");
           printf("NOT\n");
              //  if (thisIsNot ==1 ) fprintf(outfile, "NOT\n");
            thisIsNot =0;
          }
          else
          { printf("error\n");
          }
             
       } while (1);
showstck(stck,stckptr);
//keyhit(92);
    }
  i++;
 }
}
