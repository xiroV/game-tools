#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <fstream>
#include <raylib.h>

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
int exitWindowSelectedOption = 0;

string filename = "";
string levelName = "";

enum ObjectType {
    Block,
    Spawn
};

enum ExportType {
    LVL,
    CPP
};
int selectedExportType = ExportType::LVL;

vector<ObjectType> objectTypes = { ObjectType::Block, ObjectType::Spawn};

enum class EditorState {
    Editing,
    Closing,
    ShowKeyValue,
    EditKeyValue
};

struct ObjectData {
    string key;
    string value;
};

struct Object {
    int x, y, width, height;
    int type;
    vector<ObjectData> data;
};

enum class KeyOrValue {
    Key,
    Value
};

struct Editor {
    EditorState state;
    vector<Object> objects; 
    int selectedObject;
    int editKeyValueIndex;
    KeyOrValue keyOrValue;
};

Rectangle objectButton = {WINDOW_WIDTH-105, 5, 100, 30};   

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

void saveLevel(Editor *editor) {
    ofstream levelFile;
    levelFile.open(filename, ios::out);

    if(levelFile.is_open()) {
        cout << "Saving " << editor->objects.size() << " objects" << endl;

        for (unsigned int i = 0; i < editor->objects.size(); i++) {
            levelFile << editor->objects[i].x << " " << editor->objects[i].y <<
                " " << editor->objects[i].width << " " << editor->objects[i].height << " " << editor->objects[i].type << endl;
        }
    } else {
        cout << "Unable to open file " << filename << endl;
    }

    levelFile.close();
}

string exportTypeToString(int type) {
    switch (type) {
        case ExportType::LVL:
            return "LVL";
        case ExportType::CPP:
            return "C++ (not implemented)";
        default:
            return "LVL";
    }
}

void loadLevel(Editor *editor) {
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
            editor->objects.push_back(obj);
        }
    } else {
        cout << "Unable to open file" << endl; 

    }

    levelFile.close();

    cout << "Loaded " << editor->objects.size() << " objects" << endl;
}


bool isElementSelected(Editor *editor) {
    if (editor->selectedObject >= 0 && editor->selectedObject < editor->objects.size()) {
        return true;
    }
    return false;
}

bool isExitWindowOpen(EditorState state) {
    if (EditorState::Closing == state) {
        return true;
    }
    return false;
}

void updateStringByCharInput(string &str, const int maxLength) {
    int key = GetCharPressed();
    while (key > 0) {
        if (str.size() < maxLength) {
            str.push_back((char) key);
        }

        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (str.size() > 0) {
            str.pop_back();
        }
    }
}

