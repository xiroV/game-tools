#ifndef __EXPORT_CPP__
#define __EXPORT_CPP__

#include "exporter.hpp"
#include <sstream>

struct CppExporter : Exporter {
    CppExporter() {
        setName("C++");
        setExtension("hpp");
    }

    std::string generate(Editor* editor) {
        stringstream output;

        output << "//version " << editor->version << endl;
        output << endl;
        output << "#include <string>" << endl;
        output << "#include <vector>" << endl;
        output << "#include \"level_data.hpp\"" << endl;
        output << endl;

        output << "struct " << editor->levelName <<  " : LevelData {"<< endl
            << "    std::vector<LevelObject> objects;" << endl;

            output << "    void init() override {" << endl;
            output << "        objects = {" << endl;

            for (Object const& obj : editor->objects) {
                output << "            {"
                    << obj.x << ", "
                    << obj.y << ", "
                    << obj.width << ", "
                    << obj.height << ", "
                    << "\"" << editor->objectTypes[obj.type].name << "\","
                    << "{";
                for (ObjectData const& keyValuePair : obj.data) {
                    output << "{\"" << keyValuePair.key << "\", \"" << keyValuePair.value << "\"},";
                }
                output << "}";
                output << "}," << endl;
            }
            output << "        };" << endl
            << "    };" << endl << endl;

            output << "    std::vector<LevelObject> &getObjects() override {" << endl
            << "        return objects;" << endl
            << "    }" << endl
            << "};" << endl
        << endl;

        return output.str();
    }

};

#endif

