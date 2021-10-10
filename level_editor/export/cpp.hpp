#ifndef __EXPORT_CPP__
#define __EXPORT_CPP__

#include "exporter.hpp"
#include <sstream>

struct CppExporter : Exporter {
    CppExporter() {
        setName("C++");
        setExtension("cpp");
    }

    std::string generate(Editor* editor) {
        stringstream output;

        output << "//version " << editor->version << endl;
        output << endl;
        output << "#include <string>" << endl;
        output << "#include <vector>" << endl;
        output << endl;

        output << "struct Object {" << endl
            << "    int x, y, width, height;" << endl
            << "    std::string type;" << endl
            << "};" << endl
        << endl;

        output << "struct " << editor->levelName <<  " {"<< endl
            << "    std::vector<Object> objects;" << endl;

            output << "    " << editor->levelName << "() {" << endl;
            output << "        objects = {" << endl;

            for (unsigned int i = 0; i < editor->objects.size(); i++) {
                output << "            {"
                    << editor->objects[i].x << ", "
                    << editor->objects[i].y << ", "
                    << editor->objects[i].width << ", "
                    << editor->objects[i].height << ", "
                    << "\"" << editor->objectTypes[editor->objects[i].type].name << "\""
                << "}," << endl;
            }
            output << "        };" << endl
            << "    };" << endl
            << "};" << endl
        << endl;

        return output.str();
    }

};

#endif

