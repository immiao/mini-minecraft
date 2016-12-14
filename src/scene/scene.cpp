#include <scene/scene.h>

#include <scene/cube.h>
#include <time.h>

double distance(double a0, double a1, double a2, int b0, int b1, int b2)
{
    return sqrt((a0 - b0) * (a0 - b0) + (a1 - b1) * (a1 - b1) + (a2 - b2) * (a2 - b2));
}

Scene::Scene() : mMinXYZ(-150,-4, -150), mMaxXYZ(100, 3, 100), mPerlinNoise(0.5, 1.0, 1.0, 6, 100), mRefreshDistance(5), mNumRowNewBlocks(30)
{
    mIsWorm = false;
}

Scene::~Scene()
{
    clear();
}

void Scene::clear()
{
    std::map<tuple, Block*>::iterator iter;
    for (iter = mSceneMap.begin(); iter != mSceneMap.end(); iter++)
    {
        delete(iter->second);
    }
    mSceneMap.clear();
}

void Scene::Hollow(double centerx, double centery, double centerz, double OutsideRadius, double InsideRadius,
                   PerlinNoise &VoxelPerlinNoise, std::vector<Block*> &bottomBlock)
{
    int minx = centerx - OutsideRadius;
    int miny = centery - OutsideRadius;
    int minz = centerz - OutsideRadius;
    int maxx = centerx + OutsideRadius;
    int maxy = centery + OutsideRadius;
    int maxz = centerz + OutsideRadius;
    std::map<std::pair<int, int>, int> minHollowY;

    for (int i = minx; i <= maxx; i++)
        for (int j = miny; j <= maxy; j++)
        {
            if (j == mMinXYZ.y) continue; // do not hollow the BEDROCKs

            for (int k = minz; k <= maxz; k++)
            {
                tuple tempTuple(i, j, k);
                std::map<tuple, Block*>::iterator iter = mSceneMap.find(tempTuple);
                if (iter != mSceneMap.end())
                {

                    double dis = distance(centerx, centery, centerz, i, j, k);
                    if (dis < InsideRadius)
                    {
                        std::pair<int, int> p(i, k);
                        minHollowY[p] = j < minHollowY[p] ? j : minHollowY[p];
                        mSceneMap.erase(iter);

                        iter = New_map.find(tempTuple);
                        if (iter != New_map.end())
                            New_map.erase(iter);
                        //printf("%d %d %d\n", i, j, k);
                    }
                    else if (dis < OutsideRadius)
                    {
                        double r = (VoxelPerlinNoise.GetPerlinNoise(k, j, i) + 1) * 0.5f;
                        //printf("%f\n", r);
                        if (r > 0.50 && r < 0.52)
                        {

                            iter->second->mType = COAL;
                        }
                        else if (r > 0.45 && r < 0.47)
                        {
                            iter->second->mType = IRON_ORE;
                        }
                    }
                }
            }
        }
    std::map<std::pair<int, int>, int>::iterator iter;
    for (iter = minHollowY.begin(); iter != minHollowY.end(); iter++)
    {
        if (iter->second - 1 == mMinXYZ.y) continue; // ensure the block type at the bottom is BEDROCK
        tuple tempTuple(iter->first.first, iter->second - 1, iter->first.second);
        std::map<tuple, Block*>::iterator iter = mSceneMap.find(tempTuple);
        if (iter != mSceneMap.end())
            bottomBlock.push_back(iter->second);
    }
}

