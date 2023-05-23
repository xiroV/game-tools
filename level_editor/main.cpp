#include <iostream>
#include <string>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <fstream>
#include "editor.hpp"
#include "import/lvl.hpp"
#include "import/json.hpp"
#include "export/lvl.hpp"
#include "export/json.hpp"
#include "export/cpp.hpp"
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

constexpr int MOVE_INTERVAL = 10;
constexpr int MOVE_DELAY = 5;
constexpr int MOVE_HOLD_DELAY = 40;
constexpr int ROTATION_INTERVAL = 15;
constexpr int RESIZE_INTERVAL = 50;
constexpr int CAMERA_MOVE_SPEED = 5;
constexpr int GRID_DISTANCE = 50;

float cameraZoom = 1.0f;
float timeSinceLastMove = 0;
float timeSinceLastMovePress = 0;

int exitWindowSelectedOption = 0;

struct Windows {
    ExportWindow exportWindow;
    ExitWindow exitWindow;
    BlockTypeEditorWindow blockTypeEditorWindow;
    KeyValueEditorWindow keyValueEditorWindow;
};

void loadLevel(std::string filename, Editor *editor, map<string, Importer*> importers) {
    string fileLine;
    ifstream levelFile;

    string fileType = filename.substr(filename.find_last_of('.') + 1, filename.length());

    if (importers.find(fileType) == importers.end()) {
        editor->addMessage("Unknown file type\n", 5, ERROR); 
        return;
    }

    levelFile.open(filename);
    editor->objectTypes = {};

    if (levelFile.is_open()) {
        importers[fileType]->consume(&levelFile, editor);
    } else {
        editor->addMessage("Unable to open file\n", 5, ERROR); 
    }

    levelFile.close();

    char buffer[50];
    sprintf(buffer, "Loaded %lu objects\n", editor->objects.size());
    editor->addMessage(buffer, 5, SUCCESS);
}

bool isElementSelected(Editor *editor) {
    return editor->selectedObject >= 0 && editor->selectedObject < (int) editor->objects.size();
}

