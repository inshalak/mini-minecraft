#pragma once
#include <array>
#include <unordered_set>
#include "glm/glm.hpp"

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, WATER, SNOW, LAVA, BEDROCK,
    REDSTONE_TORCH_ON, REDSTONE_TORCH_OFF,
    REDSTONE_WIRE_ON, REDSTONE_WIRE_OFF,
    REDSTONE_LEVER_ON, REDSTONE_LEVER_OFF,
    REDSTONE_LAMP_ON, REDSTONE_LAMP_OFF,
    SPRUCE_SAPLING, ROSE, DAF, REDSHROOM, SHROOM, DRY_SPRIG, CACTUS
};

const static std::unordered_set<unsigned char> redstoneBlocks {
    REDSTONE_TORCH_ON, REDSTONE_TORCH_OFF,
    REDSTONE_WIRE_ON, REDSTONE_WIRE_OFF,
    REDSTONE_LEVER_ON, REDSTONE_LEVER_OFF,
    REDSTONE_LAMP_ON, REDSTONE_LAMP_OFF
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

struct VertexData {
    glm::vec4 pos;
    glm::vec2 uv;
    VertexData(glm::vec4 p, glm::vec2 u)
        : pos(p), uv(u)
    {}
};

struct BlockFace {
    Direction direction;
    glm::vec3 directionVec;
    std::array<VertexData, 4> vertices;
    BlockFace(Direction dir, glm::vec3 dirV, const VertexData &a, const VertexData &b, const VertexData &c, const VertexData &d)
        : direction(dir), directionVec(dirV), vertices{a, b, c, d}
    {}
};

#define BLK_UV 0.0625

const static std::array<BlockFace, 10> cactusFaces {
    BlockFace(XPOS, glm::vec3(1, 0, 0), VertexData(glm::vec4(0.9375,0,1,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0.9375,0,0,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0.9375,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0.9375,1,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(XNEG, glm::vec3(-1, 0, 0), VertexData(glm::vec4(0.9375,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0.9375,0,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0.9375,1,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0.9375,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(XPOS, glm::vec3(1, 0, 0), VertexData(glm::vec4(0.0625,0,1,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0.0625,0,0,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0.0625,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0.0625,1,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(XNEG, glm::vec3(-1, 0, 0), VertexData(glm::vec4(0.0625,0,0,1), glm::vec2(0,0)),
                                         VertexData(glm::vec4(0.0625,0,1,1), glm::vec2(BLK_UV,0)),
                                         VertexData(glm::vec4(0.0625,1,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                         VertexData(glm::vec4(0.0625,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(YPOS, glm::vec3(0, 1, 0), VertexData(glm::vec4(0,1,1,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,1,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(YNEG, glm::vec3(0, -1, 0), VertexData(glm::vec4(0,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,0,0,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,0,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,0,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZPOS, glm::vec3(0, 0, 1), VertexData(glm::vec4(0,0,0.9375,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,0,0.9375,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,1,0.9375,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,1,0.9375,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZNEG, glm::vec3(0, 0, -1), VertexData(glm::vec4(1,0,0.9375,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0,0,0.9375,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0,1,0.9375,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(1,1,0.9375,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZPOS, glm::vec3(0, 0, 1), VertexData(glm::vec4(0,0,0.0625,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,0,0.0625,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,1,0.0625,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,1,0.0625,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZNEG, glm::vec3(0, 0, -1), VertexData(glm::vec4(1,0,0.0625,1), glm::vec2(0,0)),
                                         VertexData(glm::vec4(0,0,0.0625,1), glm::vec2(BLK_UV,0)),
                                         VertexData(glm::vec4(0,1,0.0625,1), glm::vec2(BLK_UV, BLK_UV)),
                                         VertexData(glm::vec4(1,1,0.0625,1), glm::vec2(0,BLK_UV))),
};

const static std::array<BlockFace, 4> flowerFaces {
    BlockFace(XPOS, glm::vec3(1, 0, 0), VertexData(glm::vec4(0,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,0,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,1,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(XNEG, glm::vec3(-1, 0, 0), VertexData(glm::vec4(1,0,1,1), glm::vec2(0,0)),
                                         VertexData(glm::vec4(0,0,0,1), glm::vec2(BLK_UV,0)),
                                         VertexData(glm::vec4(0,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                         VertexData(glm::vec4(1,1,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(XPOS, glm::vec3(0, 0, 1), VertexData(glm::vec4(1,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0,0,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0,1,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(1,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(XNEG, glm::vec3(0, 0, -1), VertexData(glm::vec4(0,0,1,1), glm::vec2(0,0)),
                                         VertexData(glm::vec4(1,0,0,1), glm::vec2(BLK_UV,0)),
                                         VertexData(glm::vec4(1,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                         VertexData(glm::vec4(0,1,1,1), glm::vec2(0,BLK_UV))),
};

const static std::array<BlockFace, 4> leverOnFaces {
    BlockFace(XPOS, glm::vec3(1, 0, 0), VertexData(glm::vec4(1,0,1,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,0,0,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(1,1,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(XNEG, glm::vec3(-1, 0, 0), VertexData(glm::vec4(0.875,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0.875,0,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0.875,1,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0.875,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZPOS, glm::vec3(0, 0, 1), VertexData(glm::vec4(0.4375,0,0.5625,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1.4375,0,0.5625,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1.4375,1,0.5625,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0.4375,1,0.5625,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZNEG, glm::vec3(0, 0, -1), VertexData(glm::vec4(1.4375,0,0.4375,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0.4375,0,0.4375,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0.4375,1,0.4375,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(1.4375,1,0.4375,1), glm::vec2(0,BLK_UV))),
};

const static std::array<BlockFace, 4> leverOffFaces {
    BlockFace(XPOS, glm::vec3(1, 0, 0), VertexData(glm::vec4(0.125,0,1,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0.125,0,0,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0.125,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0.125,1,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(XNEG, glm::vec3(-1, 0, 0), VertexData(glm::vec4(0,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0,0,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0,1,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZPOS, glm::vec3(0, 0, 1), VertexData(glm::vec4(-0.4375,0,0.5625,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0.5625,0,0.5625,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0.5625,1,0.5625,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(-0.4375,1,0.5625,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZNEG, glm::vec3(0, 0, -1), VertexData(glm::vec4(0.5625,0,0.4375,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(-0.4375,0,0.4375,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(-0.4375,1,0.4375,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0.5625,1,0.4375,1), glm::vec2(0,BLK_UV))),
};

const static std::array<BlockFace, 4> torchFaces {
    BlockFace(XPOS, glm::vec3(1, 0, 0), VertexData(glm::vec4(0.5625,0,1,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0.5625,0,0,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0.5625,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0.5625,1,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(XNEG, glm::vec3(-1, 0, 0), VertexData(glm::vec4(0.4375,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0.4375,0,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0.4375,1,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0.4375,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZPOS, glm::vec3(0, 0, 1), VertexData(glm::vec4(0,0,0.5625,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,0,0.5625,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,1,0.5625,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,1,0.5625,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZNEG, glm::vec3(0, 0, -1), VertexData(glm::vec4(1,0,0.4375,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0,0,0.4375,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0,1,0.4375,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(1,1,0.4375,1), glm::vec2(0,BLK_UV))),
};

const static std::array<BlockFace, 6> adjacentFaces {
    BlockFace(XPOS, glm::vec3(1, 0, 0), VertexData(glm::vec4(1,0,1,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,0,0,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(1,1,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(XNEG, glm::vec3(-1, 0, 0), VertexData(glm::vec4(0,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0,0,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0,1,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(YPOS, glm::vec3(0, 1, 0), VertexData(glm::vec4(0,1,1,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,1,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,1,0,1), glm::vec2(0,BLK_UV))),

    BlockFace(YNEG, glm::vec3(0, -1, 0), VertexData(glm::vec4(0,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,0,0,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,0,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,0,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZPOS, glm::vec3(0, 0, 1), VertexData(glm::vec4(0,0,1,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(1,0,1,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(1,1,1,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(0,1,1,1), glm::vec2(0,BLK_UV))),

    BlockFace(ZNEG, glm::vec3(0, 0, -1), VertexData(glm::vec4(1,0,0,1), glm::vec2(0,0)),
                                        VertexData(glm::vec4(0,0,0,1), glm::vec2(BLK_UV,0)),
                                        VertexData(glm::vec4(0,1,0,1), glm::vec2(BLK_UV, BLK_UV)),
                                        VertexData(glm::vec4(1,1,0,1), glm::vec2(0,BLK_UV))),
};