void Scene::GenerateWorm(double wormx, double wormy, double wormz, glm::ivec3 &scopeMin, glm::ivec3 &scopeMax)
{
    const int rr_beginSteps = 100;
    int steps = 0;
    srand(time(NULL));
    const double OutsideRadius = 5.0f;
    const double InsideRadius = 3.0f;
    const double stepScale = 15.0f;
    const double yScale = 5.0f;
    PerlinNoise WormPerlinNoise(0.5, 1.0, 1.0, 6, 0);
    PerlinNoise MinePerlinNoise(0.5, 1.0, 1.0, 4, 0);

    int miny = 2000;
    int maxy = -2000;
    std::vector<Block*> bottomBlock;
    while (1)
    {
        miny = wormy < miny ? wormy : miny;
        maxy = wormy > maxy ? wormy : maxy;

        Hollow(wormx, wormy, wormz, OutsideRadius, InsideRadius, MinePerlinNoise, bottomBlock);
        if (steps > rr_beginSteps)
        {
            if ((rand() % 101 / 100.f) > 0.7f)
                break;
        }
        double xx = wormx;
        double yy = wormy;
        double zz = wormz;

        wormx += WormPerlinNoise.GetPerlinNoise(yy, zz) * stepScale;
        wormy += WormPerlinNoise.GetPerlinNoise(xx, zz) * yScale;
       // printf("%f\n", WormPerlinNoise.GetPerlinNoise(xx, zz));
        wormz += WormPerlinNoise.GetPerlinNoise(xx, yy) * stepScale;
        if (wormx < scopeMin.x) wormx = scopeMax.x;
        if (wormx > scopeMax.x) wormx = scopeMin.x;
        if (wormy < scopeMin.y) break;
        if (wormz < scopeMin.z) wormz = scopeMax.z;
        if (wormz > scopeMax.z) wormz = scopeMin.z;
        steps++;
    }

    miny = miny - InsideRadius < scopeMin.y ? scopeMin.y + 1 : miny - InsideRadius;
    maxy = maxy + InsideRadius;
    double lavaHeight = miny + (maxy - miny) * 0.1f;

    int size = bottomBlock.size();
    for (int i = 0; i < size; i++)
    {
        if (bottomBlock[i]->mPosition.y < lavaHeight)
        {
            bottomBlock[i]->mType = LAVA;
            //printf("LAVA:%d\n", i);
        }
    }
}

