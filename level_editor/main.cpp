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
const int ROTATION_INTERVAL = 20;
const int RESIZE_INTERVAL = 50;
const int CAMERA_MOVE_SPEED = 5;

Vector2 cameraTarget = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
float cameraZoom = 1.0f;

bool closeEditor = false;
int exitWindowSelectedOption = 0;

char illegalPathCharacters[] = {'!', '"', '#', '%', '&', '\'', '(', ')', '*', '+', ',', '/', ':', ';', '<', '=', '>', '?', '[', '\\', ']', '^', '`', '{', '|', '}', 0};
char dotList[] = {'.', 0};

string illegalFileNames[] = {
    ".",
    "..",
    "aux",
    "com1",
    "com2",
    "com3",
    "com4",
    "com5",
    "com6",
    "com7",
    "com8",
    "com9",
    "lpt1",
    "lpt2",
    "lpt3",
    "lpt4",
    "lpt5",
    "lpt6",
    "lpt7",
    "lpt8",
    "lpt9",
    "con",
    "nul",
    "prn"
};

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
    int x, y, width, height, rotation;
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
    char outputDelimiter;
    int version;
    bool levelnameError;
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

    if (levelFile.is_open()) {
        levelFile << "#version " << editor->version << endl;
        for (unsigned int i = 0; i < editor->objects.size(); i++) {
            levelFile << editor->objects[i].x << editor->outputDelimiter << editor->objects[i].y <<
                editor->outputDelimiter << editor->objects[i].width << editor->outputDelimiter << editor->objects[i].height << editor->outputDelimiter << editor->objects[i].type << editor->outputDelimiter;
                for (auto &pair : editor->objects[i].data) {
                    // Should also handle multiple empty spaces as invalid.
                    if (pair.key.length() != 0) {
                        levelFile << pair.key << "=" << pair.value << editor->outputDelimiter;
                    }
                }

                levelFile << endl;
        }
    } else {
        cout << "Unable to open file " << filename << endl;
    }

    levelFile.close();
}

