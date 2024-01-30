#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"
#include <QObject>
#include <QtMultimedia>

class Player : public Entity {
private:
    glm::vec3 m_velocity, m_acceleration;

    Camera m_camera;
    const Terrain &mcr_terrain;
    const double m_maxSpeed = 20;

    void processInputs(InputBundle &inputs);
    void computePhysics(float dT, const Terrain &terrain);

public:
    // Readonly public reference to our camera
    // for easy access from MyGL
    const Camera& mcr_camera;

    bool flightMode;
    bool waterMode;

    QSoundEffect* swimming;
    QSoundEffect* thunk;

    Player(glm::vec3 pos, const Terrain &terrain);
    virtual ~Player() override;

    void setCameraWidthHeight(unsigned int w, unsigned int h);

    void tick(float dT, InputBundle &input) override;

    // Player overrides all of Entity's movement
    // functions so that it transforms its camera
    // by the same amount as it transforms itself.
    void moveAlongVector(glm::vec3 dir) override;
    void moveForwardLocal(float amount) override;
    void moveRightLocal(float amount) override;
    void moveUpLocal(float amount) override;
    void moveForwardGlobal(float amount) override;
    void moveRightGlobal(float amount) override;
    void moveUpGlobal(float amount) override;
    void rotateOnForwardLocal(float degrees) override;
    void rotateOnRightLocal(float degrees) override;
    void rotateOnUpLocal(float degrees) override;
    void rotateOnForwardGlobal(float degrees) override;
    void rotateOnRightGlobal(float degrees) override;
    void rotateOnUpGlobal(float degrees) override;

    // For sending the Player's data to the GUI
    // for display
    QString posAsQString() const;
    QString velAsQString() const;
    QString accAsQString() const;
    QString lookAsQString() const;

    void collideWithTerrain(glm::vec3& playerPos, glm::vec3 velocity, const Terrain& terrain);
    bool gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::vec3 *out_blockHit, BlockType *out_blocktype);
    bool gridMarchBlockBefore(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::vec3 *out_blockHit);

    void editBlock(InputBundle &inputs, Terrain &terrain, bool mode, BlockType b, int& m_grass, int& m_dirt, int& m_stone, int& m_water, int& m_snow,int& m_lava);


};
