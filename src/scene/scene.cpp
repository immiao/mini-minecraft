#include <scene/scene.h>

#include <scene/cube.h>

Scene::Scene() : mMinXYZ(0, -1, 0), mMaxXYZ(1, 8, 10), mPerlinNoise(0.5, 1.0, 10.0, 6), mRefreshDistance(5.0), mNumRowNewBlocks(10)
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
}

void Scene::GenerateBlocks(int direction)
{
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
                    mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
                }
            }
        }
        mMaxXYZ.z = mMaxXYZ.z + mNumRowNewBlocks;
    }
}

