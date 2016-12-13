#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>

enum BLOCK_TYPE
{
    NONE,
    DIRT,
    GRASS,
    STONE,
    WOOD,
    LEAF,
    BEDROCK,
    COAL,
    IRON_ORE,
    LAVA,
    WATER,
    SAND,
    SNOW,
};

class Block
{
public:
    BLOCK_TYPE mType;
    glm::ivec3 mPosition;
    Block(glm::ivec3 position, BLOCK_TYPE mType = GRASS);
};

#endif // BLOCK_H
