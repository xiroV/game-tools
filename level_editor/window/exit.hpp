#ifndef __WINDOW_EXIT__
#define __WINDOW_EXIT__

#include "../lib/raylib/src/raylib.h"
#include "../editor.hpp"
#include "../export/exporter.hpp"
#include <iostream>

struct ExitWindow {
    Editor *editor;
    Exporter* exporter;
    int exitWindowSelectedOption = 0;
    char levelName[64] = "";

    ExitWindow(Editor *editor, Exporter *exporter) {
        this->editor = editor;
        this->exporter= exporter;
    }

    void control() {
        if (IsKeyPressed(KEY_ESCAPE)) {
            editor->state = EditorState::Editing;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (exitWindowSelectedOption == 0) {
                editor->levelName =  levelName;
                if (!editor->levelName.empty()) {
                    exporter->saveLevel(editor);
                    editor->closeEditor = true;
                }
            } else if(exitWindowSelectedOption == 1) {
                editor->closeEditor = true;
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

        /*updateStringByCharInput(editor->levelName, 60, illegalPathCharacters);
        
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
        }*/
    }

    void draw() {
        int scale = editor->fontSize/10;
        int yBase = 120+editor->fontSize;

        if(GuiWindowBox({100, 100, editor->windowWidth - 200, editor->windowHeight - 200}, "Exit Level Editor")) {
            editor->state = EditorState::Editing; 
        }

        GuiLabel({120, (float) yBase, 500, (float) editor->fontSize*2}, "Level name");

        if (editor->levelnameError) { GuiSetState(GUI_STATE_DISABLED); } else {GuiSetState(GUI_STATE_NORMAL); }
        GuiTextBox(
            {120, (float)yBase+editor->fontSize*2, editor->windowWidth-240, (float) editor->fontSize*2},
            levelName,
            64,
            true
        );
        GuiSetState(GUI_STATE_NORMAL);

        if(exitWindowSelectedOption == 0) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({120, (float) yBase+editor->fontSize*5, (float) 75*scale, (float) editor->fontSize*2}, "Save & Exit")) {
            editor->levelName = levelName;
            if (!editor->levelName.empty()) {
                exporter->saveLevel(editor);
                editor->closeEditor = true;
            }        
        }
        if(exitWindowSelectedOption == 0) GuiSetState(GUI_STATE_NORMAL);

        if(exitWindowSelectedOption == 1) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({(float) 120 + 75 * scale + editor->fontSize, (float) yBase+editor->fontSize*5, (float) 120*scale, (float) editor->fontSize*2}, "Close without saving")) {
            editor->closeEditor = true;
        }
        if(exitWindowSelectedOption == 1) GuiSetState(GUI_STATE_NORMAL);

        if(exitWindowSelectedOption == 2) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({(float) 120 + 75 * scale + editor->fontSize + 120 * scale + editor->fontSize, (float) yBase + editor->fontSize * 5, (float) 60 * scale, (float) editor->fontSize * 2}, "Cancel")) {
            editor->state = EditorState::Editing; 
        }
        if(exitWindowSelectedOption == 2) GuiSetState(GUI_STATE_NORMAL);
    }
};

#endif
