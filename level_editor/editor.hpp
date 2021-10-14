#ifndef __EDITOR__
#define __EDITOR__

#include <vector>
#include <string>
#include "lib/raylib/src/raylib.h"

using namespace std;

struct ObjectType {
    string name;
    Color color;
};

enum class KeyOrValue {
    Key,
    Value
};

enum class ObjectTypeParameter {
    Name,
    Color 
};

struct ObjectData {
    string key;
    string value;
};

struct Object {
    int x, y, width, height;
    int type;
    vector<ObjectData> data;
};

enum class EditorState {
    Editing,
    Closing,
    KeyValueEditor,
    BlockTypeEditor,
    Export
};

struct Editor {
    EditorState state;
    vector<Object> objects; 
    int selectedObject;
    int editKeyValueIndex;
    KeyOrValue keyOrValue;
    ObjectTypeParameter objectTypeParameter;
    int editBlockTypeIndex;
    char outputDelimiter;
    int version;
    bool levelnameError;
    Font defaultFont;
    string levelName;
    vector<ObjectType> objectTypes = {
        {"Block", RED},
        {"Spawn", BLUE}
    };

    void drawText(string text, Vector2 position, Color color = BLACK);
};

#endif
