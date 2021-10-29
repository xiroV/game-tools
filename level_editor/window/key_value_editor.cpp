#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui/src/raygui.h"
#endif

#include "../lib/raylib/src/raylib.h"
#include "../editor.hpp"
#include "../export/exporter.hpp"
#include <iostream>
#include "common.hpp"

struct KeyValueEditorWindow {
    Editor *editor;
    int editIndex = -1;
    vector<char*> keys;
    vector<char*> values;
    bool loaded = false;
    WindowFunctions func;

    KeyValueEditorWindow(Editor *editor) {
        this->editor = editor;
        this->func = WindowFunctions();
    }

    void control() {
        if (IsKeyPressed(KEY_N) && editIndex < 0) {
            editor->objects[editor->selectedObject].data.push_back({"", ""});
            reload();
        } 

        if (IsKeyPressed(KEY_DELETE)) {
            if (
                editIndex >= 0 ||
                editIndex < (int) editor->objects[editor->selectedObject].data.size()
            ) {
                editor->objects[editor->selectedObject].data.erase(editor->objects[editor->selectedObject].data.begin() + editIndex);
                editIndex = -1;
            }
            reload();
        } 

        if (IsKeyPressed(KEY_ESCAPE)) {
            if (editIndex < 0) {
                editIndex = -1;
                editor->state = EditorState::Editing;
            } else {
                editIndex = -1;
            }
            save();
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

        for (auto &val : values) {
            func.replaceIllegalExportChars(val);
        }

        for (auto &key : keys) {
            func.replaceIllegalExportChars(key);
        }
    }

    void draw() {
        if (editor->selectedObject == -1 || editor->state != EditorState::KeyValueEditor) return;

        if(GuiWindowBox({100, 100, (float) editor->windowWidth - 200, (float) editor->windowHeight - 200}, "Key/Value Editor")) {
            save();
            editor->state = EditorState::Editing; 
        }
        
        GuiDrawText("[n] for new key/value pair, [del] to delete", {(float) editor->windowWidth - 550, 100, 200, 100}, 0, BLACK);
        //Object &element = editor->objects[editor->selectedObject];


        int offsetY = 200;
        int currentIndex = 0;
        bool editing;

        for (auto &key: keys) {
            editing = false;

            if (currentIndex == editIndex) {
                editing = true;
            }
        
            if (GuiTextBox(
                {130, (float) offsetY, (float) editor->windowWidth / 2 - 130, (float) editor->fontSize*2},
                keys[currentIndex],
                64,
                editing && editor->keyOrValue == KeyOrValue::Key
            )) {
                editIndex = currentIndex;
                editor->keyOrValue = KeyOrValue::Key;
            }

            if (GuiTextBox(
                {editor->windowWidth / 2 + 30, (float) offsetY, editor->windowWidth / 2 - 160, (float) editor->fontSize*2},
                values[currentIndex],
                64,
                editing && editor->keyOrValue == KeyOrValue::Value
            )) {
                editIndex = currentIndex;
                editor->keyOrValue = KeyOrValue::Value;
            }

            offsetY += editor->fontSize * 2 + 5;
            currentIndex++;
        }
    }

    private:
        void save() {
            loaded = false;
            for (unsigned int i = 0; i < keys.size(); i++) {
                editor->objects[editor->selectedObject].data[i].key = string { keys[i] };
                editor->objects[editor->selectedObject].data[i].value = string { values[i] };
            }

        }

        void reload() {
            keys = {};
            values = {};
            for (auto const &pair : editor->objects[editor->selectedObject].data) {
                keys.push_back((char*)pair.key.c_str());
                values.push_back((char*)pair.value.c_str());
            }
            loaded = true;
        }

};

