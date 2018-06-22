#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: toy_printf

# Tool invocations
# Executable "hello" depends on the files hello.o and run.o.
toy_printf: toy_printf.o main.o
	gcc -g -Wall -o main toy_printf.o main.o

# Depends on the source and header files
toy_printf.o: toy_printf.c toy_stdio.h
	gcc -g -Wall -c -o toy_printf.o toy_printf.c 

main.o: main.c
	gcc -g -Wall -c -o main.o main.c


#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o toy_printf