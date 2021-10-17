#ifndef __WINDOW_BLOCK_TYPE_EDITOR__
#define __WINDOW_BLOCK_TYPE_EDITOR__

#include "../lib/raylib/src/raylib.h"
#include "../editor.hpp"
#include "../export/exporter.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <stdio.h>

struct BlockTypeEditorWindow {
    Editor *editor;
    int editIndex = -1;
    vector<char*> typeNames;
    bool loaded = false;

    BlockTypeEditorWindow(Editor *editor) {
        this->editor = editor;
    }

    void control() {
        if (!loaded) {
            reload();
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            save();
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
            reload();
        }

        if (IsKeyPressed(KEY_DELETE)) {
            if (editor->objectTypes.size() > 1) {
                editor->objectTypes.erase(editor->objectTypes.begin() + editor->editBlockTypeIndex);
                editor->editBlockTypeIndex = 0;
            }
        }

        ObjectType &current = editor->objectTypes[editor->editBlockTypeIndex];
        if (!editor->objectTypes.empty() && editor->editBlockTypeIndex >= 0) {
            //updateStringByCharInput(current.name, 30, illegalPathCharacters);

            if (IsKeyPressed(KEY_END)) {
                current.color = (Color) {
                    static_cast<unsigned char>(GetRandomValue(0,255)),
                    static_cast<unsigned char>(GetRandomValue(0, 255)),
                    static_cast<unsigned char>(GetRandomValue(0,255)),
                    255
                }; 
            }
        }
    }

    void draw() {
        if (editor->state != EditorState::BlockTypeEditor) return;

        if (GuiWindowBox(
            {100, 100, editor->windowWidth - 200, editor->windowHeight - 200},
            "Block Type Editor"
        )) {
            editor->state = EditorState::Editing; 
        }
        
        GuiDrawText("[n] for new block type, [del] to delete", {editor->windowWidth - 520, 100, 200, 100}, 0, BLACK);

        int offsetY = 200;
        int currentIndex = 0;
        bool editing;

        for (auto &name: typeNames) {
            editing = false;

            if (currentIndex == editor->editBlockTypeIndex) {
                editing = true;
            }
        
            if (GuiTextBox(
                {130, (float) offsetY, editor->windowWidth / 2 - 130, (float) editor->fontSize*2},
                name,
                64,
                editing && editor->keyOrValue == KeyOrValue::Key
            )) {
                editor->editBlockTypeIndex = currentIndex;
            }

            DrawRectangle(15 + editor->windowWidth / 2, offsetY, editor->fontSize*2, editor->fontSize*2, editor->objectTypes[currentIndex].color);

            if (editing) {
                GuiDrawText("[end] change color", {10 + editor->windowWidth / 2 + 70, (float) offsetY, editor->windowWidth / 2 - 130, (float) editor->fontSize * 2}, 0, BLACK);
            }

            offsetY += editor->fontSize * 2 + 5;
            currentIndex++;
        }
    }

    private:
        void save() {
            int i = 0;
            for (auto const &typeName : typeNames) {
                strcpy((char*)editor->objectTypes[i].name.c_str(), typeName);
                i++;
            }
        }

        void reload() {
            typeNames = {};
            for (auto &objectType : editor->objectTypes) {
                typeNames.push_back((char*)objectType.name.c_str());
            }
            loaded = true;
        }
};

#endif
