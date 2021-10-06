#ifndef __EXPORT_CPP__
#define __EXPORT_CPP__

#include "exporter.hpp"

struct CppExporter : Exporter {

    std::string name() {
        return "C++";
    } 

    std::string extension() {
        return "cpp";
    }

    std::string generate(Editor* editor) {
        return "cpp output";
    }

};

#endif

