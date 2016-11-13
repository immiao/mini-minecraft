#include <scene/scene.h>

#include <scene/cube.h>

Scene::Scene() : dimensions(64, 64, 64)
{
}

void Scene::CreateTestScene()
{
    for(int x = 0; x < dimensions.x; x++)
    {
        QList<QList<bool>> Ys;
        for(int y = 0; y < dimensions.y; y++)
        {
            QList<bool> Zs;
            for(int z = 0; z < dimensions.z; z++)
            {
                if(y == dimensions.y/2)
                {
                    Zs.push_back(true);
                }
                else
                {
                    Zs.push_back(false);
                }
            }
            Ys.push_back(Zs);
        }
        objects.push_back(Ys);
    }
    objects[0][dimensions.y/2+2][0] = true;
    objects[dimensions.x-1][dimensions.y/2+2][0] = true;
    objects[0][dimensions.y/2+2][dimensions.z-1] = true;
    objects[dimensions.x-1][dimensions.y/2+2][dimensions.z-1] = true;
}