void control(Editor *editor) {

    if (IsKeyDown(KEY_LEFT_ALT) && IsKeyDown(KEY_F4)) {
        if (isElementSelected(editor)) {
            editor->selectedObject = -1;
        } else {
            editor->state = EditorState::Closing;
        }
    }

    
    switch (editor->state) {
        case EditorState::Closing: {
            if (IsKeyPressed(KEY_ENTER)) {
                if (exitWindowSelectedOption == 0) {
                    if (levelName.size() > 0) {
                        filename = levelName + ".lvl";
                        saveLevel(editor);
                        closeEditor = true;
                    }
                } else if(exitWindowSelectedOption == 1) {
                    closeEditor = true;
                } else {
                    editor->state = EditorState::Editing;
                }
            }

            if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_RIGHT)) {
                if (exitWindowSelectedOption < 2) { // MOD?
                    exitWindowSelectedOption += 1;
                } else {
                    exitWindowSelectedOption = 0;
                }
            } 

            updateStringByCharInput(levelName, 60);
            break;
        } 
        case EditorState::EditKeyValue: {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {\
                cout << "Setting to ShowKeyValue" << endl;
                editor->state = EditorState::ShowKeyValue;     
            } else {
                if (
                    IsKeyPressed(KEY_TAB) ||
                    (editor->keyOrValue == KeyOrValue::Key && IsKeyPressed(KEY_RIGHT)) ||
                    (editor->keyOrValue == KeyOrValue::Value && IsKeyPressed(KEY_LEFT))
                ) {
                    if (editor->keyOrValue == KeyOrValue::Key) {
                        editor->keyOrValue = KeyOrValue::Value;
                    } else {
                        editor->keyOrValue = KeyOrValue::Key;
                    }
                } 

                // Ah, geez
                ObjectData &current = editor->objects[editor->selectedObject].data[editor->editKeyValueIndex];
                string &data = editor->keyOrValue == KeyOrValue::Key ? current.key : current.value; 
                // Ah, geez end
                updateStringByCharInput(data, 30);
            }
            break;
        }
        case EditorState::ShowKeyValue: {
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_V)) {
                editor->state = EditorState::Editing;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (editor->objects[editor->selectedObject].data.size() > 0) {
                    cout << "EditKeyValue" << endl;
                    editor->state = EditorState::EditKeyValue;
                }
            }

            if (IsKeyPressed(KEY_DOWN)) {
                ++editor->editKeyValueIndex %= editor->objects[editor->selectedObject].data.size();
            }

            if (IsKeyPressed(KEY_UP)) {
                --editor->editKeyValueIndex %= editor->objects[editor->selectedObject].data.size();
            }

            if (IsKeyPressed(KEY_N)) {
                editor->objects[editor->selectedObject].data.push_back({"key", "value"});
            }

            break;
        }
        case EditorState::Editing: {
            if (IsKeyPressed(KEY_PAGE_UP)) {
                cameraZoom += 0.5;
            }

            if (IsKeyPressed(KEY_PAGE_DOWN)) {
                cameraZoom -= 0.5;
            }
    

            // NEW OBJECT
            if (IsKeyPressed(KEY_N)) {
                Object obj = {WINDOW_WIDTH/2-50, WINDOW_HEIGHT/2-50, 100, 100};
                editor->objects.push_back(obj);
                editor->selectedObject = editor->objects.size()-1;
            }

            // SWITCH SELECTED
            if (IsKeyPressed(KEY_TAB)) {
                if (editor->selectedObject + 1 < editor->objects.size()) {
                    editor->selectedObject += 1;
                } else {
                    if (editor->objects.size() > 0) {
                        editor->selectedObject = 0;
                    } else {
                        editor->selectedObject = -1;
                    }
                            
                }
            }
            if (isElementSelected(editor)) {
                // MOVEMENT
                if (IsKeyPressed(KEY_UP)) {
                    editor->objects[editor->selectedObject].y -= MOVE_INTERVAL;
                }

                if (IsKeyPressed(KEY_DOWN)) {
                    editor->objects[editor->selectedObject].y += MOVE_INTERVAL;
                }

                if (IsKeyPressed(KEY_LEFT)) {
                    editor->objects[editor->selectedObject].x -= MOVE_INTERVAL;
                }

                if (IsKeyPressed(KEY_RIGHT)) {
                    editor->objects[editor->selectedObject].x += MOVE_INTERVAL;
                }

                // RESIZING
                if (IsKeyPressed(KEY_W)) {
                    editor->objects[editor->selectedObject].height += RESIZE_INTERVAL;
                }

                if (IsKeyPressed(KEY_S)) {
                    if (editor->objects[editor->selectedObject].height >= RESIZE_INTERVAL) {
                        editor->objects[editor->selectedObject].height -= RESIZE_INTERVAL;
                    }
                }

                if (IsKeyPressed(KEY_A)) {
                    if (editor->objects[editor->selectedObject].width >= RESIZE_INTERVAL) {
                        editor->objects[editor->selectedObject].width -= RESIZE_INTERVAL;
                    }
                }

                if (IsKeyPressed(KEY_D)) {
                    editor->objects[editor->selectedObject].width += RESIZE_INTERVAL;
                }

                // DELETE
                if (IsKeyPressed(KEY_DELETE)) {
                    editor->objects.erase(editor->objects.begin() + editor->selectedObject);
                    if (editor->objects.size() > 0) {
                        editor->selectedObject = 0;
                    } else {
                        editor->selectedObject = -1;
                    }
                }

                // CHANGE TYPE
                if (IsKeyPressed(KEY_T)) {
                    if (editor->objects[editor->selectedObject].type+1 >= objectTypes.size()) {
                        editor->objects[editor->selectedObject].type = 0;
                    } else {
                        editor->objects[editor->selectedObject].type += 1;
                    }
                }

                // OPEN KEY VALUE WINDOW
                if (IsKeyPressed(KEY_V)) {
                    editor->state = EditorState::ShowKeyValue;
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
            break;
        }
    }
}

