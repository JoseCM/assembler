

asm.elf: asm.tab.c asm.tab.h lex.yy.c asm.o
	gcc -g asm.tab.c lex.yy.c asm.o -o asm.elf

asm.o: asmcc.c asm.h
	gcc -g -c asmcc.c -o asm.o

asm.tab.c asm.tab.h: asm.y
	bison -d asm.y

lex.yy.c: asm.l asm.tab.h
	flex asm.l

clean:
	rm *.yy.c *.tab.* *.elf *.o
