#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <fstream>
#include "editor.hpp"
#include "export/cpp.hpp"
#include "export/lvl.hpp"
#include "export/swift.hpp"

#ifndef RAYGUI
#define RAYGUI
#define RAYGUI_IMPLEMENTATION
#include "lib/raygui/src/raygui.h"
#endif

#include "lib/raylib/src/raylib.h"
#include "window/export.cpp"
#include "window/exit.cpp"
#include "window/block_type_editor.cpp"
#include "window/key_value_editor.cpp"

using namespace std;

const int MOVE_INTERVAL = 10;
const int ROTATION_INTERVAL = 15;
const int RESIZE_INTERVAL = 50;
const int CAMERA_MOVE_SPEED = 5;

float cameraZoom = 1.0f;

int exitWindowSelectedOption = 0;

struct Windows {
    ExportWindow exportWindow;
    ExitWindow exitWindow;
    BlockTypeEditorWindow blockTypeEditorWindow;
    KeyValueEditorWindow keyValueEditorWindow;
};

string filename = "";

void Editor::drawText(string text, Vector2 position, Color color) {
    DrawText(text.c_str(), position.x, position.y, (float) this->fontSize, color);
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
    return editor->selectedObject >= 0 && editor->selectedObject < (int) editor->objects.size();
}

void control(Editor *editor, Windows *windows, vector<Exporter*> exporters) {

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
            windows->exitWindow.control();
            break;
        } 
        case EditorState::Export: {
            windows->exportWindow.control();
            break;
        }
        case EditorState::KeyValueEditor: {
            windows->keyValueEditorWindow.control();
            break;
        }
        case EditorState::BlockTypeEditor: {
            windows->blockTypeEditorWindow.control();
            break;
        }
        case EditorState::Editing: {
            if (IsKeyPressed(KEY_ESCAPE)) {
                if (editor->selectedObject >= 0) {
                    editor->selectedObject = -1;
                } else {
                    editor->state = EditorState::Closing;
                }
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
                Object obj = {(int) editor->windowWidth/2-50, (int) editor->windowHeight/2-50, 100, 100, 0};
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
                    editor->state = EditorState::KeyValueEditor;
                }

             } else {
                // MOVEMENT
                if (IsKeyDown(KEY_UP)) {
                    editor->cameraTarget.y -= CAMERA_MOVE_SPEED;
                }

                if (IsKeyDown(KEY_DOWN)) {
                    editor->cameraTarget.y += CAMERA_MOVE_SPEED;
                }

                if (IsKeyDown(KEY_LEFT)) {
                    editor->cameraTarget.x -= CAMERA_MOVE_SPEED;
                }

                if (IsKeyDown(KEY_RIGHT)) {
                    editor->cameraTarget.x += CAMERA_MOVE_SPEED;
                }

            }
            break;
        }
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

void drawMenu(Editor *editor) {
    int xpos = editor->windowWidth-editor->fontSize*8;

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
        ypos += 8+editor->fontSize;
    }

    if (isElementSelected(editor)) {
        editor->drawText(
            editor->objectTypes[editor->objects[editor->selectedObject].type].name,
            { 20, editor->windowHeight - 30},
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

void drawWindows(Editor *editor, Windows *windows, vector<Exporter*> exporters) {
    switch (editor->state) {
        case EditorState::Closing: {
            windows->exitWindow.draw();
            break;
        }
        case EditorState::Export: {
            windows->exportWindow.draw();
            break;
        }
        case EditorState::BlockTypeEditor: {
            windows->blockTypeEditorWindow.draw();
            break;
        }
        case EditorState::KeyValueEditor: {
            windows->keyValueEditorWindow.draw();
            break;
        }
        default: {}
    }
}

int main(int argc, char **argv) {
    CppExporter cppExport;
    LvlExporter lvlExport;
    SwiftExporter swiftExport;

    Editor editor = {};
    editor.version = 1;
    editor.outputDelimiter = ';';
    editor.state = EditorState::Editing;
    editor.keyOrValue = KeyOrValue::Key;
    editor.selectedObject = -1;
    editor.editKeyValueIndex = -1;
    editor.editBlockTypeIndex = -1;
    editor.selectedExporter = 0;
    editor.windowWidth = 1600;
    editor.windowHeight = 1000;
    editor.fontSize = 20;
    editor.cameraTarget = {editor.windowWidth/2.0f, editor.windowHeight/2.0f};
    editor.closeEditor = false;

    InitWindow((int) editor.windowWidth, (int) editor.windowHeight, "Level Editor");
    SetTargetFPS(60);

    // So ESCAPE isn't eaten by ShouldWindowClose();
    SetExitKey(KEY_NULL);

    Font fontDefault = LoadFontEx("assets/fonts/OverpassMono/OverpassMono-Regular.ttf", editor.fontSize, 0, 0);
    SetTextureFilter(fontDefault.texture, TEXTURE_FILTER_BILINEAR);
    GuiSetFont(fontDefault);

    vector<Exporter*> exporters = {
        &lvlExport,
        &cppExport,
        &swiftExport
    };

    if (argc > 1) {
        filename = argv[1];
        loadLevel(&editor);
    } else {
        filename = "TestLevel1.lvl";
        loadLevel(&editor);
    }

    Windows windows = {
        ExportWindow(&editor, exporters),
        ExitWindow(&editor, exporters[0]),
        BlockTypeEditorWindow(&editor),
        KeyValueEditorWindow(&editor)
    };

    Camera2D camera = {};
    camera.target = (Vector2){editor.windowWidth/2.0f, editor.windowHeight/2.0f};
    camera.offset = (Vector2){editor.windowWidth/2.0f, editor.windowHeight/2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    if (editor.objects.size() > 0) {
        editor.cameraTarget = Vector2 {(float) editor.objects[0].x, (float) editor.objects[0].y};
    }

    while (!editor.closeEditor) {
        control(&editor, &windows, exporters);

        camera.zoom += cameraZoom;
        if (camera.zoom > 2.0f) {
            cameraZoom = 2.0f;
            camera.zoom = cameraZoom;
        }
        else if (camera.zoom < 0.5f) {
            cameraZoom = 0.5f;
            camera.zoom = cameraZoom;
        }

        camera.target = editor.cameraTarget;

        BeginDrawing();
            BeginMode2D(camera);
                ClearBackground(DARKGRAY);
                drawObjects(&camera, &editor);
            EndMode2D();
            drawMenu(&editor);
            drawWindows(&editor, &windows, exporters);
        EndDrawing();

        camera.zoom = 0;
    }

    CloseWindow();

    return 0;
}

