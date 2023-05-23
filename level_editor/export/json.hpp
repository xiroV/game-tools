#ifndef __EXPORT_JSON__
#define __EXPORT_JSON__

#include "exporter.hpp"

struct JsonExporter : Exporter {
    JsonExporter() {
        setName("Json (default)");
        setExtension("json");
    }

    // create string with i spaces
    string s(int i) {
        return string(i, ' ');
    }

    string generate(Editor* editor) {
        string output;

        output += "{\n";
        output += s(2) + "\"version\": " + to_string(editor->version) + ",\n";
        output += s(2) + "\"objects\": [\n";

        for (unsigned int i = 0; i < editor->objects.size(); i++) {
            output += s(4) + "{\n";
            output += s(6) + "\"x\": " + to_string(editor->objects[i].x) + ",\n";
            output += s(6) + "\"y\": " + to_string(editor->objects[i].y) + ",\n";
            output += s(6) + "\"width\": " + to_string(editor->objects[i].width) + ",\n";
            output += s(6) + "\"height\": " + to_string(editor->objects[i].height) + ",\n";
            output += s(6) + "\"rotation\": " + to_string(editor->objects[i].rotation) + ",\n";
            output += s(6) + "\"type\": \"" + editor->objectTypes[editor->objects[i].type].name + "\",\n";

            if (editor->objects[i].data.size() < 1) {
                output += s(6) + "\"data\": []\n";
            } else {
                output += s(6) + "\"data\": [\n";
                
                for (unsigned int d = 0; d < editor->objects[i].data.size(); d++) {
                    ObjectData pair = editor->objects[i].data[d];
                    if (pair.key.length() != 0) {
                        output += s(8) + "\"" + pair.key + "\": \"" + pair.value + "\"";
                    }

                    output += d < editor->objects[i].data.size() - 1 ? ",\n" : "\n";
                }

                output += s(6) + "]\n";
            }

            output += s(4) + "}";

            output += i < editor->objects.size() - 1 ? ",\n" : "\n";
        }

        output += s(2) + "]\n";

        output += "}";

        return output;
    }

};

#endif

