#include "chunkworkers.h"
#include <iostream>

//
///  MILESTONE 1 : PROCEDURAL TERRAIN GEN.
///
//


// MILESTONE 2: PROC GEN CAVES

//random3 from Lecture
glm::vec3 random3( glm::vec3 p ) {
    return glm::fract(glm::sin(glm::vec3(glm::dot(p, glm::vec3(127.1, 311.7, 486.23)),
                          glm::dot(p, glm::vec3(269.5, 183.3, 332.3)),
                          glm::dot(p, glm::vec3(420.6, 631.2, 975.2))
                    )) * 43758.5453f);
}

//surflet from lecture 3D
float surflet3D(glm::vec3 p, glm::vec3 gridPoint) {
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    glm::vec3 t2 = glm::abs(p - gridPoint);
    glm::vec3 t = glm::vec3(1.f) - 6.f * glm::pow(t2, glm::vec3(5.f)) + 15.f * glm::pow(t2, glm::vec3(4.f)) - 10.f * glm::pow(t2, glm::vec3(3.f));
    // Get the random vector for the grid point (assume we wrote a function random2
    // that returns a vec2 in the range [0, 1])
    glm::vec3 gradient = random3(gridPoint) * 2.f - glm::vec3(1., 1., 1.);
    // Get the vector from the grid point to P
    glm::vec3 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y * t.z;
}
//perlinNoise3D from lecture
float perlinNoise3D(glm::vec3 p) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            for(int dz = 0; dz <= 1; ++dz) {
                surfletSum += surflet3D(p, glm::floor(p) + glm::vec3(dx, dy, dz));
            }
        }
    }
    return surfletSum;
}


// Linear interpolation between a and b, with t in the range [0, 1]
float lerp(float a, float b, float t) {
    return a * (1 - t) + b * t;
}

// Smoothly interpolate between a and b, with t in the range [0, 1]
float smoothstep(float a, float b, float t) {
    return lerp(a, b, t * t * (3 - 2 * t));
}

//perlinNoise from lecture
float perlinNoise(glm::vec2 uv) {
    float surfletSum = 0.f;
    for (int x = 0; x <= 1; ++x) {
        for (int y = 0; y <=1; ++y) {
            float sf = surflet(uv, glm::floor(uv) + glm::vec2(x, y));
            surfletSum += sf;
        }
    }
    return surfletSum;
}

//random2 from Lecture
glm::vec2 random2(glm::vec2 p) {
    return glm::fract(glm::sin(glm::vec2(glm::dot(p, glm::vec2(127.1, 311.7)),
                      glm::dot(p, glm::vec2(269.5, 183.3)))) * 43758.5453f);
}

float remap(float val, float from1, float to1, float from2, float to2) {
    return (val - from1) / (to1 - from1) * (to2 - from2) + from2;
}

//Perlin Noise and FBM implementation for SHARDY mountains
int getMountainHeight(int x, int z) {
    float frequency = 0.015;
    float amplitude = 1.0;
    float persistence = 0.5;
    int octaves = 4;

    float perlin = 0.0;
    float totalAmplitude = 0.0;

    for (int i = 0; i < octaves; i++) {
        perlin += amplitude * perlinNoise(glm::vec2(x * frequency, z * frequency));
        totalAmplitude += amplitude;
        frequency *= 2.0;
        amplitude *= persistence;
    }

    perlin /= totalAmplitude;

    perlin = remap(perlin, -1, 1, 0, 1);
    perlin = glm::smoothstep(0.15, 0.85, (double) perlin);
    perlin = pow(perlin, 2.18);
    int height = perlin * (145) + 127;
    return height;

}


//surflet from lecture
float surflet(glm::vec2 p, glm::vec2 gridPoint) {
    glm::vec2 t2 = glm::abs(p - gridPoint);
    glm::vec2 t = glm::vec2(1.f) - (6.f * glm::pow(t2, glm::vec2(5.f)) -
                                    15.f * glm::pow(t2, glm::vec2(4.f)) +
                                    10.f * glm::pow(t2, glm::vec2(3.f)));
    glm::vec2 rand = random2(gridPoint);
    rand[0] = remap(rand[0], 0, 1, -1, 1);
    rand[1] = remap(rand[1], 0, 1, -1, 1);
    glm::vec2 gradient = random2(gridPoint) * 2.f - glm::vec2(1, 1);
    glm::vec2 diff = p - gridPoint;
    float height = glm::dot(diff, gradient);
    return height * t[0] * t[1];
}


int getGrasslandHeight(int x, int z) {
    float worley = worleyNoise(glm::vec2(x / 64.f, z / 64.f));
    return 129 + (worley) * 127 / 1.5 + 5;
}


