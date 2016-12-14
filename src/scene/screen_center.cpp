#define _USE_MATH_DEFINES
#include"scene/screen_center.h"
#include<math.h>

static const int vertex_num=24;
static const int index_num=44;
void createScreenCenter(std::vector<glm::vec4> &pos,int width,int height )
{

    pos.resize(vertex_num);
    pos[0][0]=2*(0.5*width-5)/width-1;
    pos[0][1]=0;
    pos[1][0]=2*(0.5*width+5)/width-1;
    pos[1][1]=0;
    pos[2][0]=0;
    pos[2][1]=2*(0.5*height-5)/height-1;
    pos[3][0]=0;
    pos[3][1]=2*(0.5*height+5)/height-1;
    float theta=0;
    for(int i=4;i<vertex_num;i++)
    {
        pos[i][0]=2*(0.5*width+30*cos(theta))/width-1;
        pos[i][1]=2*(0.5*height+30*sin(theta))/height-1;
        theta+=M_PI/10;
    }
    for(int i=0;i<vertex_num;i++)
    {
        pos[i][2]=0;
        pos[i][3]=1;
    }
}
void createScreenCenterColor(std::vector<glm::vec4>&color)
{
    color.resize(vertex_num);
    for(int i=0;i<4;i++)
        color[i]=glm::vec4(1,0,0,1);
    for(int i=4;i<vertex_num;i++)
        color[i]=glm::vec4(0,0,0,1);
}
void createScreenCenterIndex(std::vector<GLuint>&index)
{
    index.resize(index_num);
    for(int i=0;i<4;i++)
        index[i]=i;
    int j=4;
    for(int i=4;i<vertex_num-1;i++)
    {
        index[j]=i;
        index[j+1]=i+1;
        j+=2;
    }
    index[j]=vertex_num-1;
    index[j+1]=4;

}

GLenum Screen_Center::drawMode()
{
    return GL_LINES;
}

void Screen_Center::InitializeScreenSize(int w,int h)
{
    width=w;
    height=h;
}

void Screen_Center::create()
{
    createScreenCenter(vert_pos,width,height);
    createScreenCenterColor(vert_color);
    createScreenCenterIndex(vert_index);
    generateIdx();
    generatePos();
    generateCol();
    BuildBuffer();
}
void Screen_Center::BuildBuffer()
{
    count = index_num;

    // Create a VBO on our GPU and store its handle in bufIdx

    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_num * sizeof(GLuint), vert_index.data(), GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.

    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(glm::vec4), vert_pos.data(), GL_STATIC_DRAW);

    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(glm::vec4),vert_color.data(), GL_STATIC_DRAW);


}
