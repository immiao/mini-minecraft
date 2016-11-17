#include <scene/scene.h>

#include <scene/cube.h>

Scene::Scene() : mMinXYZ(0, -1, 0), mMaxXYZ(100, 8, 100), mPerlinNoise(1.0, 1.0, 10.0, 6)
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
    for (int i = mMinXYZ.x; i < mMaxXYZ.x; i++)
    {
        for (int j = mMinXYZ.z; j < mMaxXYZ.z; j++)
        {
            int height = mPerlinNoise.GetHeight(i, j);
            //printf("%d\n", height);
            for (int k = mMinXYZ.y; k < height; k++)
            {
                Block* pBlock = new Block(glm::ivec3(i, k, j));
                tuple tempTuple(i, k, j);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
            }
        }
    }
}


