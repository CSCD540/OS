%{
#include <math.h>
#include "ecomp.tab.h"
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
IDENT    [a-z][a-z0-9]*

%%
             
{DIGIT}+    { count();
            printf( "An integer: %s \n", yytext  );
              yylval.sptr = yytext;
            return NUMBER;
            }

lock        { count();
            printf( ": %s\n", yytext );
            return LOCK;
            }
unlock      { count();
            printf( ": %s\n", yytext );
            return UNLOCK;
            }

if          { count();
            printf( ": %s\n", yytext );
            //  sprintf( parse[sindex].idop, "%s", "if");
            //  parse[sindex].type = IF;
            //  sindex++;
            return IF;
            }

else        { count();
            printf( ": %s\n", yytext );
            //  sprintf( parse[sindex].idop, "%s", "else");
            //  parse[sindex].type = ELSE;
            //  sindex++;
            return ELSE;
            }

while       { count();
            printf( ": %s\n", yytext );
            //  sprintf( parse[sindex].idop, "%s", "else");
            //  parse[sindex].type = WHILE;
            //  sindex++;
            return WHILE;
            }

subroutine  { count();
            printf( ": %s\n", yytext );
            return SUBRTN;
            }

process     { count();
            printf( ": %s\n", yytext );
            return PROCESS;
            }

end         { count();
            printf( ": %s\n", yytext );
            return END;
            }

halt        { count();
            printf( ": %s\n", yytext );
            return HALT;
            }

open        { count();
            printf( "open: %s\n", yytext );
            return OPEN;
            }

read        { count();
            printf( ": %s\n", yytext );
            return READ;
            }

write       { count();
            printf( ": %s\n", yytext );
            return WRITE;
            }

close       { count();
            printf( ": %s\n", yytext );
            return CLOSE;
            }

seek        { count();
            printf( ": %s\n", yytext );
            return SEEK;
            }

{IDENT}     { count();
              printf( "ID: %s\n", yytext );
              yylval.sptr = yytext;
            return ID;
            }

"<="       { count();  yylval.sptr = yytext; 
              sprintf( parse[sindex].idop, "%s", "<=");
              parse[sindex].type = LE_OP;
              sindex++;
              return(LE_OP); 
           }
">="       { count(); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ">=");
              parse[sindex].type = GE_OP;
              sindex++;
              return(GE_OP); 
           }
"<"        { count(); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "<");
              parse[sindex].type = LT_OP;
              sindex++; 
              return(LT_OP); 
           }
">"        { count(); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ">");
              parse[sindex].type = GT_OP;
              sindex++; 
              return(GT_OP); 
           }
"=="       { count(); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "==");
              parse[sindex].type = EQ_OP;
              sindex++; 
              return(EQ_OP);
           }
"!="       { count(); yylval.sptr = yytext; 
              sprintf( parse[sindex].idop, "%s", "!=");
              parse[sindex].type = NE_OP;
              sindex++;
              return(NE_OP);
           }
AND        { count();
            printf("AND: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "AND");
              parse[sindex].type = AND;
              sindex++;
            return AND;
           }

OR         { count();
            printf("OR: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "OR");
              parse[sindex].type = OR;
              sindex++;
            return OR;
           }

NOT        {  count();printf("NOT: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "NOT");
              parse[sindex].type = NOT;
              sindex++;
            return NOT;
           }

"="        {  count();printf("EQ: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "=");
              parse[sindex].type = EQ;
              sindex++;

            return EQ;
           }

"+"        {  count();printf("PLUS: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "+");
              parse[sindex].type = PLUS;
              sindex++;
            return PLUS;
           }

"-"        {  count();printf("MINUS: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "-");
              parse[sindex].type = MINUS;
              sindex++;
            return MINUS;
           }

"*"        {  count();printf("MUL: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "*");
              parse[sindex].type = MUL;
              sindex++;
            return MUL;
           }

"/"        {  count();printf("DIV: %s\n", yytext);  yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "/");
              parse[sindex].type = DIV;
              sindex++;
            return DIV;
           }


"("        {   count();   printf( ": %s\n", yytext ); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "(");
              parse[sindex].type = LP;
              sindex++;
            return LP;
            }

")"        {    count();  printf( ": %s\n", yytext ); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ")");
              parse[sindex].type = RP;
              sindex++;
            return RP;
           }

"{"        {   count();   printf( ": %s\n", yytext ); yylval.sptr = yytext;
             // sprintf( parse[sindex].idop, "%s", "{");
             // parse[sindex].type = LB;
             // sindex++;
            return LB;
           }

"}"        {   count();   printf( ": %s\n", yytext ); yylval.sptr = yytext;
             // sprintf( parse[sindex].idop, "%s", "}");
             // parse[sindex].type = RB;
             // sindex++;
            return RB;
           }

"["        {  count();   printf( ": %s\n", yytext ); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "[");
              parse[sindex].type = LBRCT;
              sindex++;
            return LBRCT;
           }

"]"        {  count();   printf( ": %s\n", yytext ); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "]");
              parse[sindex].type = RBRCT;
              sindex++;
            return RBRCT;
           }

";"        {  count();   printf( ": %s\n", yytext ); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ";");
              parse[sindex].type = SEMICOL;
              sindex++;
            return SEMICOL;
           }

","        {  count();   printf( ": %s\n", yytext ); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", ",");
              parse[sindex].type = COMMA;
              sindex++;
            return COMMA;
           }

"\""       {  count();   printf( ": %s\n", yytext ); yylval.sptr = yytext;
              sprintf( parse[sindex].idop, "%s", "\"");
              parse[sindex].type = DBLQT;
              sindex++;
            return DBLQT;
           }

"//"[^\n]*  { count(); /* end of line */  }

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
