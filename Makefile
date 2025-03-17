#Makefile

# Specify compiler
CC = gcc
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CCFLAGS = -std=gnu99 -Wall -g3 -c

# object files
OBJS = main.o log.o queue.o carProduction.o robotFunctionality.o

# Program name
PROGRAM = carassemble

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -pthread -lrt -o $(PROGRAM) $(OBJS)

main.o : main.c 
	$(CC) $(CCFLAGS) main.c

log.o: log.c log.h
	$(CC) $(CCFLAGS) log.c

queue.o: queue.c queue.h
	$(CC) $(CCFLAGS) queue.c

carProduction.o: carProduction.c carProduction.h
	$(CC) $(CCFLAGS) carProduction.c

robotFunctionality.o: robotFunctionality.c robotFunctionality.h
	$(CC) $(CCFLAGS) robotFunctionality.c

# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -f $(OBJS) *~ $(PROGRAM)