void Scene::Create()
{
    PerlinNoise VoxelPerlinNoise(0.5, 1.0, 1.0, 6, 0);
    int centerx = (mMinXYZ.x + mMaxXYZ.x) * 0.5f;
    int centerz = (mMinXYZ.z + mMaxXYZ.z) * 0.5f;

    for (int i = mMinXYZ.x; i <= mMaxXYZ.x; i++)
    {
        for (int j = mMinXYZ.z; j <= mMaxXYZ.z; j++)
        {
            // BEDROCK at height of -128
            if (i >= centerx && j >= centerz)
            {
                Block* pBlock0 = new Block(glm::ivec3(i, mMinXYZ.y, j), BEDROCK);
                tuple tempTuple0(i, mMinXYZ.y, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));

                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y + 1; k < height; k++)
                {
                    BLOCK_TYPE type = DIRT;
                    double r = (VoxelPerlinNoise.GetPerlinNoise(k, j, i) + 1) * 0.5f;
                    if (k <= -16 && r > 0.44f && r < 0.55f) type = STONE;

                    Block* pBlock = new Block(glm::ivec3(i, k, j), type);
                    tuple tempTuple(i, k, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }

                // GRASS at the top
                Block* pBlock1 = new Block(glm::ivec3(i, height, j), GRASS);
                tuple tempTuple1(i, height, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
            }
            else if (i >= centerx && j < centerz)
            {
                Block* pBlock0 = new Block(glm::ivec3(i, mMinXYZ.y, j), BEDROCK);
                tuple tempTuple0(i, mMinXYZ.y, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));

                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y + 1; k <= height; k++)
                {
                    BLOCK_TYPE type = SAND;
                    Block* pBlock = new Block(glm::ivec3(i, k, j), type);
                    tuple tempTuple(i, k, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }

            }
            else if (i < centerx && j >= centerz)
            {

                Block* pBlock0 = new Block(glm::ivec3(i, mMinXYZ.y, j), BEDROCK);
                tuple tempTuple0(i, mMinXYZ.y, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));

                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y + 1; k < height; k++)
                {
                    BLOCK_TYPE type = DIRT;
                    double r = (VoxelPerlinNoise.GetPerlinNoise(k, j, i) + 1) * 0.5f;
                    if (k <= -16 && r > 0.44f && r < 0.55f) type = STONE;

                    Block* pBlock = new Block(glm::ivec3(i, k, j), type);
                    tuple tempTuple(i, k, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }

                // GRASS at the top
                if (height == 0)
                {
                    Block* pBlock1 = new Block(glm::ivec3(i, height, j), WATER);
                    tuple tempTuple1(i, height, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
                }
                else
                {
                    Block* pBlock1 = new Block(glm::ivec3(i, height, j), GRASS);
                    tuple tempTuple1(i, height, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
                }
            }
            else
            {
                Block* pBlock0 = new Block(glm::ivec3(i, mMinXYZ.y, j), BEDROCK);
                tuple tempTuple0(i, mMinXYZ.y, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));

                int height = mPerlinNoise.GetHeight(i, j);
                int snowHeight = rand() % 10 + 20;
                for (int k = mMinXYZ.y + 1; k < height; k++)
                {
                    BLOCK_TYPE type = DIRT;
                    double r = (VoxelPerlinNoise.GetPerlinNoise(k, j, i) + 1) * 0.5f;
                    if (k <= -16 && r > 0.44f && r < 0.55f) type = STONE;
                    else if (k >= snowHeight) type = SNOW;

                    Block* pBlock = new Block(glm::ivec3(i, k, j), type);
                    tuple tempTuple(i, k, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }

                // GRASS at the top
                if (height < snowHeight)
                {
                    Block* pBlock1 = new Block(glm::ivec3(i, height, j), GRASS);
                    tuple tempTuple1(i, height, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
                }
            }
        }
    }

    if (mIsWorm)
    {
        //GenerateWorm((mMinXYZ.x + mMaxXYZ.x) * 0.5f, 5.f, (mMinXYZ.z + mMaxXYZ.z) * 0.5f, mMinXYZ, mMaxXYZ);
        GenerateWorm((mMinXYZ.x + mMaxXYZ.x) * 0.5f, -32.f, (mMinXYZ.z + mMaxXYZ.z) * 0.5f, mMinXYZ, mMaxXYZ);
        GenerateWorm((mMinXYZ.x + mMaxXYZ.x) * 0.5f, -64.f, (mMinXYZ.z + mMaxXYZ.z) * 0.5f, mMinXYZ, mMaxXYZ);
        GenerateWorm((mMinXYZ.x + mMaxXYZ.x) * 0.5f, -96.f, (mMinXYZ.z + mMaxXYZ.z) * 0.5f, mMinXYZ, mMaxXYZ);
        GenerateWorm((mMinXYZ.x + mMaxXYZ.x) * 0.5f, -48.f, (mMinXYZ.z + mMaxXYZ.z) * 0.5f, mMinXYZ, mMaxXYZ);
        GenerateWorm((mMinXYZ.x + mMaxXYZ.x) * 0.5f, -16.f, (mMinXYZ.z + mMaxXYZ.z) * 0.5f, mMinXYZ, mMaxXYZ);
        GenerateWorm((mMinXYZ.x + mMaxXYZ.x) * 0.5f, -112.f, (mMinXYZ.z + mMaxXYZ.z) * 0.5f, mMinXYZ, mMaxXYZ);
//        GenerateWorm(mMinXYZ.x, 5.f, mMinXYZ.z);
//        GenerateWorm(mMinXYZ.x, 5.f, mMaxXYZ.z);
//        GenerateWorm(mMaxXYZ.x, 5.f, mMinXYZ.z);
//        GenerateWorm(mMaxXYZ.x, 5.f, mMaxXYZ.z);
    }
//    for (int i = 0; i < 10; i++)
//        printf("16:%f 15:%f 14:%f\n", mPerlinNoise.GetHeight(i, 16), mPerlinNoise.GetHeight(i, 15), mPerlinNoise.GetHeight(i, 14));
}

void Scene::GenerateBlocks(int direction)
{
    New_map.clear();
    PerlinNoise VoxelPerlinNoise(0.5, 1.0, 1.0, 6, 0);
    //printf("GENERATE\n");
    glm::ivec3 scopeMin = mMinXYZ;
    glm::ivec3 scopeMax = mMaxXYZ;

    if (direction == 0)
    {
        for (int i = mMinXYZ.x - 1; i > mMinXYZ.x - 1 - mNumRowNewBlocks; i--)
        {
            for (int j = mMinXYZ.z; j <= mMaxXYZ.z; j++)
            {
                // BEDROCK at height of -128
                Block* pBlock0 = new Block(glm::ivec3(i, mMinXYZ.y, j), BEDROCK);
                tuple tempTuple0(i, mMinXYZ.y, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));
                New_map.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));

                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y + 1; k < height; k++)
                {
                    BLOCK_TYPE type = DIRT;
                    double r = (VoxelPerlinNoise.GetPerlinNoise(k, j, i) + 1) * 0.5f;
                    if (k <= -16 && r > 0.44f && r < 0.55f) type = STONE;

                    Block* pBlock = new Block(glm::ivec3(i, k, j), type);
                    tuple tempTuple(i, k, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                    New_map.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }

                // GRASS at the top
                Block* pBlock1 = new Block(glm::ivec3(i, height, j), GRASS);
                tuple tempTuple1(i, height, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
                New_map.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
            }
        }
        scopeMax.x = mMaxXYZ.x;
        mMinXYZ.x = mMinXYZ.x - mNumRowNewBlocks;
        scopeMin.x = mMinXYZ.x;
        if (mIsWorm)
        {
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -32.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -64.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -96.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -48.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -16.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -112.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
        }
    }
    else if (direction == 1)
    {
        for (int i = mMaxXYZ.x + 1; i < mMaxXYZ.x + 1 + mNumRowNewBlocks; i++)
        {
            for (int j = mMinXYZ.z; j <= mMaxXYZ.z; j++)
            {
                // BEDROCK at height of -128
                Block* pBlock0 = new Block(glm::ivec3(i, mMinXYZ.y, j), BEDROCK);
                tuple tempTuple0(i, mMinXYZ.y, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));
                New_map.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));

                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y + 1; k < height; k++)
                {
                    BLOCK_TYPE type = DIRT;
                    double r = (VoxelPerlinNoise.GetPerlinNoise(k, j, i) + 1) * 0.5f;
                    if (k <= -16 && r > 0.44f && r < 0.55f) type = STONE;

                    Block* pBlock = new Block(glm::ivec3(i, k, j), type);
                    tuple tempTuple(i, k, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                    New_map.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }

                // GRASS at the top
                Block* pBlock1 = new Block(glm::ivec3(i, height, j), GRASS);
                tuple tempTuple1(i, height, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
                New_map.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
            }
        }

        scopeMin.x = mMaxXYZ.x;
        mMaxXYZ.x = mMaxXYZ.x + mNumRowNewBlocks;
        scopeMax.x = mMaxXYZ.x;
        if (mIsWorm)
        {
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -32.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -64.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -96.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -48.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -16.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -112.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
        }
    }
    else if (direction == 2)
    {
        for (int i = mMinXYZ.x; i <= mMaxXYZ.x; i++)
        {
            for (int j = mMinXYZ.z - 1; j > mMinXYZ.z - 1 - mNumRowNewBlocks; j--)
            {
                // BEDROCK at height of -128
                Block* pBlock0 = new Block(glm::ivec3(i, mMinXYZ.y, j), BEDROCK);
                tuple tempTuple0(i, mMinXYZ.y, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));
                New_map.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));

                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y + 1; k < height; k++)
                {
                    BLOCK_TYPE type = DIRT;
                    double r = (VoxelPerlinNoise.GetPerlinNoise(k, j, i) + 1) * 0.5f;
                    if (k <= -16 && r > 0.44f && r < 0.55f) type = STONE;

                    Block* pBlock = new Block(glm::ivec3(i, k, j), type);
                    tuple tempTuple(i, k, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                    New_map.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }

                // GRASS at the top
                Block* pBlock1 = new Block(glm::ivec3(i, height, j), GRASS);
                tuple tempTuple1(i, height, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
                New_map.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
            }
        }
        scopeMax.z = mMinXYZ.z;
        mMinXYZ.z = mMinXYZ.z - mNumRowNewBlocks;
        scopeMin.z = mMinXYZ.z;
        if (mIsWorm)
        {
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -32.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -64.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -96.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -48.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -16.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -112.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
        }
    }
    else
    {
        for (int i = mMinXYZ.x; i <= mMaxXYZ.x; i++)
        {
            for (int j = mMaxXYZ.z + 1; j < mMaxXYZ.z + 1 + mNumRowNewBlocks; j++)
            {
                // BEDROCK at height of -128
                Block* pBlock0 = new Block(glm::ivec3(i, mMinXYZ.y, j), BEDROCK);
                tuple tempTuple0(i, mMinXYZ.y, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));
                New_map.insert(std::pair<tuple, Block*>(tempTuple0, pBlock0));

                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y + 1; k < height; k++)
                {
                    BLOCK_TYPE type = DIRT;
                    double r = (VoxelPerlinNoise.GetPerlinNoise(k, j, i) + 1) * 0.5f;
                    if (k <= -16 && r > 0.44f && r < 0.55f) type = STONE;

                    Block* pBlock = new Block(glm::ivec3(i, k, j), type);
                    tuple tempTuple(i, k, j);
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                    New_map.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }

                // GRASS at the top
                Block* pBlock1 = new Block(glm::ivec3(i, height, j), GRASS);
                tuple tempTuple1(i, height, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
                New_map.insert(std::pair<tuple, Block*>(tempTuple1, pBlock1));
            }
        }
        scopeMin.z = mMaxXYZ.z;
        mMaxXYZ.z = mMaxXYZ.z + mNumRowNewBlocks;
        scopeMax.z = mMaxXYZ.z;

        if (mIsWorm)
        {
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -32.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -64.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -96.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -48.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -16.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
            GenerateWorm((scopeMin.x + scopeMax.x) * 0.5f, -112.f, (scopeMin.z + scopeMax.z) * 0.5f, scopeMin, scopeMax);
        }
    }
}

