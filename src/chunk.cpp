#include "chunk.h"
#include <la.h>
#include <glwidget277.h>

#define CX 16
#define CY 16
#define CZ 16

typedef glm::ivec4 int4;

chunk::chunk(GLWidget277* context)
{
    this->context = context;
    memset(block, 0, sizeof(block));
    elements = 0;
    changed = true;
    this->context->glGenBuffers(1, &vbo);
}

chunk::~chunk(){
    context->glDeleteBuffers(1,&vbo);
}

uint8_t chunk::get(int x, int y, int z){
    return block[x][y][z];
}

void chunk::set(int x, int y, int z, uint8_t type){
    block[x][y][z] = type;
    changed = true;
}

void chunk::update(){
    changed = false;

    int4 vertex[CX*CY*CZ*6*6];
    int i = 0;
    for(int x = 0; x < CX; x++)
        for(int y = 0; y < CY; y++)
            for(int z = 0; z < CZ; z++){
                uint8_t type = block[x][y][z];
                //Empty Block?
                if(!type)
                    continue;
                //View from negative x
                //View from positive x
                //View from negative y
                //View from positive y
                //View from nagative z
                //View from positive z
            }
    elements = i;
    context->glGenBuffers(GL_ARRAY_BUFFER, &vbo);
    context->glBufferData(GL_ARRAY_BUFFER, elements * sizeof(vertex), vertex, GL_STATIC_DRAW);
}

void chunk::render(){
    if(changed)
        update();

    //If the chunk is empty, we don't draw
    if(!elements)
        return;

    context->glEnable(GL_CULL_FACE);
    context->glEnable(GL_DEPTH_TEST);

    context->glGenBuffers(GL_ARRAY_BUFFER, &vbo);
    //context->glVertexAttribPointer(attribute_coord, 4, GL_INT, GL_FALSE, 0, 0);
    context->glDrawArrays(GL_TRIANGLES, 0, elements);
}
