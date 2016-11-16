#pragma once
#include <QList>
#include <camera.h>
#include <map>
#include <tuple>
#include "block.h"

typedef std::tuple<int, int, int> tuple;

class Scene
{
public:
    Scene();
    ~Scene();

    void Create();
    void clear();

    glm::ivec3 mDimensions;
    std::map<tuple, Block*> mSceneMap;
};
