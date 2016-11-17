#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>

class Block
{
public:
    glm::ivec3 mPosition;
    Block(glm::ivec3 position);
};

#endif // BLOCK_H
