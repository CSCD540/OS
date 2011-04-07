%{
#include <math.h>
#include "easm.tab.h"
typedef struct
{ char idop[11];
  int type;
} PARSE;

extern PARSE parse[400];
extern sindex;

unsigned int linenumber = 0;
void count();
%}

DIGIT    [0-9]
IDENT    [a-zA-Z][a-zA-Z0-9]*

%%

{DIGIT}+    { count();
            //printf( "An integer: %s \n", yytext  );
              yylval.sptr = yytext;
            return NUMBER;
            }

IDATA        { count();
            //printf( ": %s\n", yytext );
            return IDATA;
            }
DATA        { count();
            //printf( ": %s\n", yytext );
            return DATA;
            }

LOAD        { count();
            //printf( ": %s\n", yytext );
            return LOAD;
            }

LOADI       { count();
            //printf( ": %s\n", yytext );
            return LOADI;
            }

LD          { count();
            //printf( ": %s\n", yytext );
            return LD;
            }

LA          { count();
            //printf( ": %s\n", yytext );
            return LA;
            }

PROC        { count();
            //printf( ": %s\n", yytext );
            return PROCESS;
            }

HALT        { count();
            //printf( ": %s\n", yytext );
            return HALT;
            }

END         { count();
            //printf( ": %s\n", yytext );
            return END;
            }

ENDP        { count();
            //printf( ": %s\n", yytext );
            return ENDP;
            }

J           { count();
            ////printf( ": %s\n", yytext );
            return JMP;
            }

JFALSE      { count();
            //printf( ": %s\n", yytext );
            return JFALSE;
            }

LOCK        { count();
            //printf( ": %s\n", yytext );
            return LOCK;
            }

UNLOCK      { count();
            //printf( ": %s\n", yytext );
            return UNLOCK;
            }

OPEN        { count();
              return OPEN;
            }
READ        { count();
              return READ;
            }
WRITE       { count();
              return WRITE;
            }
CLOSE       { count();
              return CLOSE;
            }
SEEK       { count();
              return SEEK;
            }
POPD        { count();
              return POPD;
            }
POP         { count();
              return POP;
            }

STOP        { count();
            //printf( ": %s\n", yytext );
            return STOP;
            }


LE         { count();  yylval.sptr = yytext; 
              sprintf( parse[sindex].idop, "%s", "<=");
              parse[sindex].type = LE_OP;
              sindex++;
              return(LE_OP); 
           }
GE         { count(); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ">=");
              parse[sindex].type = GE_OP;
              sindex++;
              return(GE_OP); 
           }
LT         { count(); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "<");
              parse[sindex].type = LT_OP;
              sindex++; 
              return(LT_OP); 
           }
GT         { count(); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ">");
              parse[sindex].type = GT_OP;
              sindex++; 
              return(GT_OP); 
           }
EQ         { count(); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "==");
              parse[sindex].type = EQ_OP;
              sindex++; 
              return(EQ_OP);
           }
NE         { count(); yylval.sptr = yytext; 
              sprintf( parse[sindex].idop, "%s", "!=");
              parse[sindex].type = NE_OP;
              sindex++;
              return(NE_OP);
           }
AND        { count();
            //printf("AND: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "AND");
              parse[sindex].type = AND;
              sindex++;
            return AND;
           }

OR         { count();
            //printf("OR: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "OR");
              parse[sindex].type = OR;
              sindex++;
            return OR;
           }

NOT        {  count();//printf("NOT: %s\n", yytext); 
              yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "NOT");
            
               parse[sindex].type = NOT;
              sindex++;
            return NOT;
           }

ST         {  count();
            // printf("ST: %s\n", yytext);
              yylval.sptr = yytext;
              //sprintf( parse[sindex].idop, "%s", "ST");
              //parse[sindex].type = ST;
              //sindex++;
            return ST;
           }

STOR       {  count();
              // printf("ST: %s\n", yytext);
              yylval.sptr = yytext;
              //sprintf( parse[sindex].idop, "%s", "STOR");
              //parse[sindex].type = STOR;
              //sindex++;
            return STOR;
           }

ADD        {  count();
              // printf("PLUS: %s\n", yytext);  
              yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "+");
              parse[sindex].type = ADD;
              sindex++;
            return ADD;
           }

SUB        {  count();
              //printf("MINUS: %s\n", yytext);
               yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "-");
              parse[sindex].type = SUB;
              sindex++;
            return SUB;
           }

MUL        {  count();
              //printf("MUL: %s\n", yytext); 
              yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "*");
              parse[sindex].type = MUL;
              sindex++;
            return MUL;
           }

DIV        {  count();
              //printf("DIV: %s\n", yytext); 
              yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "/");
              parse[sindex].type = DIV;
              sindex++;
            return DIV;
           }


"."        {  count(); 
             //  printf( ": %s\n", yytext ); 
              yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ";");
              parse[sindex].type = DOT;
              sindex++;
            return DOT;
           }

","        {  count(); 
             //  printf( ": %s\n", yytext );
             // yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ",");
              parse[sindex].type = COMMA;
              sindex++;
            return COMMA;
           }

":"        {  count();
              // printf( ": %s\n", yytext ); 
              yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ":");
              parse[sindex].type = COLON;
              sindex++;
            return COLON;
           }


{IDENT}     { count();
             // printf( "ID: %s\n", yytext );
              yylval.sptr = yytext;
            return ID;
            }


"//"[^\n]*  { count();   /* eat until eol */ }     
         

[ \t\n]+    { count();   /* eat up whitespace */   }

.           printf( "Unrecognized character: %s\n", yytext );

%%


int column = 0;

void count()
{
        int i;
        for (i = 0; yytext[i] != '\0'; i++)
                if (yytext[i] == '\n')
                        linenumber++;
/**
        for (i = 0; yytext[i] != '\0'; i++)
                if (yytext[i] == '\n')
                        column = 0;
                else if (yytext[i] == '\t')
                        column += 8 - (column % 8);
                else
                        column++;

        ECHO;
**/
}
