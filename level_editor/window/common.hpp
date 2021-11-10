#ifndef __WINDOW_COMMON__
#define __WINDOW_COMMON__

#include <string>
#include <vector>
#include <cstring>

struct WindowFunctions {
    std::vector<char> illegalPathCharacters;
    std::vector<char> illegalExportCharacters;
    std::vector<char> dotList;
    std::vector<std::string> illegalFileNames;

    WindowFunctions() {
        this->illegalFileNames = {
            ".",
            "..",
            "aux",
            "com1",
            "com2",
            "com3",
            "com4",
            "com5",
            "com6",
            "com7",
            "com8",
            "com9",
            "lpt1",
            "lpt2",
            "lpt3",
            "lpt5",
            "lpt6",
            "lpt7",
            "lpt8",
            "lpt9",
            "con",
            "nul",
            "prn"
        };
        this->dotList = {'.', 0};
        this->illegalPathCharacters = {'!', '"', '#', '%', '&', '\'', '(', ')', '*', '+', ',', '/', ':', ';', '<', '=', '>', '?', '[', '\\', ']', '^', '`', '{', '|', '}', 0};
        this->illegalExportCharacters = {'"', '=', ';', 0};
    }


    std::string toLowerCase(std::string str) {
        std::string result = "";
        int i = 0;
        while (str[i] != '\0') {
            result += std::tolower(str[i]);
            i++;
        }
        return result;
    }

    bool anyMatch(char key, std::vector<char> illegalChars) {
        // Currently REQUIRES final entry in array is 0.
        for (int i = 0; illegalChars[i] != 0; i++) {
            if (illegalChars[i] == key) return true;
        }
        return false;
    }

    void replaceIllegalPathChars(char* str) {
        const int strSize = strlen(str);
        for (int i = strSize; i >= 0; i--) {
            if (anyMatch(str[i], illegalPathCharacters)) {
                str[i] = '\0';
                break;
            }
        }
    }

    void replaceIllegalExportChars(char* str) {
        const int strSize = strlen(str);
        for (unsigned int i = strSize; i >= 0; i--) {
            if (anyMatch(str[i], illegalExportCharacters)) {
                str[i] = '\0';
                break;
            }
        }
    }

    bool isIllegalName(char* str) {
        for (unsigned int i = 0; i < illegalFileNames.size(); i++) {
            std::string lower = toLowerCase(str);
            if (str == illegalFileNames[i]) {
                return true;
            }
        }
        return false;
    }

};

#endif
