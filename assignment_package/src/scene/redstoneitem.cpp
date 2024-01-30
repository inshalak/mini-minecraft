#include "redstoneitem.h"
#include <iostream>

bool RedstoneItem::getState() {
    return currentState;
}

bool RedstoneItem::stateHasChanged() {
    bool result = stateChanged;
    stateChanged = false;
    return result;
}

int RedstoneItem::getXPos() {
    return pos.x;
}

int RedstoneItem::getYPos() {
    return pos.y;
}

int RedstoneItem::getZPos() {
    return pos.z;
}

void RedstoneItem::clearUpdateStatus() {
    updatedThisCycle = false;
}

int oppositeDirection(int d) {
    switch (d) {
    case XPOS:
        return XNEG;
    case XNEG:
        return XPOS;
    case YPOS:
        return YNEG;
    case YNEG:
        return YPOS;
    case ZPOS:
        return ZNEG;
    case ZNEG:
        return ZPOS;
    case -1:
        return -1;
    }
}

void RedstoneItem::setNeighbor(Direction d, RedstoneItem *neighbor) {
    int oppositeDir = oppositeDirection(d);
    neighbors[d] = neighbor;
    neighbor->neighbors[oppositeDir] = this;
}

RedstoneItem *RedstoneItem::getNeighbor(Direction d) {
    return neighbors[d];
}

void RedstoneItem::displayNeighbors() {
    for (int i = 0; i < 6; i++) {
        if (neighbors[i] != nullptr) {
            std::cout << neighbors[i]->pos.x << ", " << neighbors[i]->pos.y << "\n";
        } else {
            std::cout << "NULL\n";
        }
    }
    std::cout << "\n";
}

void RedstoneItem::setupBaseClassMembers(glm::ivec3 pos, bool currentState) {
    this->pos = pos;
    this->currentState  = currentState;
    this->stateChanged = false;
    for (int i = 0; i < 6; i++) {
        neighbors[i] = nullptr;
    }
}

RedstoneTorch::RedstoneTorch(glm::ivec3 &pos) {
    setupBaseClassMembers(pos, true);
}

void RedstoneTorch::update(int updateDir) {
    for (const BlockFace &f : adjacentFaces) {
        RedstoneItem *n = getNeighbor(f.direction);
        if (n && updateDir != f.direction && f.direction != YNEG) {
            if (RedstoneTorch *t = dynamic_cast<RedstoneTorch*>(n); t != nullptr) continue;
            if (RedstoneLever *l = dynamic_cast<RedstoneLever*>(n); l != nullptr) continue;
            n->update(oppositeDirection(f.direction));
        }
    }
}

void RedstoneTorch::checkBelow() {
    bool newState;
    RedstoneItem *below = neighbors[YNEG];
    if (below && below->getState()) {
        newState = false;
    } else {
        newState = true;
    }

    if (newState != currentState) {
        currentState = newState;
        stateChanged = true;
    }
}

RedstoneWire::RedstoneWire(glm::ivec3 &pos) {
    setupBaseClassMembers(pos, false);
}

void RedstoneWire::update(int updateDir) {
    bool powered = false;
    for (int i = 0; i < 6; i++) {
        if (neighbors[i] && neighbors[i]->getState()) {
            if (RedstoneTorch *t = dynamic_cast<RedstoneTorch*>(neighbors[i]); i == YPOS && t != nullptr) continue;
            powered = true;
        }
    }

    if (currentState != powered) {
        stateChanged = true;
        currentState = powered;

        for (const BlockFace &f : adjacentFaces) {
            RedstoneItem *n = getNeighbor(f.direction);
            if (n && updateDir != f.direction) n->update(oppositeDirection(f.direction));
        }
    }
}

void RedstoneWire::turnOff() {
    if (currentState == true) {
        stateChanged = true;
    }
    currentState = false;
}


RedstoneLever::RedstoneLever(glm::ivec3 &pos) {
    setupBaseClassMembers(pos, false);
}

void RedstoneLever::update(int updateDir) {
    for (const BlockFace &f : adjacentFaces) {
        RedstoneItem *n = getNeighbor(f.direction);
        if (n && updateDir != f.direction) {
            if (RedstoneTorch *t = dynamic_cast<RedstoneTorch*>(n); t != nullptr) continue;
            if (RedstoneLever *l = dynamic_cast<RedstoneLever*>(n); l != nullptr) continue;
            n->update(oppositeDirection(f.direction));
        }
    }
}

void RedstoneLever::toggle() {
    stateChanged = true;
    currentState = !currentState;
}

RedstoneLamp::RedstoneLamp(glm::ivec3 &pos) {
    setupBaseClassMembers(pos, false);
}

void RedstoneLamp::update(int updateDir) {
    bool powered = false;
    for (int i = 0; i < 6; i++) {
        if (neighbors[i] && neighbors[i]->getState()) {
            if (RedstoneTorch *t = dynamic_cast<RedstoneTorch*>(neighbors[i]); i == YPOS && t != nullptr) continue;
            powered = true;
        }
    }

    if (currentState != powered) {
        stateChanged = true;
        currentState = powered;

        for (const BlockFace &f : adjacentFaces) {
            RedstoneItem *n = getNeighbor(f.direction);
            if (n && updateDir != f.direction) n->update(oppositeDirection(f.direction));
        }
    }
}

void RedstoneLamp::turnOff() {
    if (currentState == true) {
        stateChanged = true;
    }
    currentState = false;
}
