#pragma once
#include <QList>
#include <camera.h>
#include <map>
#include <tuple>
#include "block.h"
#include "perlinnoise.h"

typedef std::tuple<int, int, int> tuple;

class Scene
{
public:
    Scene();
    ~Scene();

    void Create();
    void clear();

    glm::ivec3 mMinXYZ;
    glm::ivec3 mMaxXYZ;
    std::map<tuple, Block*> mSceneMap;
    PerlinNoise mPerlinNoise;
};
