#ifndef __EXPORTER__
#define __EXPORTER__

#include <string>
#include "../editor.hpp"

struct Exporter {
    virtual std::string generate(Editor* editor) = 0;
    virtual ~Exporter() = default;

    void setName(std::string name) {
        this->name = name;
    };

    std::string getName() {
        return name;
    };

    void setExtension(std::string extension) {
        this->extension = extension;
    };

    std::string getExtension() {
        return extension;
    };

    private:
        std::string name;
        std::string extension;
};

#endif

