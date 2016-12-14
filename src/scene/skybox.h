#ifndef SKYBOX_H
#define SKYBOX_H

#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Skybox : public Drawable
{
public:
    Skybox(GLWidget277* context) : Drawable(context){}
    virtual void create();
};

#endif // SKYBOX_H
