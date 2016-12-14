#ifndef PARTICLE_H
#define PARTICLE_H

#include <mygl.h>

class Particle
{
public:
    int count;
    GLuint bufPos;
    GLWidget277* context;
    Particle(GLWidget277* context);
};

#endif // PARTICLE_H
