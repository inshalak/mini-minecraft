#pragma once
#include "smartpointerhelp.h"
#include "drawable.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include "chunkhelpers.h"


//using namespace std;

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

class Chunk;

struct ChunkVBOData {
    Chunk *c;
    std::vector<float> vboDataOpaque, vboDataTransparent;
    std::vector<GLuint> idxDataOpaque, idxDataTransparent;

    ChunkVBOData(Chunk *c)
        : c(c), vboDataOpaque{}, vboDataTransparent{}, idxDataOpaque{}, idxDataTransparent{}
    {}
};

class Chunk : public Drawable {
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;

    int chunkX, chunkZ;
    int countOpq;

    ChunkVBOData vboData;

public:
    Chunk(OpenGLContext* mp_context, int x, int y);
    void createVBOdata() override;
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);
    glm::ivec2 getCoords();

    void createVBOdata(const std::vector<float> &vboDataOpaque, const std::vector<GLuint> &idxDataOpaque,
                       const std::vector<float> &vboDataTransparent, const std::vector<GLuint> &idxDataTransparent);
    static void buildVBODataForChunk(Chunk *chunk, ChunkVBOData *chunkData);

    friend class Terrain;
    friend class FBMWorker;
    friend class VBOWorker;
};
