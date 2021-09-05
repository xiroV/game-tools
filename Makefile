CC = g++
OBJDIR = obj
SRCDIR = src
LIBS = -lraylib -lpthread -lrt -lm -ldl
CFLAGS = -Wall


all:	game

game: $(OBJDIR)/main.o $(OBJDIR)/player.o
	$(CC) -o bin/$@ $(OBJDIR)/main.o $(OBJDIR)/player.o $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f bin/game obj/*.o

