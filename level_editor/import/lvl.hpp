#ifndef __IMPORT_LVL__
#define __IMPORT_LVL__

#include "importer.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

struct LvlImporter : Importer {
    LvlImporter() {}

    void consume(ifstream *file, Editor *editor) {
        string fileLine;
        unsigned int version = 0;

        // Read version
        getline(*file, fileLine);
        if (fileLine[0] == '#') {
            version = stoi(fileLine.substr(string("#version ").length(), fileLine.length()));
        };

        if (version > editor->version) {
            editor->addMessage("Version of file read is newer than this binary supports. Will try its best to parse the input file.", 5, INFO);
        }


        // Parse
        while (getline(*file, fileLine)) {            
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

            if (typeId < 0) {
                Color color = (Color) {
                    static_cast<unsigned char>(GetRandomValue(0,255)),
                    static_cast<unsigned char>(GetRandomValue(0, 255)),
                    static_cast<unsigned char>(GetRandomValue(0,255)),
                    255
                }; 

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
    }

};

#endif

