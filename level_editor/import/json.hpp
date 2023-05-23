#ifndef __IMPORT_JSON__
#define __IMPORT_JSON__

#include "importer.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>
#include <stdio.h>
#include "../lib/json/single_include/nlohmann/json.hpp"

struct JsonImporter : Importer {
    using json = nlohmann::json;

    JsonImporter() {}

    void consume(ifstream *file, Editor *editor) {
        string line;
        string content;
        json jsonObj;

        // This step shouldn't be necessary, but couldn't make it work otherwise
        while (getline(*file, line)) {
            content += line;
        }  

        try {
            jsonObj = json::parse(content);

        } catch (nlohmann::json::parse_error& e) {
            printf("Error parsing json: %s\n", e.what());
            editor->addMessage("Error parsing level file", 5, ERROR);
            return;
        }

        for (auto& item : jsonObj["objects"]) {
            Object obj = {0, 0, 0, 0, 0, 0, vector<ObjectData>()};

            item.at("x").get_to(obj.x);
            item.at("y").get_to(obj.y);
            item.at("width").get_to(obj.width);
            item.at("height").get_to(obj.height);
            item.at("rotation").get_to(obj.rotation);

            string typeName = item["type"];
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

            obj.type = typeId;

            for (auto& keyValue : item["data"].items()) {
                obj.data.push_back({keyValue.key(), keyValue.value()});
            }

            editor->objects.push_back(obj);
        }
    }
};

#endif

