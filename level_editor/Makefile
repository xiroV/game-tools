UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CC = clang++
    LIBS = -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`
else
    CC = g++
    LIBS = -Wl,--no-as-needed -fpic -lGL -lpthread -lrt -lm -ldl -lX11 lib/raylib/libraylib.a
endif

CFLAGS = -std=c++17 -Wall -Wswitch -Wunused-variable -Wmissing-braces

all:	editor	

editor: main.o
	$(CC) -o $@ main.o $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o editor

