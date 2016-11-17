#ifndef CHUNK_H
#define CHUNK_H
#include <stdint.h>
#include <la.h>
#include <glwidget277.h>
#define CX 16
#define CY 16
#define CZ 16


struct chunk{
    uint8_t block[CX][CY][CZ];
    GLuint vbo;
    int elements;
    bool changed;

    GLWidget277 *context;

    chunk(GLWidget277 *context);
    ~chunk();
    uint8_t get(int x, int y, int z);
    void set(int x, int y, int z, uint8_t type);
    void update();
    void render();
};

struct superchunk{
    std::vector <std::vector <std::vector <chunk> > > *cl;


};

#endif // CHUNK_H
