#include "particle.h"

Particle::Particle(GLWidget277* context)
{
    this->context = context;

}

void Particle::create()
{
    count = 1;

    const float pos[] =
    {
        10.f, 50.f, 10.f, 1.f
    };

    context->glGenBuffers(1, &bufPos[0]);
    //printf("%d\n", bufPosA);
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufPos[0]);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(float) * 4, pos, GL_STATIC_DRAW);

//    context->glGenBuffers(1, &bufPos[1]);
//    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufPos[1]);
//    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(float) * 4, 0, GL_STATIC_DRAW);
}
