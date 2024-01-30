#pragma once

#include "glm/glm.hpp"
#include "scene/chunkhelpers.h"

class RedstoneItem {
protected:
    glm::ivec3 pos;
    bool currentState;
    bool stateChanged;
    bool updatedThisCycle;
    std::array<RedstoneItem*, 6> neighbors;

    void setupBaseClassMembers(glm::ivec3 pos, bool currentState);

public:
    virtual void update(int updateDir) = 0;

    bool getState();
    bool stateHasChanged();
    void changeState();

    void clearUpdateStatus();

    int getXPos();
    int getYPos();
    int getZPos();

    RedstoneItem *getNeighbor(Direction d);
    void setNeighbor(Direction d, RedstoneItem* neighbor);
    void disconnectNeighbor(Direction d);

    void displayNeighbors();
};

class RedstoneTorch : public RedstoneItem {
public:
    RedstoneTorch(glm::ivec3 &pos);

    void update(int updateDir) override;
    void checkBelow();
};

class RedstoneWire : public RedstoneItem {
public:
    RedstoneWire(glm::ivec3 &pos);

    void update(int updateDir) override;

    void turnOff();
};

class RedstoneLever : public RedstoneItem {
public:
    RedstoneLever(glm::ivec3 &pos);

    void update(int updateDir) override;

    void toggle();
};

class RedstoneLamp : public RedstoneItem {
public:
    RedstoneLamp(glm::ivec3 &pos);

    void update(int updateDir) override;

    void turnOff();
};
