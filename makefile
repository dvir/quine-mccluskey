CC = g++
CFLAGS = -g -Wall -Weffc++
CLIBS = -lboost_program_options

all: qmc

qmc:
		$(CC) $(CFLAGS) -o qmc qmc.cpp $(CLIBS)

clean:
		rm -rf qmc
