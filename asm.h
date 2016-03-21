#ifndef _ASM_H_
#define _ASM_H_

#include <stdio.h>

/* ISA defines */
#define INST_WIDTH  16

/* Opcode List */

#define OP_ADD    0XC
#define OP_SUB    0x1
#define OP_AND    0x2
#define OP_OR     0x3
#define OP_XOR    0x4
#define OP_NOT    0x5
#define OP_LOADI  0x6
#define OP_LOAD   0x7
#define OP_STORE  0x8
#define OP_INOUT  0xD
#define OP_JMP    0x9
#define OP_BRZ    0xA
#define OP_BRNZ   0xB
#define OP_NOP    0x0

/* assembler data structures */

FILE *out;
char *outputfile;
int location_counter;

/*Data structs and methods for IR and symb table */

#define MAX_SYMBOLS       128
#define MAX_LEN_SYMBOL    64
#define MAX_INSTS         512

typedef struct symb{

  char name[MAX_LEN_SYMBOL];
  int value;
  struct symb *next;
  int valid;

} symbStruct;

symbStruct*  symbTable[MAX_SYMBOLS];
symbStruct*  zero_ptr, *r0_ptr;

struct {

  int   inst_op;
  symbStruct  *opd, *opa , *opbim;
  int   statusbit;

} instList[MAX_INSTS];

int inst_count;

symbStruct*  add_symbol(char *sym, int *value);
symbStruct*  get_symbol(char *sym);
symbStruct*  set_symbol(char *sym, int value);

void add_inst(int op, symbStruct* opd, symbStruct* opa, symbStruct* opb, int stb);
void print_inst(int);

void codegen();

#endif
