#ifndef __EXPORT_JSON__
#define __EXPORT_JSON__

#include "exporter.hpp"
#include "../lib/json/single_include/nlohmann/json.hpp"

struct JsonExporter : Exporter {
    using json = nlohmann::json;

    JsonExporter() {
        setName("Json (default)");
        setExtension("json");
    }

    string generate(Editor* editor) {
        json output;

        output["version"] = editor->version;
        vector<json> objects;

        for (unsigned int i = 0; i < editor->objects.size(); i++) {
            json object;
            object["x"] = editor->objects[i].x;
            object["y"] = editor->objects[i].y;
            object["width"] = editor->objects[i].width;
            object["height"] = editor->objects[i].height;
            object["rotation"] = editor->objects[i].rotation;
            object["type"] = editor->objectTypes[editor->objects[i].type].name;

            json objectData;
            for (unsigned int d = 0; d < editor->objects[i].data.size(); d++) {
                ObjectData p = editor->objects[i].data[d];
                objectData[p.key] = p.value;
            }
            object["data"] = objectData;

            objects.push_back(object);
        }

        output["objects"] = objects;

        return output.dump(2);
    }

};

#endif

