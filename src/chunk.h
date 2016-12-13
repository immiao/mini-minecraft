#ifndef CHUNK_H
#define CHUNK_H
#include <stdint.h>
#include <la.h>
#include <glwidget277.h>
#include <shaderprogram.h>
#include <drawable.h>
#include <scene/scene.h>
#include <scene/block.h>
#include <map>
#include <tuple>


#define CX 16
#define CY 16
#define CZ 16

#define SCX 64
#define SCY 64
#define SCZ 64

struct chunk{
    int block[CX][CY][CZ];
    GLuint vbo;
    int elements;
    bool changed;

    GLWidget277 *context;

    chunk(GLWidget277 *context);
    ~chunk();
    int get(int x, int y, int z);
    void set(int x, int y, int z, int type);
    void update();
    void render(ShaderProgram prog);
    //set the offset of the texture
    void setTextureOffset(glm::vec2 &, glm::vec2 &, glm::vec2 &, int type);
};

struct superchunk{
    chunk *cl[SCX][SCY][SCZ];
    glm::vec3 start_pos;

    GLWidget277 *context;

    superchunk(GLWidget277 *context);
    ~superchunk();

    int get(int x, int y, int z);
    void set(int x, int y, int z, int type);
    void render(ShaderProgram prog, glm::mat4 VP, bool RenderScreen);
    void MoveUpdate(int m, std::map<tuple, Block *> map);
};

#endif // CHUNK_H
