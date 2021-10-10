#ifndef __EXPORT_LVL__
#define __EXPORT_LVL__

#include "exporter.hpp"
#include <sstream>

struct LvlExporter : Exporter {
    LvlExporter() {
        setName("Lvl (default)");
        setExtension("lvl");
    }

    std::string generate(Editor* editor) {
        std::stringstream output;

        output << "#version " << editor->version << endl;
        for (unsigned int i = 0; i < editor->objects.size(); i++) {
            output << editor->objects[i].x << editor->outputDelimiter << editor->objects[i].y <<
                editor->outputDelimiter << editor->objects[i].width << editor->outputDelimiter << editor->objects[i].height << editor->outputDelimiter << editor->objectTypes[editor->objects[i].type].name << editor->outputDelimiter;
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

