#ifndef __EDITOR__
#define __EDITOR__

#include <vector>
#include <string>
#include "lib/raylib/src/raylib.h"

using namespace std;

enum MessageType {
    ERROR,
    INFO,
    SUCCESS
};

struct EditorMessage {
    std::string message;
    float expiration;
    MessageType type;
};

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
    int x, y, width, height, rotation;
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
    string levelName;
    vector<EditorMessage> messages;
    vector<ObjectType> objectTypes = {
        {"Block", RED},
        {"Spawn", BLUE}
    };
    int selectedExporter;
    float windowWidth;
    float windowHeight;
    int fontSize;
    Vector2 cameraTarget;
    bool closeEditor;
    bool showGrid;
    bool showHelp;
};

#endif
