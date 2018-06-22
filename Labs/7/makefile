#format is target-name: target dependencies
#{-tab-}actions

# All Targets
all: hexeditplus

hexeditplus:hexeditplus.o
	gcc -g -Wall -o hexeditplus hexeditplus.o

hexeditplus.o: hexeditplus.c
	gcc -g -Wall -c -o hexeditplus.o hexeditplus.c 

#tell make that "clean" is not a file name!
.PHONY: clean

#Clean the build directory
clean: 
	rm -f *.o hexeditplus
