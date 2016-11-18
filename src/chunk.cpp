#include "chunk.h"
#include <la.h>
#include <glwidget277.h>
#include <shaderprogram.h>
#include <iostream>

#define CX 16
#define CY 16
#define CZ 16

#define SCX 64
#define SCY 64
#define SCZ 64

struct VertexData{
    glm::ivec4 pos;
    glm::ivec4 nor;
    glm::vec4 col;
};

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

int chunk::get(int x, int y, int z){
    return block[x][y][z];
}

void chunk::set(int x, int y, int z, int type){
    block[x][y][z] = type;
    changed = true;
}

void chunk::update(){
    changed = false;

    std::vector<glm::ivec4> vertex;
    int i = 0;
    for(int x = 0; x < CX; x++)
        for(int y = 0; y < CY; y++)
            for(int z = 0; z < CZ; z++){
                int type = block[x][y][z];
                //Empty Block?
                if(!type)
                    continue;
                //View from negative x
                vertex.push_back(glm::ivec4(x   ,y  ,z  ,   type));
                vertex.push_back(glm::ivec4(x   ,y  ,z+1,   type));
                vertex.push_back(glm::ivec4(x   ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x   ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x   ,y  ,z+1,   type));
                vertex.push_back(glm::ivec4(x   ,y+1,z+1,   type));
                //View from positive x
                vertex.push_back(glm::ivec4(x+1 ,y  ,z  ,   type));
                vertex.push_back(glm::ivec4(x+1 ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x+1 ,y  ,z+1,   type));
                vertex.push_back(glm::ivec4(x+1 ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x+1 ,y+1,z+1,   type));
                vertex.push_back(glm::ivec4(x+1 ,y  ,z+1,   type));
                //View from negative y
                vertex.push_back(glm::ivec4(x   ,y  ,z  ,   type));
                vertex.push_back(glm::ivec4(x+1 ,y  ,z  ,   type));
                vertex.push_back(glm::ivec4(x   ,y  ,z+1,   type));
                vertex.push_back(glm::ivec4(x+1 ,y  ,z  ,   type));
                vertex.push_back(glm::ivec4(x+1 ,y  ,z+1,   type));
                vertex.push_back(glm::ivec4(x   ,y  ,z+1,   type));
                //View from positive y
                vertex.push_back(glm::ivec4(x   ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x   ,y+1,z+1,   type));
                vertex.push_back(glm::ivec4(x+1 ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x+1 ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x   ,y+1,z+1,   type));
                vertex.push_back(glm::ivec4(x+1 ,y+1,z+1,   type));
                //View from nagative z
                vertex.push_back(glm::ivec4(x   ,y  ,z  ,   type));
                vertex.push_back(glm::ivec4(x   ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x+1 ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x   ,y  ,z  ,   type));
                vertex.push_back(glm::ivec4(x+1 ,y+1,z  ,   type));
                vertex.push_back(glm::ivec4(x+1 ,y  ,z  ,   type));
                //View from positive z
                vertex.push_back(glm::ivec4(x   ,y  ,z+1,   type));
                vertex.push_back(glm::ivec4(x+1 ,y  ,z+1,   type));
                vertex.push_back(glm::ivec4(x   ,y+1,z+1,   type));
                vertex.push_back(glm::ivec4(x+1 ,y  ,z+1,   type));
                vertex.push_back(glm::ivec4(x+1 ,y+1,z+1,   type));
                vertex.push_back(glm::ivec4(x   ,y+1,z+1,   type));
            }
    elements = vertex.size();
    context->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    context->glBufferData(GL_ARRAY_BUFFER, elements * sizeof(glm::ivec4), vertex.data(), GL_STATIC_DRAW);
}

void chunk::render(ShaderProgram prog){
    if(this->changed)
        this->update();

    //If the chunk is empty, we don't draw
    if(!elements)
        return;
    prog.useMe();
    context->glEnable(GL_CULL_FACE);
    context->glEnable(GL_DEPTH_TEST);

    context->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    int attribute_coord = -1;
    attribute_coord = context->glGetAttribLocation(prog.prog, "vs_coord");
    context->glEnableVertexAttribArray(attribute_coord);
    context->glVertexAttribIPointer(attribute_coord, 4, GL_INT, 0, 0);
    context->glDrawArrays(GL_TRIANGLES, 0, elements);
}

superchunk::superchunk(GLWidget277 *context){
    this->context = context;
    memset(cl, 0, sizeof cl);
}

superchunk::~superchunk(){
    for(int x = 0; x < SCX; x++)
          for(int y = 0; y < SCX; y++)
            for(int z = 0; z < SCX; z++)
              delete cl[x][y][z];
}