void drawKeyValueList(Editor *editor) {
    if (editor->selectedObject == -1 || (editor->state != EditorState::ShowKeyValue && editor->state != EditorState::EditKeyValue)) return;
    DrawRectangle(5, 5, WINDOW_WIDTH - 10, WINDOW_HEIGHT - 10, GRAY);
    DrawText("[n] for new Key Value Pair", WINDOW_WIDTH - 280, 10, FONT_SIZE, WHITE);
    Object &element = editor->objects[editor->selectedObject];

    int offsetY = 40;
    int currentIndex = 0;
    for (auto &entry : element.data) {
        DrawText(entry.key.c_str(), 15, offsetY, FONT_SIZE, WHITE);
        DrawText(entry.value.c_str(), 15 + WINDOW_WIDTH / 2, offsetY, FONT_SIZE, WHITE);

        if (currentIndex == editor->editKeyValueIndex) {
            if (editor->state == EditorState::EditKeyValue) {
                switch (editor->keyOrValue) {
                    case KeyOrValue::Key: {
                        DrawRectangleLines(10, offsetY, (WINDOW_WIDTH - 30) / 2, FONT_SIZE, YELLOW);
                        break;
                    }
                    case KeyOrValue::Value: {
                        DrawRectangleLines(10 + WINDOW_WIDTH / 2, offsetY, (WINDOW_WIDTH - 30) / 2, FONT_SIZE, YELLOW);
                        break;
                    }
                }
            } else {
                DrawRectangleLines(10, offsetY, WINDOW_WIDTH - 30, FONT_SIZE, YELLOW);
            }
        }

        offsetY += FONT_SIZE + 4;
        currentIndex++;
    }
}

void drawMenu(Editor *editor) {
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


    if (isElementSelected(editor)) {
        DrawText(
            objectTypeToString(editor->objects[editor->selectedObject].type).c_str(),
            WINDOW_WIDTH - 20 - objectTypeToString(editor->objects[editor->selectedObject].type).size() * FONT_SIZE * 0.6,
            WINDOW_HEIGHT - 30,
            FONT_SIZE,
            objectTypeColor(editor->objects[editor->selectedObject].type) 
        );
    }
}

void drawObjects(Camera2D *camera, Editor *editor) {
    for (unsigned int i = 0; i < editor->objects.size(); i++) {
        DrawRectangle(editor->objects[i].x, editor->objects[i].y, editor->objects[i].width, editor->objects[i].height, objectTypeColor(editor->objects[i].type));
    }

    if (isElementSelected(editor)) {
        DrawRectangleLinesEx(
            Rectangle {
                (float) editor->objects[editor->selectedObject].x,
                (float) editor->objects[editor->selectedObject].y,
                (float) editor->objects[editor->selectedObject].width,
                (float) editor->objects[editor->selectedObject].height,
            },
            6-camera->zoom*2,
            GREEN
        );
    }
}

void drawWindows(Editor *editor) {
    int scale = FONT_SIZE/10;
    int yBase = 120+FONT_SIZE;

    if (editor->state == EditorState::Closing) {
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
                DrawRectangleLines(120 + 75 * scale + FONT_SIZE, yBase+FONT_SIZE*5, 120*scale, FONT_SIZE*2, BLACK);
                break;
            case 2:
                DrawRectangleLines(120 + 75 * scale + FONT_SIZE + 120 * scale + FONT_SIZE, yBase + FONT_SIZE * 5, 60 * scale, FONT_SIZE * 2, BLACK);
                break;
        }

    }
}

int main(int argc, char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Level Editor");
    SetTargetFPS(60);

    // So ESCAPE isn't eaten by ShouldWindowClose();
    SetExitKey(KEY_F10);

    Camera2D camera = {0};
    camera.target = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
    camera.offset = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Editor editor = {};
    editor.state = EditorState::Editing;
    editor.keyOrValue = KeyOrValue::Key;
    editor.selectedObject = -1;
    editor.editKeyValueIndex = 0;

    if (argc > 1) {
        filename = argv[1];
        loadLevel(&editor);
    }

    while (!closeEditor) {
        control(&editor);

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
                drawObjects(&camera, &editor);
            EndMode2D();
            drawMenu(&editor);
            drawWindows(&editor);
            drawKeyValueList(&editor);
        EndDrawing();

        camera.zoom = 0;
    }

    CloseWindow();

    return 0;
}

