#include "skybox.h"

void Skybox::create()
{
    float points[] = {
      -10.0f,  10.0f, -10.0f, 1.f,
      -10.0f, -10.0f, -10.0f, 1.f,
       10.0f, -10.0f, -10.0f, 1.f,
       10.0f, -10.0f, -10.0f, 1.f,
       10.0f,  10.0f, -10.0f, 1.f,
      -10.0f,  10.0f, -10.0f, 1.f,

      -10.0f, -10.0f,  10.0f, 1.f,
      -10.0f, -10.0f, -10.0f, 1.f,
      -10.0f,  10.0f, -10.0f, 1.f,
      -10.0f,  10.0f, -10.0f, 1.f,
      -10.0f,  10.0f,  10.0f, 1.f,
      -10.0f, -10.0f,  10.0f, 1.f,

       10.0f, -10.0f, -10.0f, 1.f,
       10.0f, -10.0f,  10.0f, 1.f,
       10.0f,  10.0f,  10.0f, 1.f,
       10.0f,  10.0f,  10.0f, 1.f,
       10.0f,  10.0f, -10.0f, 1.f,
       10.0f, -10.0f, -10.0f, 1.f,

      -10.0f, -10.0f,  10.0f, 1.f,
      -10.0f,  10.0f,  10.0f, 1.f,
       10.0f,  10.0f,  10.0f, 1.f,
       10.0f,  10.0f,  10.0f, 1.f,
       10.0f, -10.0f,  10.0f, 1.f,
      -10.0f, -10.0f,  10.0f, 1.f,

      -10.0f,  10.0f, -10.0f, 1.f,
       10.0f,  10.0f, -10.0f, 1.f,
       10.0f,  10.0f,  10.0f, 1.f,
       10.0f,  10.0f,  10.0f, 1.f,
      -10.0f,  10.0f,  10.0f, 1.f,
      -10.0f,  10.0f, -10.0f, 1.f,

      -10.0f, -10.0f, -10.0f, 1.f,
      -10.0f, -10.0f,  10.0f, 1.f,
       10.0f, -10.0f, -10.0f, 1.f,
       10.0f, -10.0f, -10.0f, 1.f,
      -10.0f, -10.0f,  10.0f, 1.f,
       10.0f, -10.0f,  10.0f, 1.f,
    };

    count = 36;

    int idx[36];
    for (int i = 0; i < 36; i++)
        idx[i] = i;

    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), idx, GL_STATIC_DRAW);

    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(glm::vec4), points, GL_STATIC_DRAW);
}
