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
    std::map<tuple, Block *> GenerateBlocks(int direction);

    glm::ivec3 mMinXYZ;
    glm::ivec3 mMaxXYZ;
    std::map<tuple, Block*> mSceneMap;
    PerlinNoise mPerlinNoise;

    double mRefreshDistance; // when the distance from the camera to the edge is smaller than this value, generate new blocks
    int mNumRowNewBlocks; // number of rows of the new blocks

    void Hollow(double centerx, double centery, double centerz, double OutsideRadius, double InsideRadius, PerlinNoise &VoxelPerlinNoise);
    void GenerateWorm(double wormx, double wormy, double wormz);
};
