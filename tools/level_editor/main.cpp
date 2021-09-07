#include<iostream>
#include<string>
#include<sstream>
#include<typeinfo>
#include<vector>
#include<fstream>
#include<raylib.h>

using namespace std;

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 1000;
const int FONT_SIZE = 20;
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

vector<ObjectType> objectTypes = { ObjectType::Block, ObjectType::Spawn};


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
    int xpos = WINDOW_WIDTH-FONT_SIZE*8;

    vector<const char*> entries = {
        "[n] new",
        "[arrows] move",
        "[wasd] resize",
        "[del] delete",
        "[t] switch type",
        "[page/up] zoom"
    };

    int ypos = 10;
    for (unsigned int i = 0; i < entries.size(); i++) {
        DrawText(entries[i], xpos, ypos, FONT_SIZE, LIGHTGRAY);
        ypos += 8+FONT_SIZE;
    }


    if (isElementSelected()) {
        DrawText(
            objectTypeToString(objects[selected].type).c_str(),
            WINDOW_WIDTH-20-objectTypeToString(objects[selected].type).size()*FONT_SIZE*0.6,
            WINDOW_HEIGHT-30,
            FONT_SIZE,
            objectTypeColor(objects[selected].type) 
        );
    }
}

void drawObjects(Camera2D* camera) {
    for (unsigned int i = 0; i < objects.size(); i++) {
        DrawRectangle(objects[i].x, objects[i].y, objects[i].width, objects[i].height, objectTypeColor(objects[i].type));
    }

    if (isElementSelected()) {
        DrawRectangleLinesEx(
            Rectangle {
                (float)objects[selected].x,
                (float)objects[selected].y,
                (float)objects[selected].width,
                (float)objects[selected].height,
            },
            6-camera->zoom*2,
            GREEN
        );
    }
}

void drawWindows () {
    int scale = FONT_SIZE/10;
    int yBase = 120+FONT_SIZE;

    if (exitWindow) {
        DrawRectangle(100, 100, WINDOW_WIDTH-200, WINDOW_HEIGHT-200, RAYWHITE);
        DrawText("Please enter a level name", 120, yBase, FONT_SIZE, BLACK);
        DrawText(levelName.c_str(), 130, yBase+FONT_SIZE*2+FONT_SIZE/2, FONT_SIZE, BLACK);
        DrawRectangleLines(120, yBase+FONT_SIZE*2, WINDOW_WIDTH-240, FONT_SIZE*2, BLACK);

        DrawText("Save & Exit", 120+FONT_SIZE/2, yBase+FONT_SIZE*5+FONT_SIZE/2, FONT_SIZE, BLACK);
        DrawText("Close without saving", 120+75*scale+FONT_SIZE+FONT_SIZE/2, yBase+FONT_SIZE*5+FONT_SIZE/2, FONT_SIZE, BLACK);
        DrawText("Cancel", 120+75*scale+FONT_SIZE+120*scale+FONT_SIZE+FONT_SIZE/2, yBase+FONT_SIZE*5+FONT_SIZE/2, FONT_SIZE, BLACK);

        switch(exitWindowSelectedOption) {
            case 0:
                DrawRectangleLines(120, yBase+FONT_SIZE*5, 75*scale, FONT_SIZE*2, BLACK);
                break;
            case 1:
                DrawRectangleLines(120+75*scale+FONT_SIZE, yBase+FONT_SIZE*5, 120*scale, FONT_SIZE*2, BLACK);
                break;
            case 2:
                DrawRectangleLines(120+75*scale+FONT_SIZE+120*scale+FONT_SIZE, yBase+FONT_SIZE*5, 60*scale, FONT_SIZE*2, BLACK);
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
        if (camera.zoom > 2.0f) {
            cameraZoom = 2.0f;
            camera.zoom = cameraZoom;
        }
        else if (camera.zoom < 0.5f) {
            cameraZoom = 0.5f;
            camera.zoom = cameraZoom;
        }

        camera.target = cameraTarget;

        BeginDrawing();
            BeginMode2D(camera);
                ClearBackground(DARKGRAY);
                drawObjects(&camera);
            EndMode2D();
            drawMenu();
            drawWindows();
        EndDrawing();

        camera.zoom = 0;
    }

    CloseWindow();

    return 0;
}

