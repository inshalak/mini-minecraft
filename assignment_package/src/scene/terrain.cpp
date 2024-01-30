#include "terrain.h"
#include "scene/chunkworkers.h"
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <random>
#include <QtCore/QThreadPool>

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generatedTerrain(),
      m_chunksWithVBOsMutex(), m_chunksWithVBOs{},
      m_chunksWithBlockDataMutex(), m_chunksWithBlockData{},
      redstoneItems{}, redstoneSources{},
      mp_context(context)
{}

Terrain::~Terrain() {
    for (auto &chunk : m_chunks) {
        chunk.second->destroyVBOdata();
    }
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}

glm::vec2 chunkCornerPos(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return glm::vec2(16 * xFloor, 16 * zFloor);
}

uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}


void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

void Terrain::redrawZoneEdgeChunks(Chunk *c) {
    int64_t id = toKey(floor(c->chunkX / 64.f) * 64,
                       floor(c->chunkZ / 64.f) * 64);

    if (m_generatedTerrain.count(id)) {
        m_chunksWithBlockDataMutex.lock();
        m_chunksWithBlockData.insert(c);
        m_chunksWithBlockDataMutex.unlock();
    }
}

Chunk* Terrain::instantiateChunkAt(int x, int z) {
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context, x, z);
    Chunk *cPtr = chunk.get();
    m_chunks[toKey(x, z)] = std::move(chunk);
    // Set the neighbor pointers of itself and its neighbors
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
        redrawZoneEdgeChunks(chunkNorth.get());
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
        redrawZoneEdgeChunks(chunkSouth.get());
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
        redrawZoneEdgeChunks(chunkEast.get());
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
        redrawZoneEdgeChunks(chunkWest.get());
    }
    return cPtr;
}

class ChunkDistanceFromPlayerChunk {
private:
    const glm::vec2 chunkPos;

public:
    ChunkDistanceFromPlayerChunk(const glm::vec3 &playerPos)
        : chunkPos({glm::floor(playerPos.x / 16.f) * 16.f,
                    glm::floor(playerPos.z / 16.f) * 16.f})
    {}

    bool operator() (Chunk *a, Chunk *b) {
        return glm::distance(glm::vec2(a->getCoords()), chunkPos) > glm::distance(glm::vec2(b->getCoords()), chunkPos);
    }
};

// TODO: When you make Chunk inherit from Drawable, change this code so
// it draws each Chunk with the given ShaderProgram, remembering to set the
// model matrix to the proper X and Z translation!
// instead, we should first create when we enter -- spawnblocktype, draw, multithread
// do worker stuff only once - initial stuff
// expand function
// destroy vbo data for anything that is not shone
// checks every zone that is previously rendered
// if zone does not exist place blacks
// create blocks for any zones w/o vbo data but has block type data
// only call initial create the first time you join
// expand only every 10 ticks
void Terrain::draw(const glm::vec3 &playerPos, ShaderProgram *shaderProgram) {
    glm::ivec2 currZone { 64.f * glm::floor(playerPos.x / 64.f), 64.f * glm::floor(playerPos.z / 64.f) };
    QSet<int64_t> terrainZonesToDraw = terrainZonesBorderingZone(currZone, TERRAIN_DRAW_RADIUS, false);

    for (int64_t id : terrainZonesToDraw) {
        glm::ivec2 coord = toCoords(id);
        for (int x = coord.x; x < coord.x + 64; x += 16) {
            for (int z = coord.y; z < coord.y + 64; z += 16) {
                if (hasChunkAt(x, z)) {
                    uPtr<Chunk> &c = getChunkAt(x, z);
                    shaderProgram->setModelMatrix(glm::mat4(1.0));
                    shaderProgram->drawOpaque(*(c.get()));
                }
            }
        }
    }

    std::vector<Chunk*> chunkTransparentToDraw {};
    for (int64_t id : terrainZonesToDraw) {
        glm::ivec2 coord = toCoords(id);
        for (int x = coord.x; x < coord.x + 64; x += 16) {
            for (int z = coord.y; z < coord.y + 64; z += 16) {
                if (hasChunkAt(x, z)) {
                    uPtr<Chunk> &c = getChunkAt(x, z);
                    chunkTransparentToDraw.push_back(c.get());
                }
            }
        }
    }
    ChunkDistanceFromPlayerChunk comparator = ChunkDistanceFromPlayerChunk(playerPos);
    std::sort(chunkTransparentToDraw.begin(), chunkTransparentToDraw.end(), comparator);
    for (Chunk *c : chunkTransparentToDraw) {
        shaderProgram->setModelMatrix(glm::mat4(1.));
        shaderProgram->drawTransparent(*c);
    }
    /*
    for (int64_t id : terrainZonesToDraw) {
        glm::ivec2 coord = toCoords(id);
        for (int x = coord.x; x < coord.x + 64; x += 16) {
            for (int z = coord.y; z < coord.y + 64; z += 16) {
                if (hasChunkAt(x, z)) {
                    uPtr<Chunk> &c = getChunkAt(x, z);
                    shaderProgram->setModelMatrix(glm::mat4(1.0));
                    shaderProgram->drawTransparent(*(c.get()));
                }
            }
        }
    }
    */
}

