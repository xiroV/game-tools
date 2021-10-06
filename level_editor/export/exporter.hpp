#ifndef __EXPORTER__
#define __EXPORTER__

#include <string>
#include "../editor.hpp"

struct Exporter {
    virtual std::string name() = 0;
    virtual std::string extension() = 0;
    virtual std::string generate(Editor* editor) = 0;
};

#endif

