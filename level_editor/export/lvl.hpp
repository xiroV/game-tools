#ifndef __EXPORT_LVL__
#define __EXPORT_LVL__

#include "exporter.hpp"
#include <sstream>

struct LvlExporter : Exporter {

    std::string name() {
        return "LVL (default)";
    }

    std::string extension() {
        return "lvl";
    }

    std::string generate(Editor* editor) {
        std::stringstream output;

        output << "#version " << editor->version << endl;
        for (unsigned int i = 0; i < editor->objects.size(); i++) {
            output << editor->objects[i].x << editor->outputDelimiter << editor->objects[i].y <<
                editor->outputDelimiter << editor->objects[i].width << editor->outputDelimiter << editor->objects[i].height << editor->outputDelimiter << editor->objects[i].type << editor->outputDelimiter;
                for (auto &pair : editor->objects[i].data) {
                    // Should also handle multiple empty spaces as invalid.
                    if (pair.key.length() != 0) {
                        output << pair.key << "=" << pair.value << editor->outputDelimiter;
                    }
                }

                output << endl;
        }

        return output.str();
    }

};

#endif

