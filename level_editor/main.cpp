#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <fstream>
#include "editor.hpp"
#include "export/cpp.hpp"
#include "export/lvl.hpp"

#define RAYGUI_IMPLEMENTATION
#include "lib/raylib/src/raylib.h"
#include "lib/raygui/src/raygui.h"

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
            while (objectFieldCount < 4 && getline(line, element, editor->outputDelimiter)) {
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

            Object obj = {lineElements[0], lineElements[1], lineElements[2], lineElements[3], typeId};
            
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
        if ((int) str.length() < maxLength && !anyMatch(key, illegalChars)) {
            str.push_back((char) key);
        }

        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (!str.empty()) {
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
                    if (!editor->levelName.empty()) {
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
                        filename = editor->levelName + "." + exporters[editor->selectedExporter]->getExtension();
                        saveLevel(editor, exporters[editor->selectedExporter]);
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
                if (editor->selectedExporter <= 0) {
                    editor->selectedExporter = exporters.size()  - 1;
                } else {
                    editor->selectedExporter -= 1;
                }
            }

            if (IsKeyPressed(KEY_DOWN)) {
                if (editor->selectedExporter >= (int) exporters.size() - 1) {
                    editor->selectedExporter = 0;
                } else {
                    editor->selectedExporter += 1;
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
        case EditorState::KeyValueEditor: {
            if (IsKeyPressed(KEY_N) && editor->editKeyValueIndex < 0) {
                editor->objects[editor->selectedObject].data.push_back({"", ""});
            } 

            if (IsKeyPressed(KEY_DELETE)) {
                if (
                    editor->editKeyValueIndex >= 0 ||
                    editor->editKeyValueIndex < (int) editor->objects[editor->selectedObject].data.size()
                ) {
                    editor->objects[editor->selectedObject].data.erase(editor->objects[editor->selectedObject].data.begin() + editor->editKeyValueIndex);
                    editor->editKeyValueIndex = -1;
                }
            } 

            if (IsKeyPressed(KEY_ESCAPE)) {
                if (editor->editKeyValueIndex < 0) {
                    editor->editKeyValueIndex = -1;
                    editor->state = EditorState::Editing;
                } else {
                    editor->editKeyValueIndex = -1;
                }
            } 

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_TAB)) {
                if (editor->editKeyValueIndex < 0) {
                    editor->editKeyValueIndex += 1;
                } else {
                    if (editor->keyOrValue == KeyOrValue::Key) {
                        editor->keyOrValue = KeyOrValue::Value;
                    } else {
                        editor->keyOrValue = KeyOrValue::Key;
                        if (editor->editKeyValueIndex >= (int) editor->objects[editor->selectedObject].data.size() - 1) {
                            editor->editKeyValueIndex = -1;
                        } else {
                            editor->editKeyValueIndex += 1;
                        }
                    }
                }
            }

            if (
                (editor->keyOrValue == KeyOrValue::Key && IsKeyPressed(KEY_RIGHT)) ||
                (editor->keyOrValue == KeyOrValue::Value && IsKeyPressed(KEY_LEFT))
            ) {
                if (editor->keyOrValue == KeyOrValue::Key) {
                    editor->keyOrValue = KeyOrValue::Value;
                } else {
                    editor->keyOrValue = KeyOrValue::Key;
                }
            } 


            if (editor->editKeyValueIndex >= 0) {
                // Ah, geez
                ObjectData &current = editor->objects[editor->selectedObject].data[editor->editKeyValueIndex];
                string &data = editor->keyOrValue == KeyOrValue::Key ? current.key : current.value; 
                // Ah, geez end
                updateStringByCharInput(data, 30, illegalPathCharacters);
            }
            break;
        }
        case EditorState::BlockTypeEditor: {
            if (IsKeyPressed(KEY_ESCAPE)) {
                if (editor->editBlockTypeIndex < 0) {
                    editor->editBlockTypeIndex = -1;
                    editor->state = EditorState::Editing;
                } else {
                    editor->editBlockTypeIndex = -1;
                }
            }

            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_TAB)) {
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

            ObjectType &current = editor->objectTypes[editor->editBlockTypeIndex];
            if (!editor->objectTypes.empty() && editor->editBlockTypeIndex >= 0) {
                updateStringByCharInput(current.name, 30, illegalPathCharacters);

                if (IsKeyPressed(KEY_END)) {
                    current.color = (Color) {
                        static_cast<unsigned char>(GetRandomValue(0,255)),
                        static_cast<unsigned char>(GetRandomValue(0, 255)),
                        static_cast<unsigned char>(GetRandomValue(0,255)),
                        255
                    }; 
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
                editor->state = EditorState::BlockTypeEditor;
            }

            // NEW OBJECT
            if (IsKeyPressed(KEY_N)) {
                Object obj = {WINDOW_WIDTH/2-50, WINDOW_HEIGHT/2-50, 100, 100};
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
                    editor->state = EditorState::KeyValueEditor;
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
    if (editor->selectedObject == -1 || editor->state != EditorState::KeyValueEditor) return;

    if(GuiWindowBox({100, 100, WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200}, "Key/Value Editor")) {
        editor->state = EditorState::Editing; 
    }
    
    GuiDrawText("[n] for new key/value pair, [del] to delete", {WINDOW_WIDTH - 550, 100, 200, 100}, 0, BLACK);
    Object &element = editor->objects[editor->selectedObject];


    int offsetY = 200;
    int currentIndex = 0;
    bool editing;

    for (auto &entry : element.data) {
        editing = false;

        if (currentIndex == editor->editKeyValueIndex) {
            editing = true;
        }
    
        if (GuiTextBox({130, (float) offsetY, WINDOW_WIDTH / 2 - 130, FONT_SIZE*2}, const_cast<char*>(entry.key.c_str()), 64, editing && editor->keyOrValue == KeyOrValue::Key)) {
            editor->editKeyValueIndex = currentIndex;
            editor->keyOrValue = KeyOrValue::Key;
        }

        if (GuiTextBox({WINDOW_WIDTH / 2 + 30, (float) offsetY, WINDOW_WIDTH / 2 - 160, FONT_SIZE*2}, const_cast<char*>(entry.value.c_str()), 64, editing && editor->keyOrValue == KeyOrValue::Value)) {
            editor->editKeyValueIndex = currentIndex;
            editor->keyOrValue = KeyOrValue::Value;
        }

        offsetY += FONT_SIZE * 2 + 5;
        currentIndex++;
    }

}

void drawBlockTypeEditor(Editor *editor) {
    if (editor->state != EditorState::BlockTypeEditor) return;

    if(GuiWindowBox({100, 100, WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200}, "Block Type Editor")) {
        editor->state = EditorState::Editing; 
    }
    
    GuiDrawText("[n] for new block type, [del] to delete", {WINDOW_WIDTH - 520, 100, 200, 100}, 0, BLACK);


    int offsetY = 200;
    int currentIndex = 0;
    bool editing;

    for (auto &blockType: editor->objectTypes) {
        editing = false;

        if (currentIndex == editor->editBlockTypeIndex) {
            editing = true;
        }
    
        if (GuiTextBox({130, (float) offsetY, WINDOW_WIDTH / 2 - 130, FONT_SIZE*2}, const_cast<char*>(blockType.name.c_str()), 64, editing && editor->keyOrValue == KeyOrValue::Key)) {
            editor->editBlockTypeIndex = currentIndex;
        }

        DrawRectangle(15 + WINDOW_WIDTH / 2, offsetY, FONT_SIZE*2, FONT_SIZE*2, blockType.color);

        if (editing) {
            GuiDrawText("[end] change color", {10 + WINDOW_WIDTH / 2 + 70, (float) offsetY, WINDOW_WIDTH / 2 - 130, FONT_SIZE * 2}, 0, BLACK);
        }

        offsetY += FONT_SIZE * 2 + 5;
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
    for (unsigned int i = 0; i < editor->objects.size(); i++) {
        if (editor->objects[i].width == 0 && editor->objects[i].height == 0) {
            DrawCircle(editor->objects[i].x, editor->objects[i].y, 5, editor->objectTypes[editor->objects[i].type].color);
        } else if (editor->objects[i].width == 0) {
            DrawRectangle(editor->objects[i].x, editor->objects[i].y, 1, editor->objects[i].height, editor->objectTypes[editor->objects[i].type].color);
        } else if (editor->objects[i].height == 0) {
            DrawRectangle(editor->objects[i].x, editor->objects[i].y, editor->objects[i].width, 1, editor->objectTypes[editor->objects[i].type].color);
        } else {
            DrawRectangle(editor->objects[i].x, editor->objects[i].y, editor->objects[i].width, editor->objects[i].height, editor->objectTypes[editor->objects[i].type].color);
        }
    }

    if (isElementSelected(editor)) {
        if (editor->objects[editor->selectedObject].width == 0 && editor->objects[editor->selectedObject].height == 0) {
            DrawCircleLines(
                editor->objects[editor->selectedObject].x,
                editor->objects[editor->selectedObject].y,
                5,
                GREEN
            );
        } else if (editor->objects[editor->selectedObject].width == 0) {
            DrawRectangleLinesEx(
                Rectangle {
                    (float) editor->objects[editor->selectedObject].x,
                    (float) editor->objects[editor->selectedObject].y,
                    (float) 2.0,
                    (float) editor->objects[editor->selectedObject].height,
                },
                6-camera->zoom*2,
                GREEN
            );
        } else if (editor->objects[editor->selectedObject].height == 0) {
            DrawRectangleLinesEx(
                Rectangle {
                    (float) editor->objects[editor->selectedObject].x,
                    (float) editor->objects[editor->selectedObject].y,
                    (float) editor->objects[editor->selectedObject].width,
                    (float) 2.0,
                },
                6-camera->zoom*2,
                GREEN
            );
        } else {
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
}

void drawWindows(Editor *editor, vector<Exporter*> exporters) {
    int scale = FONT_SIZE/10;
    int yBase = 120+FONT_SIZE;

    if (editor->state == EditorState::Closing) {
        if(GuiWindowBox({100, 100, WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200}, "Exit Level Editor")) {
            editor->state = EditorState::Editing; 
        }

        GuiLabel({120, (float) yBase, 500, FONT_SIZE*2}, "Level name");

        if (editor->levelnameError) { GuiSetState(GUI_STATE_DISABLED); } else {GuiSetState(GUI_STATE_NORMAL); }
        GuiTextBox({120, (float)yBase+FONT_SIZE*2, WINDOW_WIDTH-240, FONT_SIZE*2}, const_cast<char*>(editor->levelName.c_str()), 64, true);
        GuiSetState(GUI_STATE_NORMAL);

        if(exitWindowSelectedOption == 0) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({120, (float) yBase+FONT_SIZE*5, (float) 75*scale, FONT_SIZE*2}, "Save & Exit")) {
            if (editor->levelName.size() > 0) {
                filename = editor->levelName + ".lvl";
                saveLevel(editor, exporters[0]);
                closeEditor = true;
            }        
        }
        if(exitWindowSelectedOption == 0) GuiSetState(GUI_STATE_NORMAL);

        if(exitWindowSelectedOption == 1) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({(float) 120 + 75 * scale + FONT_SIZE, (float) yBase+FONT_SIZE*5, (float) 120*scale, FONT_SIZE*2}, "Close without saving")) {
            closeEditor = true;
        }
        if(exitWindowSelectedOption == 1) GuiSetState(GUI_STATE_NORMAL);

        if(exitWindowSelectedOption == 2) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({(float) 120 + 75 * scale + FONT_SIZE + 120 * scale + FONT_SIZE, (float) yBase + FONT_SIZE * 5, (float) 60 * scale, FONT_SIZE * 2}, "Cancel")) {
            editor->state = EditorState::Editing; 
        }
        if(exitWindowSelectedOption == 2) GuiSetState(GUI_STATE_NORMAL);

    } else if (editor->state == EditorState::Export) {
        if(GuiWindowBox({100, 100, WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200}, "Export Level")) {
            editor->state = EditorState::Editing; 
        }

        GuiLabel({120, (float) yBase, 500, FONT_SIZE*2}, "Level name");

        if (editor->levelnameError) { GuiSetState(GUI_STATE_DISABLED); } else {GuiSetState(GUI_STATE_NORMAL); }
        GuiTextBox({120, (float)yBase+FONT_SIZE*2, WINDOW_WIDTH-240, FONT_SIZE*2}, const_cast<char*>(editor->levelName.c_str()), 64, true);
        GuiSetState(GUI_STATE_NORMAL);


        GuiLabel({120, (float) yBase + FONT_SIZE * 4, 500, FONT_SIZE * 2}, "Export type");

        string exportOptions;
        for (auto &exporter : exporters) {
            exportOptions.append(exporter->getName());
            exportOptions.push_back(';');
        }
        exportOptions.pop_back();

        editor->selectedExporter = GuiComboBox({120, (float) yBase+FONT_SIZE*6, WINDOW_WIDTH-240, FONT_SIZE*2}, const_cast<char*>(exportOptions.c_str()), editor->selectedExporter);

        if(exportWindowSelectedOption == 0) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({120, (float) yBase+FONT_SIZE*9, (float) 75*scale, FONT_SIZE*2}, "Export")) {
            if (editor->levelName.size() > 0) {
                filename = editor->levelName + "." + exporters[editor->selectedExporter]->getExtension();
                saveLevel(editor, exporters[editor->selectedExporter]);
            }
        }
        if(exportWindowSelectedOption == 0) GuiSetState(GUI_STATE_NORMAL);

        if(exportWindowSelectedOption == 1) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({(float) 120 + 75 * scale + FONT_SIZE, (float) yBase+FONT_SIZE*9, (float) 60*scale, FONT_SIZE*2}, "Cancel")) {
            editor->state = EditorState::Editing;
        }
        if(exportWindowSelectedOption == 1) GuiSetState(GUI_STATE_NORMAL);
    }
}

int main(int argc, char **argv) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Level Editor");
    SetTargetFPS(60);

    // So ESCAPE isn't eaten by ShouldWindowClose();
    SetExitKey(KEY_F10);

    Font fontDefault = LoadFontEx("assets/fonts/OverpassMono/OverpassMono-Regular.ttf", FONT_SIZE, 0, 0);
    SetTextureFilter(fontDefault.texture, TEXTURE_FILTER_BILINEAR);
    GuiSetFont(fontDefault);

    
    CppExporter cppExport;
    LvlExporter lvlExport;

    Editor editor = {};
    editor.version = 1;
    editor.outputDelimiter = ';';
    editor.state = EditorState::Export;
    editor.keyOrValue = KeyOrValue::Key;
    editor.selectedObject = -1;
    editor.editKeyValueIndex = -1;
    editor.editBlockTypeIndex = -1;
    editor.defaultFont = fontDefault;
    editor.selectedExporter = 0;

    vector<Exporter*> exporters = {
        &lvlExport,
        &cppExport
    };

    if (argc > 1) {
        filename = argv[1];
        loadLevel(&editor);
    } else {
        filename = "TestLevel1.lvl";
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