int superchunk::get(int x, int y, int z){
    //convert the position with respect to the left-bottom corner chunk, cl[0][0][0]
    x = x - start_pos[0];
    y = y - start_pos[1];
    z = z - start_pos[2];

    int cx = x / CX;
    int cy = y / CY;
    int cz = z / CZ;

    x %= CX;
    y %= CY;
    z %= CZ;

    if(!cl[cx][cy][cz])
        return 0;
    else
        return cl[cx][cy][cz]->get(x, y, z);
}

void superchunk::set(int x, int y, int z, int type){
    //convert the position with respect to the left-bottom corner chunk, cl[0][0][0]
    x = x - start_pos[0];
    y = y - start_pos[1];
    z = z - start_pos[2];

    int cx = x / CX;
    int cy = y / CY;
    int cz = z / CZ;

    x %= CX;
    y %= CY;
    z %= CZ;

    if(!cl[cx][cy][cz])
        cl[cx][cy][cz] = new chunk(context);
    cl[cx][cy][cz]->set(x, y, z, type);
}

void superchunk::render(ShaderProgram prog){

    for(int x = 0; x < SCX; x++)
        for(int y = 0; y < SCY; y++)
            for(int z = 0; z < SCZ; z++){
                if(cl[x][y][z]){
//                    std::cout<<x<<" "<<y<<" "<<z<<"\n";
                    glm::mat4 model = glm::translate(glm::mat4(1), glm::vec3(x * CX, y * CY, z * CZ) + start_pos);
                    prog.setModelMatrix(model);
                    cl[x][y][z]->render(prog);
                }
            }
}

