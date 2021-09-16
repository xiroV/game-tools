# gcc main.c -o game -lraylib -std=c99

clang++ -std=c++17 main.cpp -o editor -framework IOKit -framework Cocoa -framework OpenGL `pkg-config --libs --cflags raylib`
