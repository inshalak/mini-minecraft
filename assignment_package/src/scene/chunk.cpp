#include "chunk.h"
#include "chunkhelpers.h"
#include <iostream>


Chunk::Chunk(OpenGLContext* context, int x, int y)
    : Drawable(context),
      m_blocks(), m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}},
      chunkX(x), chunkZ(y), vboData(this)
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    if (y < 0 || y > 255) return EMPTY;
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}


const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

bool crossesBorder(glm::ivec3 pos, glm::ivec3 dir) {
    return pos.x + dir.x < 0 || pos.x + dir.x >= 16 || pos.z + dir.z < 0 || pos.z + dir.z >= 16;
}

void appendVBOData(std::vector<float> &vboData, std::vector<GLuint> &idxData,
                   const BlockFace &f, BlockType curr, glm::ivec3 xyz,
                   unsigned int &maxIdx,
                   float world_x, float world_z) {
    int x = xyz.x;
    int y = xyz.y;
    int z = xyz.z;

    const std::array<VertexData, 4> &vertData = f.vertices;
    const std::array<glm::vec2, 4> offset =  {glm::vec2{0.f, 0.f}, glm::vec2{1.f, 0.f}, glm::vec2{1.f, 1.f}, glm::vec2{0.f, 1.f}};

    int i = 0;
    for (const VertexData &vd : vertData){
        // position
        vboData.push_back(x + vd.pos.x + world_x);
        vboData.push_back(y + vd.pos.y);
        vboData.push_back(z + vd.pos.z + world_z);
        vboData.push_back(vd.pos.w);
        // normal
        vboData.push_back(f.directionVec.x);
        vboData.push_back(f.directionVec.y);
        vboData.push_back(f.directionVec.z);
        vboData.push_back(0.f);
        // milestone 2 -- testing
        switch(curr) {
        case GRASS:
            if (f.directionVec.y == 1) {
               //Top face:
                vboData.push_back((8.f + offset[i].x)/16.f);
                vboData.push_back((13.f + offset[i].y)/16.f);
            } else if (f.directionVec.y == -1){
               //Bottom:
                vboData.push_back((2.f + offset[i].x)/16.f);
                vboData.push_back((15.f + offset[i].y)/16.f);
            } else {
                //Side:
                vboData.push_back((3.f+ offset[i].x)/16.f);
                vboData.push_back((15.f+ offset[i].y)/16.f);
            }
            vboData.push_back(0.0f);
            break;
        case DIRT:
            vboData.push_back((2.f + offset[i].x)/16.f);
            vboData.push_back((15.f + offset[i].y)/16.f);
            vboData.push_back(0.0f);
            break;
        case STONE:
            vboData.push_back((1.f + offset[i].x)/16.f);
            vboData.push_back((15.f + offset[i].y)/16.f);
            vboData.push_back(0.0f);
            break;
        case WATER:
            vboData.push_back((13.f + offset[i].x)/16.f);
            vboData.push_back((3.f + offset[i].y)/16.f);
            vboData.push_back(1.0f);
            break;
        case LAVA:
            vboData.push_back((13.f + offset[i].x)/16.f);
            vboData.push_back((1.f + offset[i].y)/16.f);
            vboData.push_back(1.0f);
            break;
        case BEDROCK:
            vboData.push_back((1.f + offset[i].x)/16.f);
            vboData.push_back((14.f + offset[i].y)/16.f);
            vboData.push_back(0.0f);
            break;
        case SNOW:
            vboData.push_back((2.f + offset[i].x)/16.f);
            vboData.push_back((11.f + offset[i].y)/16.f);
            vboData.push_back(0.0f);
            break;
        // redstone block types
        case REDSTONE_TORCH_ON:
            vboData.push_back((3.f + offset[i].x)/16.f);
            vboData.push_back((9.f + offset[i].y)/16.f);
            vboData.push_back(0.0f);
            break;
        case REDSTONE_TORCH_OFF:
            vboData.push_back((3.f + offset[i].x)/16.f);
            vboData.push_back((8.f + offset[i].y)/16.f);
            vboData.push_back(0.0f);
            break;
        case REDSTONE_LEVER_ON:
        case REDSTONE_LEVER_OFF:
            vboData.push_back((0.f + offset[i].x) / 16.f);
            vboData.push_back((9.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case REDSTONE_LAMP_ON:
            vboData.push_back((4.f + offset[i].x) / 16.f);
            vboData.push_back((2.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case REDSTONE_LAMP_OFF:
            vboData.push_back((3.f + offset[i].x) / 16.f);
            vboData.push_back((2.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case REDSTONE_WIRE_ON:
            vboData.push_back((1.f + offset[i].x) / 16.f);
            vboData.push_back((7.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case REDSTONE_WIRE_OFF:
            vboData.push_back((1.f + offset[i].x) / 16.f);
            vboData.push_back((1.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case SPRUCE_SAPLING:
            vboData.push_back((15.f + offset[i].x) / 16.f);
            vboData.push_back((12.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case ROSE:
            vboData.push_back((12.f + offset[i].x) / 16.f);
            vboData.push_back((15.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case DAF:
            vboData.push_back((13.f + offset[i].x) / 16.f);
            vboData.push_back((15.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case REDSHROOM:
            vboData.push_back((12.f + offset[i].x) / 16.f);
            vboData.push_back((14.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case SHROOM:
            vboData.push_back((13.f + offset[i].x) / 16.f);
            vboData.push_back((14.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case DRY_SPRIG:
            vboData.push_back((7.f + offset[i].x) / 16.f);
            vboData.push_back((12.f + offset[i].y) / 16.f);
            vboData.push_back(0.f);
            break;
        case CACTUS:
            if (f.directionVec.y == 1 || f.directionVec.y == -1) {
                vboData.push_back((5.f + offset[i].x)/16.f);
                vboData.push_back((11.f + offset[i].y)/16.f);
            } else {
                vboData.push_back((6.f+ offset[i].x)/16.f);
                vboData.push_back((11.f+ offset[i].y)/16.f);
            }
            vboData.push_back(0.0f);
            break;
        default:
            // Other block types are not yet handled, so we default to debug purple
            vboData.push_back(7.f/16.f);
            vboData.push_back(1.f/16.f);
            vboData.push_back(0.0f);
            break;
        }

        idxData.push_back(0 + maxIdx);
        idxData.push_back(1 + maxIdx);
        idxData.push_back(2 + maxIdx);
        idxData.push_back(0 + maxIdx);
        idxData.push_back(2 + maxIdx);
        idxData.push_back(3 + maxIdx);
        maxIdx += 4;
        i++;
    }
}

void Chunk::createVBOdata() {
    std::vector<float> vboData;
    std::vector<GLuint> idxData;
    unsigned int maxIdx = 0;

    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 256; y++) {
            for (int z = 0; z < 16; z++) {
                BlockType curr = getBlockAt(x, y, z);
                if (curr != EMPTY) {
                    for (const BlockFace &f : adjacentFaces) {
                        BlockType adj;
                        if (crossesBorder(glm::ivec3(x, y, z), glm::ivec3(f.directionVec))) {
                            Chunk *neighbor = m_neighbors[f.direction];
                            if (neighbor == nullptr) {
                                adj = EMPTY;
                            } else {
                                int dx = (x + (int) f.directionVec.x) % 16;
                                if (dx < 0) dx += 16;
                                int dy = (y + (int) f.directionVec.y) % 256;
                                if (dy < 0) dy += 256;
                                int dz = (z + (int) f.directionVec.z) % 16;
                                if (dz < 0) dz += 16;
                                adj = neighbor->getBlockAt(dx, dy, dz);
                            }
                        } else {
                            adj = getBlockAt(
                                        x + (int) f.directionVec.x,
                                        y + (int) f.directionVec.y,
                                        z + (int) f.directionVec.z
                                    );
                        }

                        if (adj == EMPTY) {
                            appendVBOData(vboData, idxData, f, curr, glm::ivec3(x,y,z), maxIdx, chunkX, chunkZ);
                        }
                    }
                }
            }
        }
    }

    m_count = idxData.size();

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxData.size() * sizeof(GLuint), idxData.data(), GL_STATIC_DRAW);

    generateInterleaved();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleaved);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(float), vboData.data(), GL_STATIC_DRAW);
}

inline bool isTransparent(BlockType b) {
    return b == WATER || b == LAVA;
}

inline bool isTorchType(BlockType b) {
    return b == REDSTONE_TORCH_ON || b == REDSTONE_TORCH_OFF;
}

inline bool isFlowerType(BlockType b) {
    return b == SPRUCE_SAPLING || b == ROSE || b == DAF || b == REDSHROOM || b == SHROOM || b == DRY_SPRIG;
}

inline bool drawAnyways(BlockType b) {
    return isTorchType(b) || isFlowerType(b) || b == REDSTONE_LEVER_OFF || b == REDSTONE_LEVER_ON || b == CACTUS;
}

void Chunk::buildVBODataForChunk(Chunk *c, ChunkVBOData *chunkData) {
    unsigned int maxIdxOpq = 0;
    unsigned int maxIdxTra = 0;
    for (int x = 0; x < 16; x++) {
        for (int y = 0; y < 256; y++) {
            for (int z = 0; z < 16; z++) {
                BlockType curr = c->getBlockAt(x, y, z);

                if (curr != EMPTY && isTorchType(curr)) {

                    for (const BlockFace &f : torchFaces) {
                        appendVBOData(chunkData->vboDataTransparent, chunkData->idxDataTransparent, f, curr, glm::ivec3(x,y,z), maxIdxTra, c->chunkX, c->chunkZ);
                    }

                } else if (curr == REDSTONE_LEVER_OFF) {

                    for (const BlockFace &f : leverOffFaces) {
                        appendVBOData(chunkData->vboDataTransparent, chunkData->idxDataTransparent, f, curr, glm::ivec3(x,y,z), maxIdxTra, c->chunkX, c->chunkZ);
                    }

                } else if (curr == REDSTONE_LEVER_ON) {

                    for (const BlockFace &f : leverOnFaces) {
                        appendVBOData(chunkData->vboDataTransparent, chunkData->idxDataTransparent, f, curr, glm::ivec3(x,y,z), maxIdxTra, c->chunkX, c->chunkZ);
                    }

                } else if (isFlowerType(curr)) {

                    for (const BlockFace &f : flowerFaces) {
                        appendVBOData(chunkData->vboDataTransparent, chunkData->idxDataTransparent, f, curr, glm::ivec3(x,y,z), maxIdxTra, c->chunkX, c->chunkZ);
                    }

                } else if (curr == CACTUS) {

                    for (const BlockFace &f : cactusFaces) {
                        appendVBOData(chunkData->vboDataTransparent, chunkData->idxDataTransparent, f, curr, glm::ivec3(x,y,z), maxIdxTra, c->chunkX, c->chunkZ);
                    }

                } else if (curr != EMPTY) {

                    for (const BlockFace &f : adjacentFaces) {
                        BlockType adj;
                        if (crossesBorder(glm::ivec3(x, y, z), glm::ivec3(f.directionVec))) {
                            Chunk *neighbor = c->m_neighbors[f.direction];
                            if (neighbor == nullptr) {
                                adj = EMPTY;
                            } else {
                                int dx = (x + (int) f.directionVec.x) % 16;
                                if (dx < 0) dx += 16;
                                int dy = (y + (int) f.directionVec.y) % 256;
                                if (dy < 0) dy += 256;
                                int dz = (z + (int) f.directionVec.z) % 16;
                                if (dz < 0) dz += 16;
                                adj = neighbor->getBlockAt(dx, dy, dz);
                            }
                        } else {
                            adj = c->getBlockAt(
                                        x + (int) f.directionVec.x,
                                        y + (int) f.directionVec.y,
                                        z + (int) f.directionVec.z
                                    );
                        }

                        if (isTransparent(curr) && adj == EMPTY) {
                            appendVBOData(chunkData->vboDataTransparent, chunkData->idxDataTransparent, f, curr, glm::ivec3(x,y,z), maxIdxTra, c->chunkX, c->chunkZ);
                        } else if (adj == EMPTY || ((isTransparent(adj) || drawAnyways(adj)) && !isTransparent(curr))) {
                            appendVBOData(chunkData->vboDataOpaque, chunkData->idxDataOpaque, f, curr, glm::ivec3(x,y,z), maxIdxOpq, c->chunkX, c->chunkZ);
                        }
                    }

                }
            }
        }
    }

    c->m_countOpq = chunkData->idxDataOpaque.size();
    c->m_countTra = chunkData->idxDataTransparent.size();
}

void Chunk::createVBOdata(const std::vector<float> &vboDataOpaque, const std::vector<GLuint> &idxDataOpaque, const std::vector<float> &vboDataTransparent, const std::vector<GLuint> &idxDataTransparent) {
    generateIdxOpq();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxOpq);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxDataOpaque.size() * sizeof(GLuint), idxDataOpaque.data(), GL_STATIC_DRAW);

    generateInterleavedOpq();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleavedOpq);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vboDataOpaque.size() * sizeof(float), vboDataOpaque.data(), GL_STATIC_DRAW);

    generateIdxTra();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxTra);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxDataTransparent.size() * sizeof(GLuint), idxDataTransparent.data(), GL_STATIC_DRAW);

    generateInterleavedTra();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleavedTra);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vboDataTransparent.size() * sizeof(float), vboDataTransparent.data(), GL_STATIC_DRAW);
}

glm::ivec2 Chunk::getCoords() {
    return glm::ivec2(chunkX, chunkZ);
}
