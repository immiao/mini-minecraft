#include "particle.h"
#include "glm/glm.hpp"

Particle::Particle(GLWidget277* context)
{
    this->context = context;

}

void Particle::create()
{
    count = 100;

    glm::vec4 pos[100];

    for (int i = 0; i < 100; i++)
    {
        float height = rand() % 200 / 10.f + 60;
        float x = rand() % 200 - 150;
        float z = rand() % 200 - 150;
        pos[i] = glm::vec4(x, height, z, 1.0f);
        //printf("%f %f %f\n",x ,height, z);
    }

    context->glGenBuffers(1, &bufPos);
    //printf("%d\n", bufPosA);
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(glm::vec4), pos, GL_STATIC_DRAW);
}
