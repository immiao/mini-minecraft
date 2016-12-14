#define _USE_MATH_DEFINES
#include<iostream>
#include<scene/block.h>
#include"scene/itemlist.h"
#include<math.h>

static const int index_num = 6;
static const int vertex_num = 4;

void createItemListVertex(std::vector<glm::vec4> &pos,glm::vec4 centerpos,int width,int height,int half_size)
{
    pos.resize(vertex_num);

    pos[0][0]=2*(centerpos[0]-half_size)/width-1;
    pos[0][1]=2*(centerpos[1]+half_size)/height-1;
    pos[1][0]=2*(centerpos[0]+half_size)/width-1;
    pos[1][1]=2*(centerpos[1]+half_size)/height-1;
    pos[2][0]=2*(centerpos[0]+half_size)/width-1;
    pos[2][1]=2*(centerpos[1]-half_size)/height-1;
    pos[3][0]=2*(centerpos[0]-half_size)/width-1;
    pos[3][1]=2*(centerpos[1]-half_size)/height-1;

    for(int i=0;i<4;i++)
    {
        pos[i][2]=0;
        pos[i][3]=1;
    }
}

void createItemListColor(std::vector<glm::vec4> &color)
{
    color.resize(vertex_num);
    for(int i=0;i<color.size();i++)
        color[i]=DEFAULT_COLOR;
}

void createItemListIndex(std::vector<GLuint> &index)
{
    index.resize(index_num);
    index[0]=0;
    index[1]=2;
    index[2]=1;
    index[3]=0;
    index[4]=3;
    index[5]=2;

}

void ItemList::Initialize(int w, int h, glm::vec4 pos, int halfsize)
{
    width=w;
    height=h;
    centerpos=pos;
    half_size=halfsize;
}

void ItemList::create()
{
    createItemListVertex(vert_pos,centerpos,width,height,half_size);
    createItemListColor(vert_color);
    createItemListIndex(vert_index);
    generateIdx();
    generatePos();
    generateCol();
    BuildBuffer();
}

void ItemList::BuildBuffer()
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

void ItemList::ChangeColor(glm::vec4 color)
{
    for(int i=0;i<vert_color.size();i++)
        vert_color[i]=color;
}

