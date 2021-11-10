#ifndef __LEVEL_DATA__
#define __LEVEL_DATA__

#include <string>
#include <vector>

#ifndef __LEVEL_OBJECT__
#define __LEVEL_OBJECT__
struct ObjectData {
    std::string key;
    std::string value;
};

struct LevelObject {
    int x, y, width, height, rotation;
    std::string type;
    std::vector<ObjectData> data;
};
#endif

struct LevelData {
    virtual void init() = 0;
    virtual std::vector<LevelObject> &getObjects() = 0;
    virtual ~LevelData() = default;
};

#endif
