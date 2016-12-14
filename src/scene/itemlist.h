#pragma once

#include "drawable.h"
#include <la.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#define DEFAULT_COLOR glm::vec4(0.2,0.2,0.2,1)
class ItemList : public Drawable
{
public:
    ItemList(GLWidget277* context) : Drawable(context){}
    virtual void create();
    void Initialize(int w,int h,glm::vec4 pos,int halfsize);
    void BuildBuffer();
    void ChangeColor(glm::vec4 color);
    void SetTexture(int type,int mode);

private:
    int width;
    int height;
    glm::vec4 centerpos;
    int half_size;
    std::vector<glm::vec4> vert_pos;
    std::vector<glm::vec4> vert_color;
    std::vector<GLuint> vert_index;

    std::vector<glm::vec2> vert_uv;
};

//class Item: public Drawable
//{
//public:
//    ItemList(GLWidget277* context) : Drawable(context){}
//    virtual void create();
//    int width;
//    int height;
//    glm::vec4 centerpos;
//    std::vector<glm::vec4> vert_pos;
//    std::vector<glm::vec4> vert_color;
//    std::vector<GLuint> vert_index;

//};

