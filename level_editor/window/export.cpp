#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui/src/raygui.h"
#endif

#include "../lib/raylib/src/raylib.h"
#include "../editor.hpp"
#include "../export/exporter.hpp"
#include <iostream>
#include "common.hpp"

struct ExportWindow {
    Editor *editor;
    vector<Exporter*> exporters;
    int exportWindowSelectedOption = 0;
    char levelName[64] = "";

    vector<char> illegalPathCharacters;
    vector<char> dotList;
    vector<string> illegalFileNames;

    ExportWindow(Editor *editor, vector<Exporter*> exporters) {
        this->editor = editor;
        this->exporters = exporters;
        this->illegalFileNames = {
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
            "lpt5",
            "lpt6",
            "lpt7",
            "lpt8",
            "lpt9",
            "con",
            "nul",
            "prn"
        };
        this->illegalPathCharacters = {
            '!', '"', '#', '%', '&', '\'', '(', ')', '*', '+', ',', '/', ':', ';', '<', '=', '>', '?', '[', '\\', ']', '^', '`', '{', '|', '}', 0
        };
        this->dotList = {'.', 0};
    }

    void control() {
        if (IsKeyPressed(KEY_ESCAPE)) {
            editor->state = EditorState::Editing;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (exportWindowSelectedOption == 0) {
                editor->levelName = levelName;
                if (!editor->levelName.empty()) {
                    exporters[editor->selectedExporter]->saveLevel(editor);
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
    }

    void draw() {
        int yBase = 120+editor->fontSize;
        int scale = editor->fontSize/10;

        if(GuiWindowBox({100, 100, (float) editor->windowWidth - 200, (float) editor->windowHeight - 200}, "Export Level")) {
            editor->state = EditorState::Editing; 
        }

        GuiLabel({120, (float) yBase, 500, (float) editor->fontSize*2}, "Level name");

        if (editor->levelnameError) { GuiSetState(GUI_STATE_DISABLED); } else {GuiSetState(GUI_STATE_NORMAL); }
        GuiTextBox(
            {120, (float)yBase+editor->fontSize*2, (float) editor->windowWidth-240, (float) editor->fontSize*2},
            levelName,
            12,
            true
        );
        GuiSetState(GUI_STATE_NORMAL);

        GuiLabel({120, (float) yBase + editor->fontSize * 4, 500, (float) editor->fontSize * 2}, "Export type");

        string exportOptions;
        for (auto &exporter : exporters) {
            exportOptions.append(exporter->getName());
            exportOptions.push_back(';');
        }
        exportOptions.pop_back();

        editor->selectedExporter = GuiComboBox({120, (float) yBase+editor->fontSize*6, (float) editor->windowWidth-240, (float) editor->fontSize*2}, const_cast<char*>(exportOptions.c_str()), editor->selectedExporter);

        if(exportWindowSelectedOption == 0) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({120, (float) yBase+editor->fontSize*9, (float) 75*scale, (float) editor->fontSize*2}, "Export")) {
            editor->levelName = levelName;
            if (!editor->levelName.empty()) {
                exporters[editor->selectedExporter]->saveLevel(editor);
            }
        }
        if(exportWindowSelectedOption == 0) GuiSetState(GUI_STATE_NORMAL);

        if(exportWindowSelectedOption == 1) GuiSetState(GUI_STATE_FOCUSED);
        if (GuiButton({(float) 120 + 75 * scale + editor->fontSize, (float) yBase+editor->fontSize*9, (float) 60*scale, (float) editor->fontSize*2}, "Cancel")) {
            editor->state = EditorState::Editing;
        }
        if(exportWindowSelectedOption == 1) GuiSetState(GUI_STATE_NORMAL);
    }
};

