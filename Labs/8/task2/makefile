#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: myELF

myELF:task2.o
	gcc -g -Wall -o myELF task2.o

task2.o: task2.c
	gcc -g -m64 -Wall -c -o task2.o task2.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o myELF
