#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <fstream>
#include <raylib.h>
#include "editor.hpp"
#include "export/cpp.hpp"
#include "export/lvl.hpp"

using namespace std;

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 1000;
const int FONT_SIZE = 20;
const int MOVE_INTERVAL = 10;
const int ROTATION_INTERVAL = 15;
const int RESIZE_INTERVAL = 50;
const int CAMERA_MOVE_SPEED = 5;

Vector2 cameraTarget = {WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f};
float cameraZoom = 1.0f;

bool closeEditor = false;
int exitWindowSelectedOption = 0;
int exportWindowSelectedOption = 0;

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

void Editor::drawText(string text, Vector2 position, Color color) {
    DrawTextEx(this->defaultFont, text.c_str(), position, FONT_SIZE, 0, color);
}

int selectedExporter = 0;

Rectangle objectButton = {WINDOW_WIDTH-105, 5, 100, 30};   

void saveLevel(Editor *editor, Exporter* exporter) {
    ofstream levelFile;
    levelFile.open(filename, ios::out);

    if (levelFile.is_open()) {
        levelFile << exporter->generate(editor);
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

void loadLevel(Editor *editor) {
    int version = 0;

    string fileLine;
    ifstream levelFile;
    levelFile.open(filename);
    editor->objectTypes = {};

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

            getline(line, element, editor->outputDelimiter);
            string typeName = element;
            int typeId = -1;
            for (unsigned int i = 0; i < editor->objectTypes.size(); i++) {
                if (editor->objectTypes[i].name == typeName) {
                    typeId = i;
                }
            }

            Color color = (Color) {
                static_cast<unsigned char>(GetRandomValue(0,255)),
                static_cast<unsigned char>(GetRandomValue(0, 255)),
                static_cast<unsigned char>(GetRandomValue(0,255)),
                255
            }; 

            if (typeId < 0) {
                editor->objectTypes.push_back((ObjectType){typeName, color});
                typeId = editor->objectTypes.size() - 1;
            }

            Object obj = {lineElements[0], lineElements[1], lineElements[2], lineElements[3], lineElements[4], typeId};
            
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
    if (editor->selectedObject >= 0 && editor->selectedObject < (int) editor->objects.size()) {
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
        if ((int) str.size() < maxLength && !anyMatch(key, illegalChars)) {
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

void control(Editor *editor, vector<Exporter*> exporters) {

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
                    if (editor->levelName.size() > 0) {
                        filename = editor->levelName + "." + exporters[0]->getExtension();
                        saveLevel(editor, exporters[0]);
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

            updateStringByCharInput(editor->levelName, 60, illegalPathCharacters);
            
            editor->levelnameError = false;
            bool containsDot = false;
            int dotPlacement = -1;
            for (auto &ch : editor->levelName) {
                bool containsDot = anyMatch(ch, dotList);
                dotPlacement++;
                if (containsDot) break;
            }

            if (containsDot && dotPlacement == 0) {
                editor->levelnameError = true;
            } else {
                for (auto &filename : illegalFileNames) {
                    auto lower = toLowerCase(editor->levelName);
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
        case EditorState::Export: {
            if (IsKeyPressed(KEY_ESCAPE)) {
                editor->state = EditorState::Editing;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (exportWindowSelectedOption == 0) {
                    if (editor->levelName.size() > 0) {
                        filename = editor->levelName + "." + exporters[selectedExporter]->getExtension();
                        saveLevel(editor, exporters[selectedExporter]);
                    }
                }
                editor->state = EditorState::Editing;
            }

            if (IsKeyPressed(KEY_TAB) || IsKeyPressed(KEY_RIGHT)) {
                if (exportWindowSelectedOption < 1) { // MOD?
                    exportWindowSelectedOption += 1;
                } else {
                    exportWindowSelectedOption = 0;
                }
            } 

            if (IsKeyPressed(KEY_LEFT)) {
                if (exportWindowSelectedOption == 0) {
                    exportWindowSelectedOption = 1;
                } else {
                    exportWindowSelectedOption -= 1;
                }
            }

            if (IsKeyPressed(KEY_UP)) {
                if (selectedExporter <= 0) {
                    selectedExporter = exporters.size()  - 1;
                } else {
                    selectedExporter -= 1;
                }
            }

            if (IsKeyPressed(KEY_DOWN)) {
                if (selectedExporter >= (int) exporters.size() - 1) {
                    selectedExporter = 0;
                } else {
                    selectedExporter += 1;
                }
            }

            updateStringByCharInput(editor->levelName, 60, illegalPathCharacters);
            
            editor->levelnameError = false;
            bool containsDot = false;
            int dotPlacement = -1;
            for (auto &ch : editor->levelName) {
                bool containsDot = anyMatch(ch, dotList);
                dotPlacement++;
                if (containsDot) break;
            }

            if (containsDot && dotPlacement == 0) {
                editor->levelnameError = true;
            } else {
                for (auto &filename : illegalFileNames) {
                    auto lower = toLowerCase(editor->levelName);
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

            if (IsKeyPressed(KEY_DELETE)) {
                if (editor->objects[editor->selectedObject].data.size() > 0) {
                    editor->objects[editor->selectedObject].data.erase(editor->objects[editor->selectedObject].data.begin() + editor->editKeyValueIndex);
                    editor->editKeyValueIndex = 0;
                }
            }

            break;
        }
        case EditorState::ShowBlockTypes: {
            if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_V)) {
                editor->state = EditorState::Editing;
            }

            if (IsKeyPressed(KEY_ENTER)) {
                editor->state = EditorState::EditBlockTypes;
            }

            if (IsKeyPressed(KEY_DOWN)) {
                ++editor->editBlockTypeIndex %= editor->objectTypes.size();
            }

            if (IsKeyPressed(KEY_UP)) {
                --editor->editBlockTypeIndex %= editor->objectTypes.size();
            }

            if (IsKeyPressed(KEY_N)) {
                editor->objectTypes.push_back({"", BLUE});
            }

            if (IsKeyPressed(KEY_DELETE)) {
                if (editor->objectTypes.size() > 1) {
                    editor->objectTypes.erase(editor->objectTypes.begin() + editor->editBlockTypeIndex);
                    editor->editBlockTypeIndex = 0;
                }
            }

            break;
        }
        case EditorState::EditBlockTypes: {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                editor->state = EditorState::ShowBlockTypes;     
            } else {
                if (
                    IsKeyPressed(KEY_TAB) ||
                    (editor->objectTypeParameter == ObjectTypeParameter::Name && IsKeyPressed(KEY_RIGHT)) ||
                    (editor->objectTypeParameter == ObjectTypeParameter::Color && IsKeyPressed(KEY_LEFT))
                ) {
                    if (editor->objectTypeParameter == ObjectTypeParameter::Name) {
                        editor->objectTypeParameter= ObjectTypeParameter::Color;
                    } else {
                        editor->objectTypeParameter = ObjectTypeParameter::Name;
                    }
                } 

                ObjectType &current = editor->objectTypes[editor->editBlockTypeIndex];
                if (editor->objectTypeParameter == ObjectTypeParameter::Name) {
                    updateStringByCharInput(current.name, 30, illegalPathCharacters);
                } else {
                    if (IsKeyPressed(KEY_SPACE)) {
                        current.color = (Color) {
                            static_cast<unsigned char>(GetRandomValue(0,255)),
                            static_cast<unsigned char>(GetRandomValue(0, 255)),
                            static_cast<unsigned char>(GetRandomValue(0,255)),
                            255
                        }; 
                    }
                }
            }
            break;
        }
        case EditorState::Editing: {
            if (IsKeyPressed(KEY_ESCAPE)) {
                editor->state = EditorState::Closing;
            }

            if (IsKeyPressed(KEY_M)) {
                editor->state = EditorState::Export;
            }

            if (IsKeyPressed(KEY_PAGE_UP)) {
                cameraZoom += 0.5;
            }

            if (IsKeyPressed(KEY_PAGE_DOWN)) {
                cameraZoom -= 0.5;
            }

            if (IsKeyPressed(KEY_Y)) {
                editor->state = EditorState::ShowBlockTypes;
            }

            // NEW OBJECT
            if (IsKeyPressed(KEY_N)) {
                Object obj = {WINDOW_WIDTH/2-50, WINDOW_HEIGHT/2-50, 100, 100, 0};
                editor->objects.push_back(obj);
                editor->selectedObject = editor->objects.size()-1;
            }

            // SWITCH SELECTED
            if (IsKeyPressed(KEY_TAB)) {
                if (editor->selectedObject + 1 < (int) editor->objects.size()) {
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
                    if (editor->objects[editor->selectedObject].type+1 >= (int) editor->objectTypes.size()) {
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
    editor->drawText("[n] for new Key Value Pair", {WINDOW_WIDTH - 280, 10}, WHITE);
    Object &element = editor->objects[editor->selectedObject];

    int offsetY = 40;
    int currentIndex = 0;
    for (auto &entry : element.data) {
        editor->drawText(entry.key, {15, (float) offsetY}, WHITE);
        editor->drawText(entry.value, {15 + WINDOW_WIDTH / 2, (float) offsetY}, WHITE);

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

void drawRect(float x, float y, float width, float height, float rotation, Color color) {
     DrawRectanglePro(
        Rectangle {x + width / 2.0f,y + height / 2.0f,width,height},
        Vector2 {width / 2.0f,  height / 2.0f},
        rotation,
        color
    );
}

void drawBlockTypeEditor(Editor *editor) {
    if (editor->state != EditorState::ShowBlockTypes && editor->state != EditorState::EditBlockTypes) return;
    DrawRectangle(5, 5, WINDOW_WIDTH - 10, WINDOW_HEIGHT - 10, GRAY);
    editor->drawText("[n] for new block type", {WINDOW_WIDTH - 280, 10}, WHITE);

    int offsetY = 40;
    int currentIndex = 0;
    for (auto &blockType: editor->objectTypes) {
        editor->drawText(blockType.name, {15, (float) offsetY}, WHITE);
        DrawRectangle(15 + WINDOW_WIDTH / 2, offsetY, 100, 20, blockType.color);

        if (currentIndex == editor->editBlockTypeIndex) {
            if (editor->state == EditorState::EditBlockTypes) {
                switch (editor->objectTypeParameter) {
                    case ObjectTypeParameter::Name: {
                        DrawRectangleLines(10, offsetY, (WINDOW_WIDTH - 30) / 2, FONT_SIZE, YELLOW);
                        break;
                    }
                    case ObjectTypeParameter::Color: {
                        DrawRectangleLines(10 + WINDOW_WIDTH / 2, offsetY, (WINDOW_WIDTH - 30) / 2, FONT_SIZE, YELLOW);
                        editor->drawText("[space] to change", {10 + WINDOW_WIDTH / 2 + 120, (float) offsetY}, WHITE);
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
        "[page/up] zoom",
        "[y] edit types",
        "[v] key/values",
        "[m] export"
    };

    int ypos = 10;
    for (unsigned int i = 0; i < entries.size(); i++) {
        editor->drawText(entries[i], {(float) xpos, (float) ypos}, LIGHTGRAY);
        ypos += 8+FONT_SIZE;
    }

    if (isElementSelected(editor)) {
        editor->drawText(
            editor->objectTypes[editor->objects[editor->selectedObject].type].name,
            { 20, WINDOW_HEIGHT - 30},
            editor->objectTypes[editor->objects[editor->selectedObject].type].color
        );
    }
}

void drawObjects(Camera2D *camera, Editor *editor) {
    if (isElementSelected(editor)) {
        auto &selectedObject = editor->objects[editor->selectedObject];

        if (selectedObject.width == 0 && selectedObject.height == 0) {
            DrawCircleLines(
                selectedObject.x,
                selectedObject.y,
                5.0f,
                GREEN
            );
        } else {
            drawRect(selectedObject.x - 1, selectedObject.y - 1, selectedObject.width + 2, selectedObject.height + 2, selectedObject.rotation, GREEN);
        }
    }
    for (unsigned int i = 0; i < editor->objects.size(); i++) {
        auto &object = editor->objects[i];
        if (object.width == 0 && object.height == 0) {
            DrawCircle(object.x, object.y, 5, editor->objectTypes[object.type].color);
        } else {
            drawRect(object.x, object.y, object.width == 0 ? 1.0f : object.width, object.height == 0 ? 1.0f : object.height, object.rotation, editor->objectTypes[object.type].color);
        }
    }
}

void drawWindows(Editor *editor, vector<Exporter*> exporters) {
    int scale = FONT_SIZE/10;
    int yBase = 120+FONT_SIZE;

    if (editor->state == EditorState::Export) {
        DrawRectangle(100, 100, WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200, RAYWHITE);
        editor->drawText("Please enter a level name", {120, (float) yBase});
        editor->drawText(editor->levelName, {130, (float) yBase+FONT_SIZE+FONT_SIZE/2});
        DrawRectangleLines(120, yBase+FONT_SIZE, WINDOW_WIDTH-240, FONT_SIZE*2, editor->levelnameError ? RED : BLACK);

        editor->drawText("Export type", {120, (float)yBase+FONT_SIZE*4});
        DrawRectangleLines(120, yBase+FONT_SIZE*5, WINDOW_WIDTH-240, FONT_SIZE*2, BLACK);
        editor->drawText(exporters[selectedExporter]->getName(), {120+FONT_SIZE/2, (float)yBase+FONT_SIZE*5+FONT_SIZE/2});

        editor->drawText("Export", {120+FONT_SIZE/2, (float) yBase+FONT_SIZE*8+FONT_SIZE/2});
        editor->drawText("Cancel", {(float) 120+75*scale+FONT_SIZE+FONT_SIZE/2, (float) yBase+FONT_SIZE*8+FONT_SIZE/2});

        switch(exportWindowSelectedOption) {
            case 0:
                DrawRectangleLines(120, yBase+FONT_SIZE*8, 75*scale, FONT_SIZE*2, BLACK);
                break;
            case 1:
                DrawRectangleLines(120 + 75 * scale + FONT_SIZE, yBase+FONT_SIZE*8, 75*scale, FONT_SIZE*2, BLACK);
                break;
        }

    } else if (editor->state == EditorState::Closing) {
        DrawRectangle(100, 100, WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200, RAYWHITE);
        editor->drawText("Please enter a level name", {120, (float) yBase});
        editor->drawText(editor->levelName, {130, (float) yBase+FONT_SIZE+FONT_SIZE/2});
        DrawRectangleLines(120, yBase+FONT_SIZE, WINDOW_WIDTH-240, FONT_SIZE*2, editor->levelnameError ? RED : BLACK);

        int yOffset = 5;
        editor->drawText("Save & Exit", {120+FONT_SIZE/2, (float) yBase+FONT_SIZE*yOffset+FONT_SIZE/2});
        editor->drawText("Close without saving", {(float) 120+75*scale+FONT_SIZE+FONT_SIZE/2, (float) yBase+FONT_SIZE*yOffset+FONT_SIZE/2});
        editor->drawText("Cancel", {(float) 120+75*scale+FONT_SIZE+120*scale+FONT_SIZE+FONT_SIZE/2, (float) yBase+FONT_SIZE*yOffset+FONT_SIZE/2});

        switch(exitWindowSelectedOption) {
            case 0:
                DrawRectangleLines(120, yBase+FONT_SIZE*yOffset, 75*scale, FONT_SIZE*2, BLACK);
                break;
            case 1:
                DrawRectangleLines(120 + 75 * scale + FONT_SIZE, yBase+FONT_SIZE*yOffset, 120*scale, FONT_SIZE*2, BLACK);
                break;
            case 2:
                DrawRectangleLines(120 + 75 * scale + FONT_SIZE + 120 * scale + FONT_SIZE, yBase + FONT_SIZE * yOffset, 60 * scale, FONT_SIZE * 2, BLACK);
                break;
        }

    }
}

int main(int argc, char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Level Editor");
    SetTargetFPS(60);

    // So ESCAPE isn't eaten by ShouldWindowClose();
    SetExitKey(KEY_NULL);

    Font fontDefault = LoadFontEx("assets/fonts/OverpassMono/OverpassMono-Regular.ttf", FONT_SIZE, 0, 0);
    SetTextureFilter(fontDefault.texture, TEXTURE_FILTER_BILINEAR);

    
    CppExporter cppExport;
    LvlExporter lvlExport;

    Editor editor = {};
    editor.version = 1;
    editor.outputDelimiter = ';';
    editor.state = EditorState::Editing;
    editor.keyOrValue = KeyOrValue::Key;
    editor.selectedObject = -1;
    editor.editKeyValueIndex = 0;
    editor.editBlockTypeIndex = 0;
    editor.defaultFont = fontDefault;

    vector<Exporter*> exporters = {
        &lvlExport,
        &cppExport
    };

    if (argc > 1) {
        filename = argv[1];
        loadLevel(&editor);
    }

    Camera2D camera = {};
    camera.target = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
    camera.offset = (Vector2){WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    if (editor.objects.size() > 0) {
        cameraTarget = {(float) editor.objects[0].x, (float) editor.objects[0].y};
    }

    while (!closeEditor) {
        control(&editor, exporters);

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
            drawWindows(&editor, exporters);
            drawKeyValueList(&editor);
            drawBlockTypeEditor(&editor);
        EndDrawing();

        camera.zoom = 0;
    }

    CloseWindow();

    return 0;
}