void ItemList::SetTexture(int type,int mode)
{
    vert_uv.resize(vertex_num);
    if(mode==1)
    {
        if(type==DIRT)
        {
            vert_uv[0]=glm::vec2(2.0f*1.0f/16.0f,0.0f);
            vert_uv[1]=glm::vec2(3.0f*1.0f/16.0f,0.0f);
            vert_uv[2]=glm::vec2(3.0f*1.0f/16.0f,1.0f/16.0f);
            vert_uv[3]=glm::vec2(2.0f*1.0f/16.0f,1.0f/16.0f);
        }
        else if(type==GRASS)
        {
            vert_uv[0]=glm::vec2(3.0f*1.0f/16.0f,0.0f);
            vert_uv[1]=glm::vec2(4.0f*1.0f/16.0f,0.0f);
            vert_uv[2]=glm::vec2(4.0f*1.0f/16.0f,1.0f/16.0f);
            vert_uv[3]=glm::vec2(3.0f*1.0f/16.0f,1.0f/16.0f);
        }
        else if(type==STONE)
        {
            vert_uv[0]=glm::vec2(1.0f*1.0f/16.0f,0.0f);
            vert_uv[1]=glm::vec2(2.0f*1.0f/16.0f,0.0f);
            vert_uv[2]=glm::vec2(2.0f*1.0f/16.0f,1.0f/16.0f);
            vert_uv[3]=glm::vec2(1.0f*1.0f/16.0f,1.0f/16.0f);
        }
        else if(type==WOOD)
        {
            vert_uv[0]=glm::vec2(5.0f*1.0f/16.0f,1.0f/16.0f);
            vert_uv[1]=glm::vec2(6.0f*1.0f/16.0f,1.0f/16.0f);
            vert_uv[2]=glm::vec2(6.0f*1.0f/16.0f,2.0f/16.0f);
            vert_uv[3]=glm::vec2(5.0f*1.0f/16.0f,2.0f/16.0f);

        }
        else if(type==LEAF)
        {
            vert_uv[0]=glm::vec2(5.0f*1.0f/16.0f,3.0f/16.0f);
            vert_uv[1]=glm::vec2(6.0f*1.0f/16.0f,3.0f/16.0f);
            vert_uv[2]=glm::vec2(6.0f*1.0f/16.0f,4.0f/16.0f);
            vert_uv[3]=glm::vec2(5.0f*1.0f/16.0f,4.0f/16.0f);

        }
        else if(type==BEDROCK)
        {
            vert_uv[0]=glm::vec2(1.0f*1.0f/16.0f,1.0f/16.0f);
            vert_uv[1]=glm::vec2(2.0f*1.0f/16.0f,1.0f/16.0f);
            vert_uv[2]=glm::vec2(2.0f*1.0f/16.0f,2.0f/16.0f);
            vert_uv[3]=glm::vec2(1.0f*1.0f/16.0f,2.0f/16.0f);
        }

        else if(type==COAL)
        {
            vert_uv[0]=glm::vec2(2.0f*1.0f/16.0f,2.0f/16.0f);
            vert_uv[1]=glm::vec2(3.0f*1.0f/16.0f,2.0f/16.0f);
            vert_uv[2]=glm::vec2(3.0f*1.0f/16.0f,3.0f/16.0f);
            vert_uv[3]=glm::vec2(2.0f*1.0f/16.0f,3.0f/16.0f);
        }
        else if(type==IRON_ORE)
        {
            vert_uv[0]=glm::vec2(1.0f*1.0f/16.0f,2.0f/16.0f);
            vert_uv[1]=glm::vec2(2.0f*1.0f/16.0f,2.0f/16.0f);
            vert_uv[2]=glm::vec2(2.0f*1.0f/16.0f,3.0f/16.0f);
            vert_uv[3]=glm::vec2(1.0f*1.0f/16.0f,3.0f/16.0f);
        }
        else if(type==LAVA)
        {
            vert_uv[0]=glm::vec2(15.0f*1.0f/16.0f,15.0f/16.0f);
            vert_uv[1]=glm::vec2(16.0f*1.0f/16.0f,15.0f/16.0f);
            vert_uv[2]=glm::vec2(16.0f*1.0f/16.0f,16.0f/16.0f);
            vert_uv[3]=glm::vec2(15.0f*1.0f/16.0f,16.0f/16.0f);
        }
    }
    else if(mode==2)
    {
        if(type==0)
        {
            vert_uv[0]=glm::vec2(0.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(1.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(1.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(0.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==1)
        {
            vert_uv[0]=glm::vec2(1.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(2.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(2.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(1.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==2)
        {
            vert_uv[0]=glm::vec2(2.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(3.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(3.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(2.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==3)
        {
            vert_uv[0]=glm::vec2(3.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(4.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(4.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(3.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==4)
        {
            vert_uv[0]=glm::vec2(4.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(5.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(5.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(4.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==5)
        {
            vert_uv[0]=glm::vec2(5.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(6.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(6.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(5.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==6)
        {
            vert_uv[0]=glm::vec2(6.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(7.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(7.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(6.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==7)
        {
            vert_uv[0]=glm::vec2(7.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(8.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(8.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(7.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==8)
        {
            vert_uv[0]=glm::vec2(8.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(9.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(9.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(8.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==9)
        {
            vert_uv[0]=glm::vec2(9.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[1]=glm::vec2(10.0f*1.0f/16.0f,1.0f/6.0f);
            vert_uv[2]=glm::vec2(10.0f*1.0f/16.0f,2.0f/6.0f);
            vert_uv[3]=glm::vec2(9.0f*1.0f/16.0f,2.0f/6.0f);
        }
        else if(type==-1)
        {
            vert_uv[0]=glm::vec2(8.0f*1.0f/16.0f,5.0f/6.0f);
            vert_uv[1]=glm::vec2(9.0f*1.0f/16.0f,5.0f/6.0f);
            vert_uv[2]=glm::vec2(9.0f*1.0f/16.0f,6.0f/6.0f);
            vert_uv[3]=glm::vec2(8.0f*1.0f/16.0f,6.0f/6.0f);

        }
    }
    context->glDeleteBuffers(1,&bufUv);
    generateUv();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufUv);
    context->glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(glm::vec2),vert_uv.data(), GL_STATIC_DRAW);

}
