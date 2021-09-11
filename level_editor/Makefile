CC = g++
LIBS = -lraylib -lpthread -lrt -lm -ldl
CFLAGS = -Wall -std=c++17


all:	editor	

editor: main.o
	$(CC) -o $@ main.o $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o editor

