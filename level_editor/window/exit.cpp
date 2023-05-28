#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui/src/raygui.h"
#endif

#include "../lib/raylib/src/raylib.h"
#include "../editor.hpp"
#include "../export/exporter.hpp"
#include <iostream>
#include "common.hpp"


struct ExitWindow {
    Editor *editor;
    Exporter* exporter;
    int exitWindowSelectedOption = 0;
    char levelName[64] = "";
    bool levelNameError = false;
    WindowFunctions func;

    ExitWindow(Editor *editor, Exporter *exporter) {
        this->editor = editor;
        this->exporter= exporter;
        this->func = WindowFunctions();
    }


    void control() {
        if (IsKeyPressed(KEY_ESCAPE)) {
            editor->state = EditorState::Editing;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (exitWindowSelectedOption == 0) {
                editor->levelName =  levelName;
                if (!editor->levelName.empty() && !levelNameError) {
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

        func.replaceIllegalPathChars(levelName);
        levelNameError = func.isIllegalName(levelName);
    }

    void draw() {
        int scale = editor->fontSize/10;
        int yBase = 120+editor->fontSize;

        if(GuiWindowBox({100, 100, editor->windowWidth - 200, editor->windowHeight - 200}, "Exit Level Editor")) {
            editor->state = EditorState::Editing; 
        }

        GuiLabel({120, (float) yBase, 500, (float) editor->fontSize*2}, "Level name");

        if (levelNameError) {
            GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED, ColorToInt(RED));
        } else {
            GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED, 0x0492c7ff);
        }

        GuiTextBox(
            {120, (float)yBase+editor->fontSize*2, editor->windowWidth-240, (float) editor->fontSize*2},
            levelName,
            64,
            true
        );
        
        GuiSetState(GUI_STATE_NORMAL);

        if(exitWindowSelectedOption == 0) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({(float) 120, (float) yBase+editor->fontSize*5, (float) editor->fontSize*8*scale, (float) editor->fontSize*2}, "Save & Exit")) {
            editor->levelName = levelName;
            if (!editor->levelName.empty() && !levelNameError) {
                exporter->saveLevel(editor);
                editor->closeEditor = true;
            }        
        }

        if(exitWindowSelectedOption == 0) GuiSetState(GUI_STATE_NORMAL);

        if(exitWindowSelectedOption == 1) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({(float) 120 + editor->fontSize*8*scale + editor->fontSize, (float) yBase+editor->fontSize*5, (float) editor->fontSize*12*scale, (float) editor->fontSize*2}, "Close without saving")) {
            editor->closeEditor = true;
        }
        if(exitWindowSelectedOption == 1) GuiSetState(GUI_STATE_NORMAL);

        if(exitWindowSelectedOption == 2) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({(float) 470, (float) yBase + editor->fontSize * 5, (float) 60 * scale, (float) editor->fontSize * 2}, "Cancel")) {
            editor->state = EditorState::Editing; 
        }
        if(exitWindowSelectedOption == 2) GuiSetState(GUI_STATE_NORMAL);
    }
};