void copyBlock(Editor *editor) {
    if (isElementSelected(editor)) {
        const auto selectedObject = editor->objects[editor->selectedObject];
        Object obj = {selectedObject.x, selectedObject.y, selectedObject.width, selectedObject.height, selectedObject.rotation};
        obj.data = selectedObject.data;
        editor->objects.push_back(obj);
        editor->selectedObject = editor->objects.size() - 1;
        editor->addMessage("Block copied\n", 3, SUCCESS);
    } else {
        editor->addMessage("No block selected\n", 3, ERROR);
    }
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

    if (IsKeyPressed(KEY_F10)) {
        editor->showFPS = !editor->showFPS;
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

            if (IsKeyPressed(KEY_G)) {
                editor->showGrid = !editor->showGrid;
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

            if (IsKeyPressed(KEY_C)) {
                copyBlock(editor);
            }

            if (isElementSelected(editor)) {
                bool upPressed = IsKeyPressed(KEY_UP);
                bool downPressed = IsKeyPressed(KEY_DOWN);
                bool leftPressed = IsKeyPressed(KEY_LEFT);
                bool rightPressed = IsKeyPressed(KEY_RIGHT);
                bool isMoveDown = IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) ||
                    IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT);

                // MOVEMENT
                if (upPressed) {
                    editor->objects[editor->selectedObject].y -= MOVE_INTERVAL;
                    timeSinceLastMovePress = 0;
                }

                if (downPressed) {
                    editor->objects[editor->selectedObject].y += MOVE_INTERVAL;
                    timeSinceLastMovePress = 0;
                }

                if (leftPressed) {
                    editor->objects[editor->selectedObject].x -= MOVE_INTERVAL;
                    timeSinceLastMovePress = 0;
                }

                if (rightPressed) {
                    editor->objects[editor->selectedObject].x += MOVE_INTERVAL;
                    timeSinceLastMovePress = 0;
                }

                if (timeSinceLastMovePress > MOVE_HOLD_DELAY) {
                    if (IsKeyDown(KEY_UP) && !upPressed) {
                        if (timeSinceLastMove > MOVE_DELAY) {
                            editor->objects[editor->selectedObject].y -= MOVE_INTERVAL;
                        }
                    }

                    if (IsKeyDown(KEY_DOWN) && !downPressed) {
                        if (timeSinceLastMove > MOVE_DELAY) {
                            editor->objects[editor->selectedObject].y += MOVE_INTERVAL;
                        }
                    }

                    if (IsKeyDown(KEY_LEFT) && !leftPressed) {
                        if (timeSinceLastMove > MOVE_DELAY) {
                            editor->objects[editor->selectedObject].x -= MOVE_INTERVAL;
                        }
                    }

                    if (IsKeyDown(KEY_RIGHT) && !rightPressed) {
                        if (timeSinceLastMove > MOVE_DELAY) {
                            editor->objects[editor->selectedObject].x += MOVE_INTERVAL;
                        }
                    }
                }

                if (isMoveDown) {
                    timeSinceLastMovePress += GetFrameTime() * 100;
                    if (timeSinceLastMove > MOVE_DELAY) {
                        timeSinceLastMove = 0;
                    } else {
                         timeSinceLastMove += GetFrameTime() * 100; 
                    }
                } else {
                    timeSinceLastMove = 0;
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

void drawHelp(Editor *editor) {
    int xpos = editor->windowWidth-editor->fontSize*8-30;

    static std::array<const char*, 14> entries = {{
        "[n] new",
        "[tab] cycle objects",
        "[arrows] move",
        "[wasd] resize",
        "[del] delete",
        "[t] switch type",
        "[page/up] zoom",
        "[y] edit types",
        "[v] key/values",
        "[m] export",
        "[g] toggle grid",
        "[c] copy block",
        "[esc] deselect/exit",
        "[F10] toggle FPS"
    }};

    if (editor->showHelp) {
        editor->showHelp = !GuiWindowBox({(float) xpos - 50, 10, 230, 432}, "Help");

        int ypos = 50;
        for (unsigned int i = 0; i < entries.size(); i++) {
            GuiLabel({(float) xpos-40, (float) ypos, 100, 20}, entries[i]);
            ypos += 8 + editor->fontSize;
        }
    } else {
        editor->showHelp = GuiButton({(float) xpos + 150, 10, 30, 30}, "?");
    }

    if (isElementSelected(editor)) {
        DrawText(
            editor->objectTypes[editor->objects[editor->selectedObject].type].name.c_str(),
            20, editor->windowHeight - 30, editor->fontSize,
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

void drawGrid(Editor &editor, Camera2D &camera) {
    Color color = { 255, 255, 255, 20 };
    if (editor.showGrid) {   
        int yOffset = ((int) camera.target.y) % GRID_DISTANCE;
        for (int i = 0; i < editor.windowHeight + yOffset; i += GRID_DISTANCE) {
            DrawLine(0, i - yOffset, editor.windowWidth, i- yOffset, color);
        }

        int xOffset = ((int) camera.target.x) % GRID_DISTANCE;
        for (int j = 0; j < editor.windowWidth + xOffset; j += GRID_DISTANCE) {
            DrawLine(j - xOffset, 0, j - xOffset, editor.windowHeight, color);
        }
    }
}

void updateEditor(Editor *editor) {
    const auto delta = GetFrameTime();
    for (int i = editor->messages.size() - 1; i >= 0; i--) {
        editor->messages[i].expiration -= delta;
        if (editor->messages[i].expiration < 0) {
            editor->messages.erase(editor->messages.begin() + i);
        }
    }
}

Color colorFromType(MessageType type) {
    switch (type) {
        case SUCCESS:
            return GREEN;
        case ERROR:
            return RED;
        case INFO:
            return WHITE;
        default:
            return WHITE;
    }
}

void drawMessages(Editor *editor) {
    int offsetY = 20;
    for (int i = editor->messages.size() - 1; i >= 0; i--) {
        auto c = colorFromType(editor->messages[i].type);
        DrawText(
            editor->messages[i].message.c_str(),
            20, 
            editor->windowHeight - 30 - offsetY,
            editor->fontSize,
            c
        );
        offsetY += 20;
    }
}

int main(int argc, char **argv) {
    LvlImporter lvlImport;
    JsonImporter jsonImport;
    
    JsonExporter jsonExport;
    CppExporter cppExport;
    LvlExporter lvlExport;
    SwiftExporter swiftExport;

    Editor editor = {};
    editor.version = 2;
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
    editor.showGrid = true;
    editor.showHelp = false;
    editor.showFPS = false;

    InitWindow((int) editor.windowWidth, (int) editor.windowHeight, "Level Editor");
    SetTargetFPS(60);

    // So ESCAPE isn't eaten by ShouldWindowClose();
    SetExitKey(KEY_NULL);

    Font fontDefault = LoadFontEx("assets/fonts/OverpassMono/OverpassMono-Regular.ttf", editor.fontSize, 0, 0);
    SetTextureFilter(fontDefault.texture, TEXTURE_FILTER_BILINEAR);
    GuiSetFont(fontDefault);

    map<string, Importer*> importers;
    importers["lvl"] = &lvlImport;
    importers["json"] = &jsonImport;

    vector<Exporter*> exporters = {
        &jsonExport,
        &lvlExport,
        &cppExport,
        &swiftExport
    };

    if (argc > 1) {
        loadLevel(argv[1], &editor, importers);
    } else {
        loadLevel("TestLevel1.lvl", &editor, importers);
    }

    Windows windows = {
        ExportWindow(&editor, exporters),
        ExitWindow(&editor, exporters[0]),
        BlockTypeEditorWindow(&editor),
        KeyValueEditorWindow(&editor)
    };

    Camera2D camera = {};
    camera.target = Vector2{editor.windowWidth/2.0f, editor.windowHeight/2.0f};
    camera.offset = Vector2{editor.windowWidth/2.0f, editor.windowHeight/2.0f};
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
            ClearBackground(DARKGRAY);
            drawGrid(editor, camera);
            BeginMode2D(camera);
                drawObjects(&camera, &editor);
            EndMode2D();
            drawHelp(&editor);
            drawWindows(&editor, &windows, exporters);
            if (editor.showFPS) DrawFPS(20, 20);
            drawMessages(&editor);
        EndDrawing();

        camera.zoom = 0;
        updateEditor(&editor);
    }

    CloseWindow();

    return 0;
}

