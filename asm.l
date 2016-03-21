%{
#include "asm.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include "asm.h"
int temp = 0;
%}

digit  [0-9]
hexa   [0-9a-fA-F]
regnum [0-7]
letter [a-zA-Z]

%%
ADD|add               {yylval = NULL; return (ADD);}
SUB|sub               {yylval = NULL; return (SUB);}
AND|and               {yylval = NULL; return (AND);}
OR|or                 {yylval = NULL; return (OR);}
XOR|xor               {yylval = NULL; return (XOR);}
NOT|not               {yylval = NULL; return (NOT);}
LOADI|loadi           {yylval = NULL; return (LOADI);}
LOAD|load             {yylval = NULL; return (LOAD);}
STORE|store           {yylval = NULL; return (STORE);}
IN|in                 {yylval = NULL; return (IN);}
OUT|out               {yylval = NULL; return (OUT);}
JMP|jmp               {yylval = NULL; return (JMP);}
BRZ|brz               {yylval = NULL; return (BRZ);}
BRNZ|brnz                {yylval = NULL; return (BRNZ);}
NOP|nop                  {yylval = NULL; return (NOP);}
".org"|".ORG"            {yylval = NULL; return (ORG);}
".equ"|".EQU"            {yylval = NULL; return (EQU);}
".word"|".WORD"          {yylval = NULL; return (WORD);}
".alloc"|".ALLOC"        {yylval = NULL; return (ALLOC);}

(r|R){regnum}            {
                         sscanf(yytext, "%*[rR]%d", &temp);
                         yylval = add_symbol(yytext, &temp);
                         return (REG);
                        }

{digit}+                {
                         sscanf(yytext, "%d", &temp);
                         yylval = add_symbol(yytext, &temp);
                         return (NUMBER);
                        }
0(x|X){hexa}+           {
                         sscanf(yytext, "0%*[xX]%x", &temp);
                         yylval = add_symbol(yytext, &temp);
                         return (NUMBER);
                        }
(_|{letter}){letter}*   {
                          yylval = add_symbol(yytext, NULL);
                          return (ID);
                        }

","		{yylval = NULL;		return(',');}
"#"		{yylval = NULL;		return('#');}
"+"		{yylval = NULL;		return('+');}
"-"		{yylval = NULL;		return('-');}
":"		{yylval = NULL;		return(':');}
"@"		{yylval = NULL;		return('@');}

%%

int yywrap() {return 1;}
