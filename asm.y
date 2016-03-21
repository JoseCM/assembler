%{
#include <stdio.h>
#include "asm.h"

#define YYSTYPE symbStruct*
%}


%token REG
%token ID
%token NUMBER
%token ADD
%token SUB
%token AND
%token OR
%token XOR
%token NOT
%token LOADI
%token LOAD
%token STORE
%token IN
%token OUT
%token JMP
%token BRZ
%token BRNZ
%token NOP

%token WORD
%token ORG
%token EQU
%token ALLOC

%%

stmt_list: stmt | stmt_list stmt

stmt: inst | pragma | label stmt

inst : ADD REG ',' REG ',' REG  { add_inst(OP_ADD, $2, $4, $6, 0); } |
       SUB REG ',' REG ',' REG  { add_inst(OP_SUB, $2, $4, $6, 0); } |
       AND REG',' REG',' REG    { add_inst(OP_AND, $2, $4, $6, 0); } |
       OR REG',' REG',' REG     { add_inst(OP_OR, $2, $4, $6, 0); } |
       XOR REG',' REG',' REG    { add_inst(OP_XOR, $2, $4, $6, 0); } |
       NOT REG',' REG           { add_inst(OP_NOT, $2, $4, NULL, 0); } |
       LOADI REG',' imm         { add_inst(OP_LOADI, $2, NULL, $4 ,0); } |
       LOAD REG',''@'REG        { add_inst(OP_LOAD, $2, NULL, $5 ,  0); } |
       LOAD REG',''@'imm        { add_inst(OP_LOAD, $2, NULL, $5 ,  1); } |
       STORE REG',''@'REG       { add_inst(OP_STORE, $2, NULL, $5 , 0); } |
       STORE REG',''@'imm       { add_inst(OP_STORE, $2, NULL, $5 , 1); } |
       IN REG                   { add_inst(OP_INOUT, $2, NULL, NULL , 0); } |
       OUT REG                  { add_inst(OP_INOUT, NULL, $2, NULL , 1); } |
       JMP '@'REG               { add_inst(OP_JMP, NULL, $3, NULL , 0); } |
       JMP '@'imm               { add_inst(OP_LOADI, r0_ptr, NULL, $3 ,0);
                                  add_inst(OP_JMP, NULL, r0_ptr, NULL , 0); } |
       BRNZ REG',' '@'REG       { add_inst(OP_BRNZ, NULL, $2, $5 , 0); } |
       BRNZ REG',' '@'imm       { add_inst(OP_LOADI, r0_ptr, NULL, $5 ,0);
                                  add_inst(OP_BRNZ, NULL, $2, r0_ptr , 0); } |
       BRZ REG',' '@'REG        { add_inst(OP_BRZ, NULL, $5, $2 , 0); } |
       BRZ REG',' '@'imm        { add_inst(OP_LOADI, r0_ptr, NULL, $5 ,0);
                                  add_inst(OP_BRZ, NULL, r0_ptr, $2 , 0); } |
       NOP                      { add_inst(OP_NOP, NULL, NULL, NULL , 0); }
;

pragma: WORD NUMBER { add_inst(PRAGMA_WORD, $2, NULL, NULL, 0); }  |
        ALLOC NUMBER { add_inst(PRAGMA_ALLOC, $2, NULL, NULL, 0); } |
        ID EQU NUMBER { set_symbol($1->name, $3->value); }  |
        ORG NUMBER { location_counter = $2->value; add_inst(PRAGMA_ORG, $2, NULL, NULL, 0);  }
;

label: ID ':' { set_symbol($1->name, location_counter); }
;

imm:  '#'NUMBER  {$$ = $2;} | ID  {$$ = $1;}
;

%%

int yyerror(char *s){
  printf("ERROR: %s!\n", s);
}

// main(){
//
//   yyparse();
//   return 0;
//
// }
