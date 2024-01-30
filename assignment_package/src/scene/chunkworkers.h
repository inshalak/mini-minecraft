#pragma once

#include "scene/chunk.h"
#include "QtCore/QMutex"
#include "QtCore/QRunnable"

int getGrasslandHeight(int x, int z);

int getMountainHeight(int x, int z);

int getSandHeight(int x, int z);

float perlinNoise(glm::vec2 uv);

float surflet(glm::vec2 p, glm::vec2 gridPoint);

float worleyNoise(glm::vec2 uv);

float interpolate(float a);

float randomNoise(int a);

float remap(float a, float b, float c, float d, float e);

void fillBlock(Chunk *c, int x, int z);

float smoothstep(float a, float b, float t);

//lerp function

float lerp(float a, float b, float t);

//standard perlinNoise function

float PerlinNoise(glm::vec2 uv);

//2D perlin noise

float perlin_noise_2d(float x, float y);

class FBMWorker : public QRunnable {
private:
    int x, z;
    std::vector<Chunk*> chunks;
    std::unordered_set<Chunk*> &chunksWithBlockData;
    QMutex &chunksWithBlockDataMutex;

public:
    FBMWorker(int x, int z,
              std::vector<Chunk*> chunks,
              std::unordered_set<Chunk*> &chunksWithBlockData,
              QMutex &chunksWithBlockDataMutex);

    void run() override;
};

class VBOWorker : public QRunnable {
private:
    Chunk *chunk;
    std::vector<ChunkVBOData> *chunksWithVBOs;
    QMutex *chunksWithVBOsMutex;

public:
    VBOWorker(Chunk *chunk, std::vector<ChunkVBOData> *chunksWithVBOs, QMutex *chunksWithVBOsMutex);

    void run() override;
};
