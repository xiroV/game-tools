#ifndef __LEVEL_DATA__
#define __LEVEL_DATA__

#include <string>
#include <vector>

#ifndef __LEVEL_OBJECT__
#define __LEVEL_OBJECT__
struct LevelObject {
    int x, y, width, height;
    std::string type;
};
#endif

struct LevelData {
    virtual void init() = 0;
    virtual std::vector<LevelObject> &getObjects() = 0;
    virtual ~LevelData() = default;
};

#endif
