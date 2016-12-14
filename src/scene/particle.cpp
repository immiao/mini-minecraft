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
        10.f, 10.f, 10.f, 1.f
    };

    context->glGenBuffers(1, &bufPos);
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(float) * 4, pos, GL_STATIC_DRAW);
}
