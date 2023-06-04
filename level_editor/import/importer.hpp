#ifndef __IMPORTER__
#define __IMPORTER__

#include <string>
#include "../editor.hpp"
#include <fstream>
#include <iostream>

struct Importer {
    virtual void consume(ifstream* file, Editor* editor) = 0;
    virtual ~Importer() = default;
};

#endif

