#define _USE_MATH_DEFINES
#include"screen_triangles.h"
#include<math.h>

static const int index_num = 12;
static const int vertex_num = 12;

void createVertex(std::vector<glm::vec4> &pos,int width,int height)
{
    pos.resize(vertex_num);

    pos[0][0]=2*(0.5*width+7)/width-1;
    pos[0][1]=0;
    pos[1][0]=2*(0.5*width+30*cos(-M_PI/15))/width-1;
    pos[1][1]=2*(0.5*height+30*sin(-M_PI/15))/height-1;
    pos[2][0]=2*(0.5*width+30*cos(M_PI/15))/width-1;
    pos[2][1]=2*(0.5*height+30*sin(M_PI/15))/height-1;

    pos[3][0]=0;
    pos[3][1]=2*(0.5*height+7)/height-1;
    pos[4][0]=2*(0.5*width+30*cos(-M_PI/15+M_PI/2))/width-1;
    pos[4][1]=2*(0.5*height+30*sin(-M_PI/15+M_PI/2))/height-1;
    pos[5][0]=2*(0.5*width+30*cos(M_PI/15+M_PI/2))/width-1;
    pos[5][1]=2*(0.5*height+30*sin(M_PI/15+M_PI/2))/height-1;

    for(int i=0;i<6;i++)
    {
        pos[i][2]=0;
        pos[i][3]=1;
    }

    for(int i=6;i<vertex_num;i++)
        pos[i]=glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0,0,1))*pos[i-6];




}

void createVertexColor(std::vector<glm::vec4> &color)
{
    color.resize(vertex_num);
    for(int i=0;i<color.size();i++)
        color[i]=glm::vec4(0,0,0,1);
}

void createVertexIndex(std::vector<GLuint> &index)
{
    index.resize(index_num);
    for(int i=0;i<index.size();i++)
        index[i]=i;

}

void Screen_Triangles::InitializeScreenSize(int w,int h)
{
    width=w;
    height=h;
}

void Screen_Triangles::create()
{

    createVertex(vert_pos,width,height);
    createVertexColor(vert_color);
    createVertexIndex(vert_index);
    generateIdx();
    generatePos();
    generateCol();
    BuildBuffer();
}
void Screen_Triangles::BuildBuffer()
{
    count = index_num;

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_num * sizeof(GLuint), vert_index.data(), GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(glm::vec4), vert_pos.data(), GL_STATIC_DRAW);

    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(glm::vec4),vert_color.data(), GL_STATIC_DRAW);


}
