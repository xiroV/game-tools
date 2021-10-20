#ifndef __WINDOW_EXIT__
#define __WINDOW_EXIT__

#include "../lib/raylib/src/raylib.h"
#include "../editor.hpp"
#include "../export/exporter.hpp"
#include <iostream>

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

std::string toLowerCase(char *str) {
    std::string result = "";
    int i = 0;
    while (str[i] != '\0') {
        result += std::tolower(str[i]);
        i++;
    }
    return result;
}

bool anyMatch(char key, char illegalChars[]) {
    // Currently REQUIRES final entry in array is 0.
    for (int i = 0; illegalChars[i] != 0; i++) {
        if (illegalChars[i] == key) return true;
    }

    return false;
}

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
        
        // Handle illegal characters
        const int levelNameSize = sizeof(levelName) / sizeof((levelName)[0]);
        for (int i = levelNameSize; i >= 0; i--) {
            if (anyMatch(levelName[i], illegalPathCharacters)) {
                levelName[i] = '\0';
                break;
            }
        }

        // Handle illegal filenames
        const int fileNameCount = sizeof(illegalFileNames) / sizeof((illegalFileNames)[0]);
        bool illegalName = false;
        for (int i = 0; i < fileNameCount; i++) {
            std::string lower = toLowerCase(levelName);
            if (levelName == illegalFileNames[i]) {
                illegalName = true;
                break;
            }
        }

        if (illegalName) { std::cout << "illegal file name" << std::endl; }
        else { std::cout << "Legal" << std::endl; }
        
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