void Terrain::updateChunk(Chunk *c) {
    glm::ivec2 chunkCoords = c->getCoords();
    m_chunksWithBlockDataMutex.lock();
    m_chunksWithBlockData.insert(getChunkAt(chunkCoords.x, chunkCoords.y).get());
    m_chunksWithBlockData.insert(getChunkAt(chunkCoords.x + 16, chunkCoords.y).get());
    m_chunksWithBlockData.insert(getChunkAt(chunkCoords.x - 16, chunkCoords.y).get());
    m_chunksWithBlockData.insert(getChunkAt(chunkCoords.x, chunkCoords.y + 16).get());
    m_chunksWithBlockData.insert(getChunkAt(chunkCoords.x, chunkCoords.y - 16).get());
    m_chunksWithBlockDataMutex.unlock();
}

void Terrain::spawnVBOWorker(Chunk *c) {
    VBOWorker *worker = new VBOWorker(c, &m_chunksWithVBOs, &m_chunksWithVBOsMutex);
    QThreadPool::globalInstance()->start(worker);
}

void Terrain::spawnVBOWorkers(std::unordered_set<Chunk*> &chunksToGenVBOs) {
    for (Chunk *c : chunksToGenVBOs) {
        spawnVBOWorker(c);
    }
}

void Terrain::spawnFBMWorker(int64_t zoneToGenerate) {
    m_generatedTerrain.insert(zoneToGenerate);
    std::vector<Chunk*> chunksForWorker;
    glm::ivec2 coords = toCoords(zoneToGenerate);
    for (int x = coords.x; x < coords.x + 64; x += 16) {
        for (int z = coords.y; z < coords.y + 64; z += 16) {
            Chunk *c = instantiateChunkAt(x, z);
            c->m_count = 0; // this will need to be changed for the transparency vbo; not sure if it is necessary at all though
            c->m_countOpq = 0;
            c->m_countTra = 0;
            chunksForWorker.push_back(c);
        }
    }
    FBMWorker *worker = new FBMWorker(coords.x,
                                      coords.y,
                                      chunksForWorker,
                                      m_chunksWithBlockData,
                                      m_chunksWithBlockDataMutex);
    QThreadPool::globalInstance()->start(worker);
}

void Terrain::spawnFBMWorkers(std::unordered_set<int64_t> &zonesToGenerate) {
    for (int64_t id : zonesToGenerate) {
        spawnFBMWorker(id);
    }
}

QSet<int64_t> Terrain::terrainZonesBorderingZone(glm::ivec2 zoneCoords, unsigned int radius, bool onlyCircumference) {
    int radiusInZoneScale = static_cast<int>(radius) * 64;
    QSet<int64_t> result {};
    if (onlyCircumference) {
        for (int i = -radiusInZoneScale; i <= radiusInZoneScale; i += 64) {
            result.insert(toKey(zoneCoords.x + radiusInZoneScale, zoneCoords.y + i));
            result.insert(toKey(zoneCoords.x - radiusInZoneScale, zoneCoords.y + i));
            result.insert(toKey(zoneCoords.x + i, zoneCoords.y + radiusInZoneScale));
            result.insert(toKey(zoneCoords.x + i, zoneCoords.y - radiusInZoneScale));
        }
    } else {
        for (int i = -radiusInZoneScale; i <= radiusInZoneScale; i += 64) {
            for (int j = -radiusInZoneScale; j <= radiusInZoneScale; j += 64) {
                result.insert(toKey(zoneCoords.x + i, zoneCoords.y + j));
            }
        }
    }
    return result;
}

void Terrain::checkThreadResults() {
    m_chunksWithBlockDataMutex.lock();
    spawnVBOWorkers(m_chunksWithBlockData);
    m_chunksWithBlockData.clear();
    m_chunksWithBlockDataMutex.unlock();

    m_chunksWithVBOsMutex.lock();
    for (ChunkVBOData &cd : m_chunksWithVBOs) {
        cd.c->createVBOdata(cd.vboDataOpaque, cd.idxDataOpaque, cd.vboDataTransparent, cd.idxDataTransparent);
    }
    m_chunksWithVBOs.clear();
    m_chunksWithVBOsMutex.unlock();
}

