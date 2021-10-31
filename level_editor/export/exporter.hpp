#ifndef __EXPORTER__
#define __EXPORTER__

#include <string>
#include "../editor.hpp"
#include <fstream>
#include <iostream>

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

    void saveLevel(Editor *editor) {
        string filename = editor->levelName + "." + getExtension();
        ofstream levelFile;
        levelFile.open(filename, ios::out);

        if (levelFile.is_open()) {
            levelFile << generate(editor);
        } else {
            std::cout << "Unable to open file " << filename << std::endl;
        }

        levelFile.close();
    }

    private:
        std::string name;
        std::string extension;
};



#endif