float worleyNoise(glm::vec2 uv) {
    uv *= 2;
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::fract(uv);
    float minDist = 1;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            glm::vec2 neighbor = glm::vec2(1.0f * i, 1.0f * j);
            glm::vec2 point = random2(uvInt + neighbor);
            glm::vec2 diff = neighbor + point - uvFract;
            float dist = glm::length(diff);
            minDist = glm::min(minDist, dist);
        }
    }
    float perlin = perlinNoise(uv);
    minDist = abs(perlin) * minDist;
    return minDist;
}

// 1D noise function for fun
float randomNoise(int x) {
    x = (x << 13) ^ x;
    return ((int) 1.0 - (x * (x * x * 15731 + 789221)
                    + 1376312589) & 0x7fffffff) / 10737741824.0;
}


float interpolate(float x) {
    float intx = glm::floor(x);
    float fractx = glm::fract(x);
    float v1 = randomNoise(intx);
    float v2 = randomNoise(intx+1);
    return glm::mix(v1, v2, fractx);
}


void fillBlock(Chunk *c, int x, int z) {
    glm::ivec2 worldCoord = c->getCoords();
    int worldX = x + worldCoord.x;
    int worldZ = z + worldCoord.y;
    int mountainHeight = getMountainHeight(worldX, worldZ);
    int grassHeight = getGrasslandHeight(worldX, worldZ);
    float t = perlinNoise(glm::vec2(worldX / 128.f, worldZ / 128.f));
    float biomeType = remap(t, -1, 1, 0, 1);
    biomeType = glm::smoothstep(0.4, 0.6, (double) biomeType);
    int lerped = lerp(grassHeight, mountainHeight, biomeType);
    lerped = fmax(130, lerped);
    // set biomeType to grassland based off of smooth lerp
    if (biomeType < 0.7) {
        for (int y = 1; y < lerped; ++y) {
            if (y == lerped - 1 && y >139) {
                c->setBlockAt(x, y, z, GRASS);
            } else if (y <= 128) {

                c->setBlockAt(x, y, z, STONE);
            }
            else {
                c->setBlockAt(x, y, z, DIRT);
            }
        }
    } else { // mountain
        for (int y = 1; y < lerped; ++y) {
            if (y == lerped - 1 && lerped > 200) {
                c->setBlockAt(x, y, z, SNOW);
            } else if (y == lerped - 1) {
                c->setBlockAt(x, y, z, STONE);
            } else if (y <= 128) {

                c->setBlockAt(x, y, z, STONE);
            } else {
                c->setBlockAt(x, y, z, STONE);
            }

        }
    }

    // set bedrock + caves
    c->setBlockAt(x, 50, z, BEDROCK);
    for (int y = 50; y <= 128; y++){
        float p = perlinNoise3D(glm::vec3(worldX / 38.f, y / 68.f, worldZ / 48.f));
                   if (p < 0) {
                       if (y < 25){
                           c->setBlockAt(x,y,z, LAVA);
                       } else {
                           c->setBlockAt(x,y,z, EMPTY);
                       }
                   }
    }

    for ( int y = 128; y < 139; y++) {
        if (c->getBlockAt(x, y, z) == EMPTY) {
            c->setBlockAt(x,y,z, WATER);
        }
    }

}

FBMWorker::FBMWorker(int x,
                     int z,
                     std::vector<Chunk*> chunks,
                     std::unordered_set<Chunk*> &chunksWithBlockData,
                     QMutex &chunksWithBlockDataMutex)
    : x(x), z(z), chunks(chunks),
      chunksWithBlockData(chunksWithBlockData),
      chunksWithBlockDataMutex(chunksWithBlockDataMutex)
{}


void FBMWorker::run() {
    for (Chunk *c : chunks) {
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                fillBlock(c, x, z);
            }
        }

        chunksWithBlockDataMutex.lock();
        chunksWithBlockData.insert(c);
        chunksWithBlockDataMutex.unlock();
    }
}

VBOWorker::VBOWorker(Chunk *chunk, std::vector<ChunkVBOData> *chunksWithVBOs, QMutex *chunksWithVBOsMutex)
    : chunk(chunk), chunksWithVBOs(chunksWithVBOs), chunksWithVBOsMutex(chunksWithVBOsMutex)
{}

void VBOWorker::run() {
    ChunkVBOData chunkData(chunk);

    Chunk::buildVBODataForChunk(chunk, &chunkData);

    chunksWithVBOsMutex->lock();
    chunksWithVBOs->push_back(chunkData);
    chunksWithVBOsMutex->unlock();
}
