#include <scene/scene.h>

#include <scene/cube.h>

Scene::Scene() : mMinXYZ(-10, -20, -10), mMaxXYZ(50, 8, 50), mPerlinNoise(0.5, 1.0, 1.0, 6, 100), mRefreshDistance(0.1), mNumRowNewBlocks(30)
{

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

void Scene::Create()
{
    for (int i = mMinXYZ.x; i <= mMaxXYZ.x; i++)
    {
        for (int j = mMinXYZ.z; j <= mMaxXYZ.z; j++)
        {
            int height = mPerlinNoise.GetHeight(i, j);
            for (int k = mMinXYZ.y; k <= height; k++)
            {
                Block* pBlock = new Block(glm::ivec3(i, k, j));
                tuple tempTuple(i, k, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
            }
        }
    }
//    for (int i = 0; i < 10; i++)
//        printf("16:%f 15:%f 14:%f\n", mPerlinNoise.GetHeight(i, 16), mPerlinNoise.GetHeight(i, 15), mPerlinNoise.GetHeight(i, 14));
}

std::map<tuple, Block*> Scene::GenerateBlocks(int direction)
{
    //printf("GENERATE\n");
    std::map<tuple, Block*> New_map;
    if (direction == 0)
    {
        for (int i = mMinXYZ.x - 1; i > mMinXYZ.x - 1 - mNumRowNewBlocks; i--)
        {
            for (int j = mMinXYZ.z; j <= mMaxXYZ.z; j++)
            {
                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y; k <= height; k++)
                {
                    Block* pBlock = new Block(glm::ivec3(i, k, j));
                    tuple tempTuple(i, k, j);
                    New_map.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }
            }
        }
        mMinXYZ.x = mMinXYZ.x - mNumRowNewBlocks;
    }
    else if (direction == 1)
    {
        for (int i = mMaxXYZ.x + 1; i < mMaxXYZ.x + 1 + mNumRowNewBlocks; i++)
        {
            for (int j = mMinXYZ.z; j <= mMaxXYZ.z; j++)
            {
                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y; k <= height; k++)
                {
                    Block* pBlock = new Block(glm::ivec3(i, k, j));
                    tuple tempTuple(i, k, j);
                    New_map.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }
            }
        }
        mMaxXYZ.x = mMaxXYZ.x + mNumRowNewBlocks;
    }
    else if (direction == 2)
    {
        for (int i = mMinXYZ.x; i <= mMaxXYZ.x; i++)
        {
            for (int j = mMinXYZ.z - 1; j > mMinXYZ.z - 1 - mNumRowNewBlocks; j--)
            {
                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y; k <= height; k++)
                {
                    Block* pBlock = new Block(glm::ivec3(i, k, j));
                    tuple tempTuple(i, k, j);
                    New_map.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }
            }
        }
        mMinXYZ.z = mMinXYZ.z - mNumRowNewBlocks;
    }
    else
    {
        for (int i = mMinXYZ.x; i <= mMaxXYZ.x; i++)
        {
            for (int j = mMaxXYZ.z + 1; j < mMaxXYZ.z + 1 + mNumRowNewBlocks; j++)
            {
                int height = mPerlinNoise.GetHeight(i, j);
                for (int k = mMinXYZ.y; k <= height; k++)
                {
                    Block* pBlock = new Block(glm::ivec3(i, k, j));
                    tuple tempTuple(i, k, j);
                    New_map.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }
            }
        }
        mMaxXYZ.z = mMaxXYZ.z + mNumRowNewBlocks;
    }
    return New_map;
}

