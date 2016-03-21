#include <stdlib.h>
#include <string.h>
#include "asm.h"

extern FILE* yyin;

void init_asm(){

   int i = 0, temp = 0;

   inst_count = 0;
   location_counter = 0;
   out = NULL;
   outputfile = "myfile.mif";

   for(i = 0; i < MAX_SYMBOLS; i++)
    symbTable[i] = NULL;

   temp = 0;
   r0_ptr = add_symbol("r0", &temp);

}

int hash_value(char *str){

  int hash = 5381, i;
  int len = strlen(str);

  for(i = 0; i < len; i++)
    hash = (((hash << 5) + hash) + str[i]) % MAX_SYMBOLS;

  return hash;
}

symbStruct* get_symbol(char *sym){

  symbStruct *newsymbol = NULL;
  int hash = 0, len;

  if (sym == NULL || (len = strlen(sym) >= MAX_LEN_SYMBOL)){
    /* ERROR HERE - Invalid symbol */
    return NULL;
  }

  hash = hash_value(sym);
  newsymbol = symbTable[hash];

  while(newsymbol != NULL){
    if(strcmp(newsymbol->name, sym) == 0)
      break;
    newsymbol = newsymbol->next;
  }

  return newsymbol;
}

symbStruct* set_symbol(char *sym, int value){

  symbStruct *newsymbol = get_symbol(sym);

  if(newsymbol){
    newsymbol->value = value;
    newsymbol->valid = 1;
  }

  return newsymbol;

}

symbStruct* add_symbol(char *sym, int *value){

    int len;

    if (sym == NULL || (len = strlen(sym) >= MAX_LEN_SYMBOL)){
    /* ERROR HERE - Invalid symbol */
      return 0;
    }

    symbStruct *newsymbol, *temp = NULL;
    int hash;

    hash = hash_value(sym);
    newsymbol = symbTable[hash];

    while(newsymbol != NULL){
      if(strcmp(newsymbol->name, sym) == 0){
        /* ERROR HERE - symbol already in table */
        return newsymbol;
      }
      temp = newsymbol;
      newsymbol = newsymbol->next;
    }

    newsymbol = malloc(sizeof(symbStruct));
    strcpy(newsymbol->name, sym);
    newsymbol->next = temp;
    if(value){
      newsymbol->value = *value;
      newsymbol->valid = 1;
    } else {
      newsymbol->valid = 0;
    }

    symbTable[hash] = newsymbol;


    return newsymbol;
}

void add_inst(int op, symbStruct* opd, symbStruct* opa, symbStruct* opb, int stb){

  if(inst_count >= MAX_INSTS){
    /* ERROR HERE */
    return;
  }

  instList[inst_count].inst_op = op;
  instList[inst_count].opd = opd;
  instList[inst_count].opa = opa;
  instList[inst_count].opbim = opb;
  instList[inst_count].statusbit = stb;

  //printf("adding instruction %x with rd = %d; ra = %d; rb/im = %d\n", op, opd->value, opa->value, opb->value);

  inst_count++;
  location_counter += 1;
}

void print_inst(int inst){

    unsigned temp = 0x8000;
    int i;

    fprintf(out, "@%d\t", location_counter);

    for(i = 0; i < INST_WIDTH; i++)
      (inst & (temp >> i)) ? fprintf(out, "1") : fprintf(out, "0");

    fprintf(out, "\n");

    location_counter++;

}

void codegen(){

    int inst = 0, i = 0;

    if(!(out = fopen(outputfile, "w"))){
      /* ERROR HERE */
      printf("COuld open output file..\n");
      return;
    }

    location_counter = 0;

    for (i = 0; i < inst_count; i++) {

      inst = 0;
      inst |= (instList[i].inst_op & 0xf) << 12;

      /* TODO */
      /* validaçao dos operandos */
      if( (instList[i].opd && !instList[i].opd->valid) ||
          (instList[i].opa && !instList[i].opa->valid) ||
          (instList[i].opbim && !instList[i].opbim->valid)
        ) {
          /* ERROR HERE - symbol undefined */
          return;
        }

      switch (instList[i].inst_op){

        case OP_ADD: case OP_SUB: case OP_AND: case OP_OR: case OP_XOR:
          case OP_NOT: case OP_NOP: case OP_JMP: case OP_BRZ: case OP_BRNZ:

          inst |= instList[i].opd ? (instList[i].opd->value & 0x7) << 9 : 0;
          inst |= instList[i].opa ? (instList[i].opa->value & 0x7) << 6 : 0;
          inst |= instList[i].opbim ? (instList[i].opbim->value & 0x7) << 3 : 0;
          print_inst(inst);
          break;

        case OP_INOUT:

          if(instList[i].statusbit) {
            inst |= (instList[i].opa->value & 0x7) << 6;
            inst |= 0x1;
          } else {
            inst |= (instList[i].opd->value & 0x7) << 9;
            inst &= ~(0x1);
          }
          print_inst(inst);
          break;

        case OP_LOADI:

          inst |= (instList[i].opd->value & 0x7) << 9;
          inst |= (instList[i].opbim->value & 0xff);
          print_inst(inst);
          break;

        case OP_LOAD:

          if(instList[i].statusbit) {
            inst |= (instList[i].opd->value & 0x7) << 9;
            inst |= (instList[i].opbim->value & 0xff) << 1;
            inst |= 0x1;
          } else {
            inst |= (instList[i].opd->value & 0x7) << 9;
            inst |= (instList[i].opbim->value & 0x7) << 3;
            inst &= ~(0x1);
          }
          print_inst(inst);
          break;

        case OP_STORE:

          if(instList[i].statusbit) {
            inst |= (instList[i].opa->value & 0x7) << 6;
            inst |= ((instList[i].opbim->value & (0x7 << 5)) << 4) |
                        ((instList[i].opbim->value & 0x1f) << 1);
            inst |= 0x1;
          } else {
            inst |= (instList[i].opa->value & 0x7) << 6;
            inst |= (instList[i].opbim->value & 0x7) << 3;
            inst &= ~(0x1);
          }
          print_inst(inst);
          break;

        case PRAGMA_ORG:
          location_counter = instList[i].opd->value;
          break;

        case PRAGMA_WORD:
          inst = instList[i].opd->value;
          print_inst(inst);
          break;

        case PRAGMA_ALLOC:
          for(i = 0; i < instList[i].opd->value; i++){
              print_inst(0);
          }
          break;

      }
    }

    fclose(out);
}

int main(int argc, char *argv[]){

  if(argc < 2)
    return;

  init_asm();

  if(!(yyin = fopen(argv[1], "r"))){
     /* ERROR HERE */
     printf("hello!\n");
    return -1;
  }

  if(yyparse()){
    /* Error Here */
    return -1;
  }

  codegen();

  printf("Assembler exited successfully!\n");

  return 0;
}
