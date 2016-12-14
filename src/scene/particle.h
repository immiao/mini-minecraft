#ifndef PARTICLE_H
#define PARTICLE_H

#include <glwidget277.h>

class Particle
{
public:
    int count;
    GLuint bufPos;
    GLWidget277* context;
    Particle(GLWidget277* context);
    void create();
};

#endif // PARTICLE_H
