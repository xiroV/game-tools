#ifndef __WINDOW_KEY_VALUE_EDITOR__
#define __WINDOW_KEY_VALUE_EDITOR__

#include "../lib/raylib/src/raylib.h"
#include "../editor.hpp"
#include "../export/exporter.hpp"
#include <iostream>

struct KeyValueEditorWindow {
    Editor *editor;
    int editIndex = -1;

    KeyValueEditorWindow(Editor *editor) {
        this->editor = editor;
    }

    void control() {
        if (IsKeyPressed(KEY_N) && editIndex < 0) {
            editor->objects[editor->selectedObject].data.push_back({"", ""});
        } 

        if (IsKeyPressed(KEY_DELETE)) {
            if (
                editIndex >= 0 ||
                editIndex < (int) editor->objects[editor->selectedObject].data.size()
            ) {
                editor->objects[editor->selectedObject].data.erase(editor->objects[editor->selectedObject].data.begin() + editIndex);
                editIndex = -1;
            }
        } 

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (editIndex < 0) {
                editIndex = -1;
                editor->state = EditorState::Editing;
            } else {
                editIndex = -1;
            }
        } 

        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_TAB)) {
            if (editIndex < 0) {
                editIndex += 1;
            } else {
                if (editor->keyOrValue == KeyOrValue::Key) {
                    editor->keyOrValue = KeyOrValue::Value;
                } else {
                    editor->keyOrValue = KeyOrValue::Key;
                    if (editIndex >= (int) editor->objects[editor->selectedObject].data.size() - 1) {
                        editIndex = -1;
                    } else {
                        editIndex += 1;
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


        if (editIndex >= 0) {
            // Ah, geez
            //ObjectData &current = editor->objects[editor->selectedObject].data[editIndex];
            //string &data = editor->keyOrValue == KeyOrValue::Key ? current.key : current.value; 
            // Ah, geez end
            //updateStringByCharInput(data, 30, illegalPathCharacters);
        }

    }

    void draw() {
        if (editor->selectedObject == -1 || editor->state != EditorState::KeyValueEditor) return;

        if(GuiWindowBox({100, 100, (float) editor->windowWidth - 200, (float) editor->windowHeight - 200}, "Key/Value Editor")) {
            editor->state = EditorState::Editing; 
        }
        
        GuiDrawText("[n] for new key/value pair, [del] to delete", {(float) editor->windowWidth - 550, 100, 200, 100}, 0, BLACK);
        Object &element = editor->objects[editor->selectedObject];


        int offsetY = 200;
        int currentIndex = 0;
        bool editing;

        for (auto &entry : element.data) {
            editing = false;

            if (currentIndex == editIndex) {
                editing = true;
            }
        
            if (GuiTextBox({130, (float) offsetY, (float) editor->windowWidth / 2 - 130, (float) editor->fontSize*2}, const_cast<char*>(entry.key.c_str()), 64, editing && editor->keyOrValue == KeyOrValue::Key)) {
                editIndex = currentIndex;
                editor->keyOrValue = KeyOrValue::Key;
            }

            if (GuiTextBox({editor->windowWidth / 2 + 30, (float) offsetY, editor->windowWidth / 2 - 160, (float) editor->fontSize*2}, const_cast<char*>(entry.value.c_str()), 64, editing && editor->keyOrValue == KeyOrValue::Value)) {
                editIndex = currentIndex;
                editor->keyOrValue = KeyOrValue::Value;
            }

            offsetY += editor->fontSize * 2 + 5;
            currentIndex++;
        }
    }
};

#endif
