#pragma once
#include <QList>
#include <camera.h>

class Scene
{
public:
    Scene();
    QList<QList<QList<bool>>> objects;//A 3D list of the geometry in the world. Currently, it just lists whether or not a given cell contains a block.
    void CreateTestScene();

    glm::ivec3 dimensions;
};