void Terrain::expandTerrain(const glm::vec3 &playerPos, const glm::vec3 &playerPosPrev) {
    glm::ivec2 currZone { 64.f * glm::floor(playerPos.x / 64.f), 64.f * glm::floor(playerPos.z / 64.f) };
    glm::ivec2 prevZone { 64.f * glm::floor(playerPosPrev.x / 64.f), 64.f * glm::floor(playerPosPrev.z / 64.f) };

    QSet<int64_t> terrainZonesBorderingCurrPos = terrainZonesBorderingZone(currZone, TERRAIN_CREATE_RADIUS, false);
    QSet<int64_t> terrainZonesBorderingPrevPos = terrainZonesBorderingZone(prevZone, TERRAIN_CREATE_RADIUS, false);

    for (const int64_t &id : terrainZonesBorderingPrevPos) {
        if (!terrainZonesBorderingCurrPos.contains(id)) {
            glm::ivec2 coord = toCoords(id);
            for (int x = coord.x; x < coord.x + 64; x += 16) {
                for (int z = coord.y; z < coord.y + 64; z += 16) {
                    auto &chunk = getChunkAt(x, z);
                    chunk->destroyVBOdata();
                }
            }
        }
    }

    for (const int64_t &id : terrainZonesBorderingCurrPos) {
        if (m_generatedTerrain.count(id)) {
            if (!terrainZonesBorderingPrevPos.contains(id)) {
                glm::ivec2 coord = toCoords(id);
                for (int x = coord.x; x < coord.x + 64; x += 16) {
                    for (int z = coord.y; z < coord.y + 64; z += 16) {
                        spawnVBOWorker(getChunkAt(x, z).get());
                    }
                }
            }
        } else {
            spawnFBMWorker(id);
        }
    }
}

void Terrain::updateRedstone() {
    for (RedstoneItem *i : redstoneSources) {
        if (RedstoneTorch *r = dynamic_cast<RedstoneTorch*>(i); r != nullptr) {
            r->checkBelow();
        }
    }

    for (uPtr<RedstoneItem> &i : redstoneItems) {
        if (RedstoneLamp *r = dynamic_cast<RedstoneLamp*>(i.get()); r != nullptr) {
            r->turnOff();
        }

        if (RedstoneWire *r = dynamic_cast<RedstoneWire*>(i.get()); r != nullptr) {
            r->turnOff();
        }
    }

    for (RedstoneItem *i : redstoneSources) {
        i->update(-1);
    }

    std::unordered_set<int64_t> changedChunks {};

    for (uPtr<RedstoneItem> &i : redstoneItems) {
        if (i->stateHasChanged()) {
            if (RedstoneTorch *r = dynamic_cast<RedstoneTorch*>(i.get()); r != nullptr) {
                if (i->getState()) {
                    setBlockAt(i->getXPos(), i->getYPos(), i->getZPos(), REDSTONE_TORCH_ON);
                } else {
                    setBlockAt(i->getXPos(), i->getYPos(), i->getZPos(), REDSTONE_TORCH_OFF);
                }
            }

            if (RedstoneLamp *r = dynamic_cast<RedstoneLamp*>(i.get()); r != nullptr) {
                if (i->getState()) {
                    setBlockAt(i->getXPos(), i->getYPos(), i->getZPos(), REDSTONE_LAMP_ON);
                } else {
                    setBlockAt(i->getXPos(), i->getYPos(), i->getZPos(), REDSTONE_LAMP_OFF);
                }
            }

            if (RedstoneWire *r = dynamic_cast<RedstoneWire*>(i.get()); r != nullptr) {
                if (i->getState()) {
                    setBlockAt(i->getXPos(), i->getYPos(), i->getZPos(), REDSTONE_WIRE_ON);
                } else {
                    setBlockAt(i->getXPos(), i->getYPos(), i->getZPos(), REDSTONE_WIRE_OFF);
                }
            }

            if (RedstoneLever *r = dynamic_cast<RedstoneLever*>(i.get()); r != nullptr) {
                if (i->getState()) {
                    setBlockAt(i->getXPos(), i->getYPos(), i->getZPos(), REDSTONE_LEVER_ON);
                } else {
                    setBlockAt(i->getXPos(), i->getYPos(), i->getZPos(), REDSTONE_LEVER_OFF);
                }
            }

            changedChunks.insert(toKey(i->getXPos(), i->getZPos()));
        }
    }

    for (int64_t c : changedChunks) {
        updateChunk(getChunkAt(toCoords(c).x, toCoords(c).y).get());
    }
}

