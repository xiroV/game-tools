#include<iostream>
#include<string>
#include<sstream>
#include<typeinfo>
#include<vector>
#include<fstream>
#include<raylib.h>

using namespace std;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 500;
const int MOVE_INTERVAL = 10;
const int RESIZE_INTERVAL = 50;
const int CAMERA_MOVE_SPEED = 5;

Vector2 cameraTarget = {WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
float cameraZoom = 1.0f;

bool closeEditor = false;
bool exitWindow = false;
int exitWindowSelectedOption = 0;

string filename = "";
string levelName = "";

enum ObjectType {
    Block,
    Spawn 
};

vector<ObjectType> objectTypes = { ObjectType::Block, ObjectType::Spawn };


struct Object {
    int x, y, width, height;
    int type;
};

Rectangle objectButton = {WINDOW_WIDTH-105, 5, 100, 30};

vector<Object> objects;    
int selected = -1;

string objectTypeToString(int type) {
    switch(type) {
        case ObjectType::Block:
            return "Block";
        case ObjectType::Spawn:
            return "Spawn";
        default:
            return "Block";
    }
}

Color objectTypeColor(int type) {
    switch(type) {
        case ObjectType::Block:
            return RED;
        case ObjectType::Spawn:
            return BLUE;
        default:
            return RED;
    }
}

void saveLevel() {
    ofstream levelFile;
    levelFile.open(filename, ios::out);

    if(levelFile.is_open()) {
        cout << "Saving " << objects.size() << " objects" << endl;

        for (unsigned int i = 0; i < objects.size(); i++) {
            levelFile << objects[i].x << " " << objects[i].y <<
                " " << objects[i].width << " " << objects[i].height << " " << objects[i].type << endl;
        }
    } else {
        cout << "Unable to open file " << filename << endl;
    }

    levelFile.close();
}

void loadLevel() {
    cout << "Loading level file: " << filename << endl;

    string fileLine;
    ifstream levelFile;
    levelFile.open(filename);

    if (levelFile.is_open()) {
        while (getline(levelFile, fileLine)) {
            size_t pos = 0;
            string element;
            vector<int> lineElements;
            istringstream line(fileLine);

            while (getline(line, element, ' ')) {
                lineElements.push_back(stoi(element));
            }

            Object obj = {lineElements[0], lineElements[1], lineElements[2], lineElements[3], lineElements[4]};
            objects.push_back(obj);
        }
    } else {
        cout << "Unable to open file" << endl; 

    }

    levelFile.close();

    cout << "Loaded " << objects.size() << " objects" << endl;
}


bool isElementSelected() {
    if (selected >= 0 && selected < objects.size()) {
        return true;
    }
    return false;
}

bool isWindowOpen() {
    if (exitWindow) {
        return true;
    }
    return false;
}

void control() {
    // EXIT WINDOW
    if (WindowShouldClose()) {
        if (isElementSelected()) {
            selected = -1;
        } else {
            exitWindow = true;
        }
    }

    if (isWindowOpen()) {
        if (exitWindow) {
            if (IsKeyPressed(KEY_ENTER)) {
                if (exitWindowSelectedOption == 0) {
                    if (levelName.size() > 0) {
                        filename = levelName + ".lvl";
                        saveLevel();
                        closeEditor = true;
                    }
                } else if(exitWindowSelectedOption == 1) {
                    closeEditor = true;
                } else {
                    exitWindow = false;
                }
            }

            if (IsKeyPressed(KEY_TAB)) {
                if (exitWindowSelectedOption == 0) {
                    exitWindowSelectedOption = 1;
                } else if (exitWindowSelectedOption == 1) {
                    exitWindowSelectedOption = 2;
                } else {
                    exitWindowSelectedOption = 0;
                }
            } 

            int key = GetCharPressed();
            while(key > 0) {
                if (levelName.size() < 60) {
                    levelName.push_back((char)key);
                }

                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (levelName.size() > 0) {
                    levelName.pop_back();
                }
            }
        }
    } else {
        if (IsKeyPressed(KEY_PAGE_UP)) {
            cameraZoom += 0.5;
        }

        if (IsKeyPressed(KEY_PAGE_DOWN)) {
            cameraZoom -= 0.5;
        }
   

        // NEW OBJECT
        if (IsKeyPressed(KEY_N)) {
            Object obj = {WINDOW_WIDTH/2-50, WINDOW_HEIGHT/2-50, 100, 100};
            objects.push_back(obj);
            selected = objects.size()-1;
        }

        // SWITCH SELECTED
        if (IsKeyPressed(KEY_TAB)) {
            if (selected + 1 < objects.size()) {
                selected += 1;
            } else {
                if (objects.size() > 0) {
                    selected = 0;
                } else {
                    selected = -1;
                }
                        
            }
        }

        if (isElementSelected()) {
            // MOVEMENT
            if (IsKeyPressed(KEY_UP)) {
                objects[selected].y -= MOVE_INTERVAL;
            }

            if (IsKeyPressed(KEY_DOWN)) {
                objects[selected].y += MOVE_INTERVAL;
            }

            if (IsKeyPressed(KEY_LEFT)) {
                objects[selected].x -= MOVE_INTERVAL;
            }

            if (IsKeyPressed(KEY_RIGHT)) {
                objects[selected].x += MOVE_INTERVAL;
            }

            // RESIZING
            if (IsKeyPressed(KEY_W)) {
                objects[selected].height += RESIZE_INTERVAL;
            }

            if (IsKeyPressed(KEY_S)) {
                if (objects[selected].height >= RESIZE_INTERVAL) {
                    objects[selected].height -= RESIZE_INTERVAL;
                }
            }

            if (IsKeyPressed(KEY_A)) {
                if (objects[selected].width >= RESIZE_INTERVAL) {
                    objects[selected].width -= RESIZE_INTERVAL;
                }
            }

            if (IsKeyPressed(KEY_D)) {
                objects[selected].width += RESIZE_INTERVAL;
            }

            // DELETE
            if (IsKeyPressed(KEY_DELETE)) {
                objects.erase(objects.begin()+selected);
                if (objects.size() > 0) {
                    selected = 0;
                } else {
                    selected = -1;
                }
            }

            // CHANGE TYPE
            if (IsKeyPressed(KEY_T)) {
                if (objects[selected].type+1 >= objectTypes.size()) {
                    objects[selected].type = 0;
                } else {
                    objects[selected].type += 1;
                }
            }
        } else {
            // MOVEMENT
            if (IsKeyDown(KEY_UP)) {
                cameraTarget.y -= CAMERA_MOVE_SPEED;
            }

            if (IsKeyDown(KEY_DOWN)) {
                cameraTarget.y += CAMERA_MOVE_SPEED;
            }

            if (IsKeyDown(KEY_LEFT)) {
                cameraTarget.x -= CAMERA_MOVE_SPEED;
            }

            if (IsKeyDown(KEY_RIGHT)) {
                cameraTarget.x += CAMERA_MOVE_SPEED;
            }

        }
    
    }
}

void drawMenu() {
    DrawText("[n] new", WINDOW_WIDTH-105+20, 10, 10, LIGHTGRAY);
    DrawText("[arrows] move ", WINDOW_WIDTH-105+20, 25, 10, LIGHTGRAY);
    DrawText("[wasd] resize", WINDOW_WIDTH-105+20, 40, 10, LIGHTGRAY);
    DrawText("[del] delete", WINDOW_WIDTH-105+20, 55, 10, LIGHTGRAY);
    DrawText("[t] switch type", WINDOW_WIDTH-105+20, 70, 10, LIGHTGRAY);
    DrawText("[page/up] zoom", WINDOW_WIDTH-105+20, 85, 10, LIGHTGRAY);
}

void drawObjects() {
    for (unsigned int i = 0; i < objects.size(); i++) {
        DrawRectangle(objects[i].x, objects[i].y, objects[i].width, objects[i].height, objectTypeColor(objects[i].type));
        DrawText(objectTypeToString(objects[i].type).c_str(), objects[i].x+2, objects[i].y+2, 10, RAYWHITE);
    }

    if (isElementSelected()) {
        DrawRectangleLines(
            objects[selected].x,
            objects[selected].y,
            objects[selected].width,
            objects[selected].height,
            GREEN
        );
    }
}

void drawWindows () {
    if (exitWindow) {
        DrawRectangle(100, 100, WINDOW_WIDTH-200, WINDOW_HEIGHT-200, RAYWHITE);
        DrawText("Please enter a level name", 120, 120, 10, BLACK);
        DrawText(levelName.c_str(), 130, 145, 10, BLACK);
        DrawRectangleLines(120, 140, WINDOW_WIDTH-240, 20, BLACK);

        DrawText("Save & Exit", 130, 185, 10, BLACK);
        DrawText("Close without saving", 220, 185, 10, BLACK);
        DrawText("Cancel", 350, 185, 10, BLACK);

        switch(exitWindowSelectedOption) {
            case 0:
                DrawRectangleLines(120, 180, 80, 20, BLACK);
                break;
            case 1:
                DrawRectangleLines(210, 180, 120, 20, BLACK);
                break;
            case 2:
                DrawRectangleLines(340, 180, 80, 20, BLACK);
                break;
        }

    }
}

int main(int argc, char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RPG Project Level Editor");
    SetTargetFPS(60);

    Camera2D camera = {0};
    camera.target = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
    camera.offset = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;


    if (argc > 1) {
        filename = argv[1];
        loadLevel();
    }

    while (!closeEditor) {
        control();

        camera.zoom += cameraZoom;
        if (camera.zoom > 3.0f) {
            cameraZoom = 3.0f;
            camera.zoom = cameraZoom;
        }
        else if (camera.zoom < 0.1f) {
            cameraZoom = 0.1f;
            camera.zoom = cameraZoom;
        }

        camera.target = cameraTarget;

        BeginDrawing();
            BeginMode2D(camera);
                ClearBackground(BLACK);
                drawObjects();
            EndMode2D();
            drawMenu();
            drawWindows();
        EndDrawing();

        camera.zoom = 0;
    }

    CloseWindow();

    return 0;
}

