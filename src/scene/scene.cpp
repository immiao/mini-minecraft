#include <scene/scene.h>

#include <scene/cube.h>

Scene::Scene() : mDimensions(8, 8, 8)
{

}

Scene::~Scene()
{
    clear();
}

void Scene::clear()
{
    for (int i = 0; i < mDimensions.x; i++)
    {
        for (int j = 0; j < mDimensions.y; j++)
        {
            for (int k = 0; k < mDimensions.z; k++)
            {
                tuple tempTuple(i, j, k);
                std::map<tuple, Block*>::iterator iter = mSceneMap.find(tempTuple);
                if (iter != mSceneMap.end())
                    delete iter->second;
            }
        }
    }
}

void Scene::Create()
{
    for (int i = 0; i < mDimensions.x; i++)
    {
        for (int j = 0; j < mDimensions.y; j++)
        {
            for (int k = 0; k < mDimensions.z; k++)
            {
                Block* pBlock = new Block(glm::ivec3(i, j, k));
                tuple tempTuple(i, j, k);
                mSceneMap.insert(std::pair<tuple, Block*>(tempTuple, pBlock));
            }
        }
    }
}


