UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CC = clang++
    LIBS = -framework IOKit -framework Cocoa -framework OpenGL lib/raylib/lib/libraylib.a
else
    CC = g++
    LIBS = -Wl,--no-as-needed -fpic -lGL -lpthread -lrt -lm -ldl -lX11 lib/raylib/lib/libraylib.a
endif

CFLAGS = -std=c++17 -Wall -Wswitch -Wunused-variable -Wmissing-braces -Wno-enum-compare

all:	editor	

editor: main.o
	$(CC) -o $@ main.o $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o editor

