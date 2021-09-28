UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CC = clang++
    LIBS = -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`
else
    CC = g++
    LIBS = -lraylib -lpthread -lrt -lm -ldl
endif

CFLAGS = -std=c++17 -Wall -Wswitch -Wunused-variable -Wmissing-braces

all:	editor	

editor: main.o
	$(CC) -o $@ main.o $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o editor

