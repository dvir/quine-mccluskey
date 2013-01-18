CC = g++
CFLAGS = -g -Wall -Weffc++
CLIBS = -lboost_program_options

all: qmc

qmc: bin/binary.o bin/term.o
	$(CC) $(CFLAGS) -o bin/qmc src/qmc.cpp bin/binary.o bin/term.o $(CLIBS)

bin/term.o: bin/binary.o src/term.cpp include/term.h
	$(CC) $(CFLAGS) -c -o bin/term.o src/term.cpp

bin/binary.o: src/binary.cpp include/binary.h
	$(CC) $(CFLAGS) -c -o bin/binary.o src/binary.cpp

clean:
	rm -f bin/*
