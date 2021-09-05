#include<raylib.h>

class Player {
    int height;
    int width;
    int speed;

    public:
        int x;
        int y;

        Player(); 
        void init(int x, int y);
        void draw();
        void control();
};