std::string toLowerCase(std::string str) {
    std::string result = "";
    for (auto &ch : str) {
        result += std::tolower(ch);
    }
    return result;
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
    int version = 0;

    string fileLine;
    ifstream levelFile;
    levelFile.open(filename);

    if (levelFile.is_open()) {
        
        // Read version
        getline(levelFile, fileLine);
        if (fileLine[0] == '#') {
            version = stoi(fileLine.substr(string("#version ").length(), fileLine.length()));
        };

        if (version > editor->version) {
            cout << endl << "Version of file read is newer than this binary supports. Will try its best to parse the input file." << endl << endl;
        }

        // Only one version currently, so nothing special to do
        while (getline(levelFile, fileLine)) {            
            string element;
            vector<int> lineElements;
            istringstream line(fileLine);

            int objectFieldCount = 0;
            while (objectFieldCount < 5 && getline(line, element, editor->outputDelimiter)) {
                lineElements.push_back(stoi(element));
                objectFieldCount++;
            }

            Object obj = {lineElements[0], lineElements[1], lineElements[2], lineElements[3], lineElements[4]}  ;

            while (getline(line, element, editor->outputDelimiter)) {
                istringstream keyValuePair = istringstream(element);
                string key;
                string value;
                getline(keyValuePair, key, '=');
                getline(keyValuePair, value, editor->outputDelimiter);
                obj.data.push_back({key, value});
            }

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

bool anyMatch(char key, char illegalChars[]) {
    // Currently REQUIRES final entry in array is 0.
    for (int i = 0; illegalChars[i] != 0; i++) {
        if (illegalChars[i] == key) return true;
    }

    return false;
}

void updateStringByCharInput(string &str, const int maxLength, char illegalChars[]) {
    int key = GetCharPressed();
    while (key > 0) {
        if (str.size() < maxLength && !anyMatch(key, illegalChars)) {
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

    if (WindowShouldClose()) {
        editor->state = EditorState::Closing;
    }
    
    switch (editor->state) {
        case EditorState::Closing: {
            if (IsKeyPressed(KEY_ESCAPE)) {
                editor->state = EditorState::Editing;
            }

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

            if (IsKeyPressed(KEY_LEFT)) {
                if (exitWindowSelectedOption == 0) {
                    exitWindowSelectedOption = 2;
                } else {
                    exitWindowSelectedOption -= 1;
                }
            }

            updateStringByCharInput(levelName, 60, illegalPathCharacters);
            
            editor->levelnameError = false;
            bool containsDot = false;
            int dotPlacement = -1;
            for (auto &ch : levelName) {
                bool containsDot = anyMatch(ch, dotList);
                dotPlacement++;
                if (containsDot) break;
            }

            if (containsDot && dotPlacement == 0) {
                editor->levelnameError = true;
            } else {
                for (auto &filename : illegalFileNames) {
                    auto lower = toLowerCase(levelName);
                    if (containsDot) {
                        if (filename == lower.substr(0, dotPlacement)) {
                            editor->levelnameError = true;
                            break;
                        } 
                    } else {
                        if (filename == lower) {
                            editor->levelnameError = true;
                            break;
                        }
                    }
                }
            }

            break;
        } 
        case EditorState::EditKeyValue: {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
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
                updateStringByCharInput(data, 30, illegalPathCharacters);
            }
            break;
        }
        case EditorState::ShowKeyValue: {
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_V)) {
                editor->state = EditorState::Editing;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (editor->objects[editor->selectedObject].data.size() > 0) {
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
            if (IsKeyPressed(KEY_ESCAPE)) {
                editor->state = EditorState::Closing;
            }

            if (IsKeyPressed(KEY_PAGE_UP)) {
                cameraZoom += 0.5;
            }

            if (IsKeyPressed(KEY_PAGE_DOWN)) {
                cameraZoom -= 0.5;
            }
    

            // NEW OBJECT
            if (IsKeyPressed(KEY_N)) {
                Object obj = {WINDOW_WIDTH/2-50, WINDOW_HEIGHT/2-50, 100, 100, 0};
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
                if (IsKeyPressed(KEY_S)) {
                    editor->objects[editor->selectedObject].height += RESIZE_INTERVAL;
                }

                if (IsKeyPressed(KEY_W)) {
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
                
                // ROTATE 
                if (IsKeyPressed(KEY_Q)) {
                    editor->objects[editor->selectedObject].rotation = (editor->objects[editor->selectedObject].rotation - ROTATION_INTERVAL) % 360;

                }

                if (IsKeyPressed(KEY_E)) {
                    editor->objects[editor->selectedObject].rotation = (editor->objects[editor->selectedObject].rotation + ROTATION_INTERVAL) % 360;
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
    /* for (unsigned int i = 0; i < editor->objects.size(); i++) {
        auto &object = editor->objects[i];
        if (object.width == 0 && object.height == 0) {
            DrawCircle(object.x, object.y, 5, objectTypeColor(object.type));
        } else {
            DrawRectanglePro(
                Rectangle {(float)object.x, (float)object.y, (float)object.width, object.height}, Vector2 {object.width / 2, object.height / 2}, object.rotation, objectTypeColor(object.type));
        }
    } */

    if (isElementSelected(editor)) {
        auto &selectedObject = editor->objects[editor->selectedObject];

        if (selectedObject.width == 0 && selectedObject.height == 0) {
            DrawCircleLines(
                selectedObject.x,
                selectedObject.y,
                5.0f,
                GREEN
            );
        } else if (selectedObject.width == 0) {
            DrawRectanglePro(
                Rectangle {
                    (float) selectedObject.x,
                    (float) selectedObject.y,
                    (float) 1.0f,
                    (float) selectedObject.height,
                },
                Vector2 {
                    (float) selectedObject.width / 2,
                    (float) selectedObject.height / 2 
                },
                selectedObject.rotation,
                GREEN
            );
        } else if (selectedObject.height == 0) {
            DrawRectanglePro(
                Rectangle {
                    (float) selectedObject.x,
                    (float) selectedObject.y,
                    (float) selectedObject.width,
                    (float) 1.0,
                },
                Vector2 {
                    (float) selectedObject.width / 2,
                    (float) selectedObject.height / 2 
                },
                selectedObject.rotation,
                GREEN
            );
        } else {
            // TODO: Rotate selected x, y
            static float rotation = 90;

            rotation += GetFrameTime() * 20;
            DrawRectangle(selectedObject.x, selectedObject.y, selectedObject.width, selectedObject.height, GREEN);
            DrawRectanglePro(
                Rectangle {(float)selectedObject.x + selectedObject.width / 2.0f,(float)selectedObject.y + selectedObject.height / 2.0f,(float)selectedObject.width,(float)selectedObject.height},
                Vector2 {(float)selectedObject.width / 2.0f, (float) selectedObject.height / 2.0f},
                rotation,
                RED
            );

            // DrawLineEx(Vector2{(float) selectedObject.x, (float) selectedObject.y}, Vector2 {selectedObject.x + selectedObject.width, selectedObject.y}, 2, GREEN);
            // DrawLineEx(Vector2{(float) selectedObject.x, (float) selectedObject.y}, Vector2 {selectedObject.x, selectedObject.y + selectedObject.height}, 2, GREEN);
            // DrawLineEx(Vector2{(float) selectedObject.x + selectedObject.width, selectedObject.y}, Vector2 {selectedObject.x + selectedObject.width, selectedObject.y +  selectedObject.height}, 2, GREEN);
            // DrawLineEx(Vector2{(float) selectedObject.x, selectedObject.y + selectedObject.height}, Vector2 {selectedObject.x + selectedObject.width, selectedObject.y +  selectedObject.height}, 2, GREEN);
        }
    }
}

void drawWindows(Editor *editor) {
    int scale = FONT_SIZE/10;
    int yBase = 120+FONT_SIZE;

    if (editor->state == EditorState::Closing) {
        DrawRectangle(100, 100, WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200, RAYWHITE);
        DrawText("Please enter a level name", 120, yBase, FONT_SIZE, BLACK);
        DrawText(levelName.c_str(), 130, yBase+FONT_SIZE*2+FONT_SIZE/2, FONT_SIZE, BLACK);
        DrawRectangleLines(120, yBase+FONT_SIZE*2, WINDOW_WIDTH-240, FONT_SIZE*2, editor->levelnameError ? RED : BLACK);
    
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

    Camera2D camera = {};
    camera.target = Vector2 {WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
    camera.offset = Vector2 {WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    Editor editor = {};
    editor.version = 1;
    editor.outputDelimiter = ';';
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

