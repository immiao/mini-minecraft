#pragma once

#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Screen_Triangles : public Drawable
{
public:
    Screen_Triangles(GLWidget277* context) : Drawable(context){}
    virtual void create();
    void InitializeScreenSize(int w,int h);

private:
    int width;
    int height;
};

