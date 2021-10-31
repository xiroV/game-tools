#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui/src/raygui.h"
#endif

#include "../lib/raylib/src/raylib.h"
#include "../editor.hpp"
#include "../export/exporter.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <stdio.h>
#include "common.hpp"

struct BlockTypeEditorWindow {
    Editor *editor;
    int editIndex = -1;
    vector<char*> typeNames;
    bool loaded = false;
    WindowFunctions func;

    BlockTypeEditorWindow(Editor *editor) {
        this->editor = editor;
        this->func = WindowFunctions();
    }

    void control() {
        if (!loaded) {
            reload();
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            save();
            if (editIndex < 0) {
                editIndex = -1;
                editor->state = EditorState::Editing;
            } else {
                editIndex = -1;
            }
        }

        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_TAB)) {
            ++editIndex %= editor->objectTypes.size();
        }

        if (IsKeyPressed(KEY_UP)) {
            --editIndex %= editor->objectTypes.size();
        }

        if (IsKeyPressed(KEY_N) && editIndex < 0) {
            editor->objectTypes.push_back({"", BLUE});
            reload();
        }

        if (IsKeyPressed(KEY_DELETE)) {
            if (editor->objectTypes.size() > 1) {
                editor->objectTypes.erase(editor->objectTypes.begin() + editIndex);
                editIndex = 0;
            }
            reload(); 
        }

        ObjectType &current = editor->objectTypes[editIndex];
        if (!editor->objectTypes.empty() && editIndex >= 0) {
            if (IsKeyPressed(KEY_END)) {
                current.color = (Color) {
                    static_cast<unsigned char>(GetRandomValue(0,255)),
                    static_cast<unsigned char>(GetRandomValue(0, 255)),
                    static_cast<unsigned char>(GetRandomValue(0,255)),
                    255
                }; 
            }
        }

        for (auto &name : typeNames) {
            func.replaceIllegalExportChars(name);
        }
    }

    void draw() {
        if (editor->state != EditorState::BlockTypeEditor) return;

        if (GuiWindowBox(
            {100, 100, editor->windowWidth - 200, editor->windowHeight - 200},
            "Block Type Editor"
        )) {
            save();
            editor->state = EditorState::Editing; 
        }
        
        GuiDrawText("[n] for new block type, [del] to delete", {editor->windowWidth - 520, 100, 200, 100}, 0, BLACK);

        int offsetY = 200;
        int currentIndex = 0;
        bool editing;

        for (auto &name : typeNames) {
            editing = false;

            if (currentIndex == editIndex) {
                editing = true;
            }
        
            if (GuiTextBox(
                {130, (float) offsetY, editor->windowWidth / 2 - 130, (float) editor->fontSize*2},
                name,
                64,
                editing && editor->keyOrValue == KeyOrValue::Key
            )) {
                editIndex = currentIndex;
            }

            DrawRectangle(15 + editor->windowWidth / 2, offsetY, editor->fontSize*2, editor->fontSize*2, editor->objectTypes[currentIndex].color);
            Color c = editor->objectTypes[currentIndex].color;
            editor->objectTypes[currentIndex].color = GuiColorPicker(Rectangle{15 + editor->windowWidth / 2 + editor->fontSize*2 + 10, (float) offsetY, editor->fontSize*2.0f, editor->fontSize*2.0f}, c);

            if (editing) {
                GuiDrawText("[end] random color", {10 + editor->windowWidth / 2 + 150, (float) offsetY, editor->windowWidth / 2 - 130, (float) editor->fontSize * 2}, 0, BLACK);
            }

            offsetY += editor->fontSize * 2 + 5;
            currentIndex++;
        }
    }

    private:
        void save() {
            loaded = false;
            int i = 0;
            for (auto const &typeName : typeNames) {
                editor->objectTypes[i].name = string { typeName };
                i++;
            }
        }

        void reload() {
            typeNames = {};
            for (auto const &objectType : editor->objectTypes) {
                typeNames.push_back((char*)objectType.name.c_str());
            }
            loaded = true;
        }
};