void superchunk::MoveUpdate(int m, std::map<tuple, Block*> map){
    if(m == 0){
        //set the new start_pos
        start_pos = start_pos + glm::vec3(16,0,0);
        //delete the -x-end line
        for(int y = 0; y < SCY; y++)
            for(int z = 0; z < SCZ; z++){
                if(cl[0][y][z]){
                    cl[0][y][z] = nullptr;
                }
            }
        //shift to -x
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCY; y++)
                for(int z = 0; z < SCZ; z++){
                    if(x == SCX-1){
                        cl[x][y][z] = nullptr;
                    }
                    else{
                        cl[x][y][z] = cl[x+1][y][z];
                    }
                }
        //set new line
        std::map<tuple, Block*>::iterator iter;
        for (iter = map.begin(); iter != map.end(); iter++)
        {
            // iter->first is a tuple
            // std::get<index>(tuple) can get the elements in the tuple
            int x = std::get<0>(iter->first), y = std::get<1>(iter->first), z = std::get<2>(iter->first);
            if(x >= (start_pos[0] + 63 * 16) && x <= (start_pos[0] + 64 * 16) &&
                    y >= start_pos[1] && y <= (start_pos[1] + 64 * 16) &&
                    z >= start_pos[2] && z <= (start_pos[2] + 64 * 16)){
                set(x,y,z,int(1));
            }
        }
    }
    else if(m == 1){
        //set the new start_pos
        start_pos = start_pos + glm::vec3(-16,0,0);
        //delete the +x-end line
        for(int y = 0; y < SCY; y++)
            for(int z = 0; z < SCZ; z++){
                if(cl[63][y][z]){
                    cl[63][y][z] = nullptr;
                }
            }
        //shift to +x
        for(int x = SCX - 1; x >= 0; x--)
            for(int y = 0; y < SCY; y++)
                for(int z = 0; z < SCZ; z++){
                    if(x == 0){
                        cl[x][y][z] = nullptr;
                    }
                    else{
                        cl[x][y][z] = cl[x-1][y][z];
                    }
                }
        //set new line
        std::map<tuple, Block*>::iterator iter;
        for (iter = map.begin(); iter != map.end(); iter++)
        {
            // iter->first is a tuple
            // std::get<index>(tuple) can get the elements in the tuple
            int x = std::get<0>(iter->first), y = std::get<1>(iter->first), z = std::get<2>(iter->first);
            if(x >= start_pos[0] && x <= (start_pos[0] + 16) &&
                    y >= start_pos[1] && y <= (start_pos[1] + 64 * 16) &&
                    z >= start_pos[2] && z <= (start_pos[2] + 64 * 16)){
                set(x,y,z,int(1));
            }
        }
    }
    else if(m == 2){
        //set the new start_pos
        start_pos = start_pos + glm::vec3(0,16,0);
        //delete the -y-end line
        for(int x = 0; x < SCX; x++)
            for(int z = 0; z < SCZ; z++){
                if(cl[x][0][z]){
                    cl[x][0][z] = nullptr;
                }
            }
        //shift to -y
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCY; y++)
                for(int z = 0; z < SCZ; z++){
                    if(y == SCY-1){
                        cl[x][y][z] = nullptr;
                    }
                    else{
                        cl[x][y][z] = cl[x][y+1][z];
                    }
                }
        //set new line
        std::map<tuple, Block*>::iterator iter;
        for (iter = map.begin(); iter != map.end(); iter++)
        {
            // iter->first is a tuple
            // std::get<index>(tuple) can get the elements in the tuple
            int x = std::get<0>(iter->first), y = std::get<1>(iter->first), z = std::get<2>(iter->first);
            if(x >= start_pos[0] && x <= (start_pos[0] + 64 * 16) &&
                    y >= (start_pos[1] + 63 * 16) && y <= (start_pos[1] + 64 * 16) &&
                    z >= start_pos[2] && z <= (start_pos[2] + 64 * 16)){
                set(x,y,z,int(1));
            }
        }
    }
    else if(m == 3){
        //set the new start_pos
        start_pos = start_pos + glm::vec3(0,-16,0);
        //delete the +y-end line
        for(int x = 0; x < SCX; x++)
            for(int z = 0; z < SCZ; z++){
                if(cl[x][63][z]){
                    cl[x][63][z] = nullptr;
                }
            }
        //shift to +x
        for(int x = 0; x < SCX; x++)
            for(int y = SCY - 1; y >= 0; y--)
                for(int z = 0; z < SCZ; z++){
                    if(y == 0){
                        cl[x][y][z] = nullptr;
                    }
                    else{
                        cl[x][y][z] = cl[x][y-1][z];
                    }
                }
        //set new line
        std::map<tuple, Block*>::iterator iter;
        for (iter = map.begin(); iter != map.end(); iter++)
        {
            // iter->first is a tuple
            // std::get<index>(tuple) can get the elements in the tuple
            int x = std::get<0>(iter->first), y = std::get<1>(iter->first), z = std::get<2>(iter->first);
            if(x >= start_pos[0] && x <= (start_pos[0] + 64 * 16) &&
                    y >= start_pos[1] && y <= (start_pos[1] + 16) &&
                    z >= start_pos[2] && z <= (start_pos[2] + 64 * 16)){
                set(x,y,z,int(1));
            }
        }
    }
    else if(m == 4){
        //set the new start_pos
        start_pos = start_pos + glm::vec3(0,0,16);
        //delete the -z-end line
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCY; y++){
                if(cl[x][y][0]){
                    cl[x][y][0] = nullptr;
                }
            }
        //shift to -z
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCY; y++)
                for(int z = 0; z < SCZ; z++){
                    if(z == SCZ-1){
                        cl[x][y][z] = nullptr;
                    }
                    else{
                        cl[x][y][z] = cl[x][y][z+1];
                    }
                }
        //set new line
        std::map<tuple, Block*>::iterator iter;
        for (iter = map.begin(); iter != map.end(); iter++)
        {
            // iter->first is a tuple
            // std::get<index>(tuple) can get the elements in the tuple
            int x = std::get<0>(iter->first), y = std::get<1>(iter->first), z = std::get<2>(iter->first);
            if(x >= start_pos[0] && x <= (start_pos[0] + 64 * 16) &&
                    y >= start_pos[1] && y <= (start_pos[1] + 64 * 16) &&
                    z >= (start_pos[2] + 63 * 16) && z <= (start_pos[2] + 64 * 16)){
                set(x,y,z,int(1));
            }
        }
    }
    else if(m == 5){
        //set the new start_pos
        start_pos = start_pos + glm::vec3(0,0,-16);
        //delete the +z-end line
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCY; y++){
                if(cl[x][y][63]){
                    cl[x][y][63] = nullptr;
                }
            }
        //shift to +z
        for(int x = 0; x < SCX; x++)
            for(int y = 0; y < SCY; y++)
                for(int z = SCZ - 1; z >= 0; z--){
                    if(z == 0){
                        cl[x][y][z] = nullptr;
                    }
                    else{
                        cl[x][y][z] = cl[x][y][z-1];
                    }
                }
        //set new line
        std::map<tuple, Block*>::iterator iter;
        for (iter = map.begin(); iter != map.end(); iter++)
        {
            // iter->first is a tuple
            // std::get<index>(tuple) can get the elements in the tuple
            int x = std::get<0>(iter->first), y = std::get<1>(iter->first), z = std::get<2>(iter->first);
            if(x >= start_pos[0] && x <= (start_pos[0] + 64 * 16) &&
                    y >= start_pos[1] && y <= (start_pos[1] + 64 * 16) &&
                    z >= start_pos[2] && z <= (start_pos[2] + 16)){
                set(x,y,z,int(1));
            }
        }
    }
}
