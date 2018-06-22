#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: calc

calc:main.o asm.o
	gcc -g -Wall -o calc main.o asm.o

# Depends on the source and header files
main.o: main.c
	gcc -g -Wall -c -o main.o main.c 
	
asm.o: asm.s
	nasm -g -f elf64 -F dwarf -w+all -o asm.o asm.s
 
#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o calc