bool Terrain::hasRedstoneItemAt(int x, int y, int z) {
    if (hasChunkAt(x, z))
        return redstoneBlocks.count(getBlockAt(x, y, z));
    else
        return false;
}

RedstoneItem *Terrain::getRedstoneItemAt(int x, int y, int z) {
    for (const uPtr<RedstoneItem> &i : redstoneItems) {
        if (i->getXPos() == x && i->getYPos() == y && i->getZPos() == z)
            return i.get();
    }
    throw new std::out_of_range("Tried to access out of bounds redstoneitem");
}

const uPtr<RedstoneItem> &Terrain::getRedstoneUptrItemAt(int x, int y, int z) {
    for (const uPtr<RedstoneItem> &i : redstoneItems) {
        if (i->getXPos() == x && i->getYPos() == y && i->getZPos() == z)
            return i;
    }
    throw new std::out_of_range("Tried to access out of bounds redstoneitem");
}


void Terrain::setRedstoneItemAt(int x, int y, int z, BlockType b) {
    uPtr<RedstoneItem> i;

    glm::ivec3 pos = {x, y, z};
    switch (b) {
    case REDSTONE_TORCH_OFF:
    case REDSTONE_TORCH_ON:
        i = mkU<RedstoneTorch>(pos);
        redstoneSources.insert(i.get());
        break;
    case REDSTONE_LEVER_OFF:
    case REDSTONE_LEVER_ON:
        i = mkU<RedstoneLever>(pos);
        redstoneSources.insert(i.get());
        break;
    case REDSTONE_LAMP_OFF:
    case REDSTONE_LAMP_ON:
        i = mkU<RedstoneLamp>(pos);
        break;
    case REDSTONE_WIRE_OFF:
    case REDSTONE_WIRE_ON:
        i = mkU<RedstoneWire>(pos);
        break;
    default:
        throw new std::out_of_range("Attempting to add non-redstone block type to redstone item list");
    }

    for (const BlockFace &f : adjacentFaces) {
        glm::ivec3 adjPos = pos + glm::ivec3(f.directionVec);
        if (hasRedstoneItemAt(adjPos.x, adjPos.y, adjPos.z)) {
            i->setNeighbor(f.direction, getRedstoneItemAt(adjPos.x, adjPos.y, adjPos.z));
        }
    }

//    i->update();
//    for (const BlockFace &f : adjacentFaces) {
//        RedstoneItem *neighbor = i->getNeighbor(f.direction);
//        if (neighbor) {
//            neighbor->update();
//        }
//    }

    redstoneItems.push_back(std::move(i));
}

void Terrain::removeRedstoneItemAt(int x, int y, int z, BlockType b) {
    const uPtr<RedstoneItem> &toRemove = getRedstoneUptrItemAt(x, y, z);
    redstoneItems.remove(toRemove);

    if (redstoneSources.count(toRemove.get())) {
        redstoneSources.erase(toRemove.get());
    }
}

void Terrain::toggleLever(int x, int y, int z) {
    RedstoneItem *i = getRedstoneItemAt(x, y, z);
    if (RedstoneLever *lever = dynamic_cast<RedstoneLever*>(i); lever != nullptr) {
        lever->toggle();
//        lever->update();
//        for (const BlockFace &f : adjacentFaces) {
//            RedstoneItem *neighbor = lever->getNeighbor(f.direction);
//            if (neighbor) {
//                neighbor->update();
//            }
//        }
    } else {
        throw new std::out_of_range("Attempted to toggle a non-toggleable redstone block");
    }
}

void Terrain::CreateTestScene()
{
    // Create the Chunks that will
    // store the blocks for our
    // initial world space
    for(int x = 0; x < 64; x += 16) {
        for(int z = 0; z < 64; z += 16) {
            instantiateChunkAt(x, z);
        }
    }
    // Tell our existing terrain set that
    // the "generated terrain zone" at (0,0)
    // now exists.
    m_generatedTerrain.insert(toKey(0, 0));

    // Create the basic terrain floor
    for(int x = 0; x < 64; ++x) {
        for(int z = 0; z < 64; ++z) {
            if((x + z) % 2 == 0) {
                setBlockAt(x, 128, z, STONE);
            }
            else {
                setBlockAt(x, 128, z, DIRT);
            }
        }
    }
    // Add "walls" for collision testing
    for(int x = 0; x < 64; ++x) {
        setBlockAt(x, 129, 0, GRASS);
        setBlockAt(x, 130, 0, GRASS);
        setBlockAt(x, 129, 63, GRASS);
        setBlockAt(0, 130, x, GRASS);
    }
    // Add a central column
    for(int y = 129; y < 140; ++y) {
        setBlockAt(32, y, 32, GRASS);
    }
}



