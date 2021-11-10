#ifndef __EXPORT_SWIFT__
#define __EXPORT_SWIFT__

#include "exporter.hpp"
#include <sstream>

struct SwiftExporter : Exporter {
    SwiftExporter() {
        setName("Swift");
        setExtension("swift");
    }

    std::string generate(Editor* editor) {
        stringstream output;

        output << "//version " << editor->version << endl;
        output << endl;

        output << "class " << editor->levelName <<  " : LevelData {"<< endl
            << "    let objects: Array<LevelObject>" << endl;
            output << endl;

            output << "    init() {" << endl;
            output << "        self.objects = [" << endl;

            for (Object const& obj : editor->objects) {
                output << "            LevelObject("
                    << obj.x << ", "
                    << obj.y << ", "
                    << obj.width << ", "
                    << obj.height << ", "
                    << obj.rotation << ", "
                    << "\"" << editor->objectTypes[obj.type].name << "\", "
                    << "[";
                for (ObjectData const& keyValuePair : obj.data) {
                    output << "ObjectData(\"" << keyValuePair.key << "\", \"" << keyValuePair.value << "\"),";
                }
                output << "]";
                output << ")," << endl;
            }
            output << "        ]" << endl
            << "    }" << endl << endl;

            output << "    func getObjects() -> Array<LevelObject> {" << endl
            << "        return objects" << endl
            << "    }" << endl
            << "}" << endl
        << endl;

        return output.str();
    }

};

#endif

