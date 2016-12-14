#ifndef FIREBLOCKS
#define FIREBLOCKS
#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Fire
{
public:
    Fire();
    bool FireBurning();
    void setpos(glm::vec3 pos);
    glm::vec3 getpos();
    double Get_Lifetime();
    double Get_Spreadtime();
private:
    glm::vec3 position;
    double lifetime=20;
    double spreadtime=5;
};
#endif // FIREBLOCKS


