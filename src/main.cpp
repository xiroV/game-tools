#include<iostream>
#include<string>
#include<vector>
#include<raylib.h>
#include"player.h"
using namespace std;

int main(int argc, char **argv) {
    InitWindow(800, 450, "RPG Project");
    Player player = Player();
    player.init(500, 300);
    SetTargetFPS(60);


    while (!WindowShouldClose()) {
        player.control();

        BeginDrawing();
            ClearBackground(BLACK);
            player.draw(); 
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
