#include "player.h"
#include <QString>

#include "player.h"
#include <QString>
#include <iostream>

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0), m_camera(pos + glm::vec3(0, 1.5f, 0)),
      mcr_terrain(terrain), mcr_camera(m_camera),
      flightMode(true), waterMode(false), swimming(new QSoundEffect), thunk(new QSoundEffect)
{
    swimming -> setSource(QUrl::fromLocalFile("sounds/bubbles.wav"));
    swimming -> setVolume(0.5);
    thunk -> setSource(QUrl::fromLocalFile("sounds/thunk.wav"));
    thunk -> setVolume(1);
}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    processInputs(input);
    computePhysics(dT, mcr_terrain);
}

void Player::editBlock(InputBundle &inputs, Terrain &terrain, bool mode, BlockType b, int& m_grass, int& m_dirt, int& m_stone, int& m_water, int& m_snow,int& m_lava) {
    // if mode is true - add block ; if mode is false - remove block
    float dist = 0;
    glm::vec3 blockHit;
    BlockType blockType;
    bool collided = gridMarch(m_camera.mcr_position,
                              3.f * glm::normalize(m_camera.mcr_forward), terrain, &dist, &blockHit, &blockType);

    if (collided) {
        if (mode) {
            // ADD BLOCK MODE --
            float dist2 = 0;
            glm::vec3 blockHit2;
            thunk ->play();
            gridMarchBlockBefore(m_camera.mcr_position, 3.f * glm::normalize(m_camera.mcr_forward), terrain, &dist2, &blockHit2);
            terrain.setBlockAt(blockHit2.x, blockHit2.y, blockHit2.z, b);
            terrain.updateChunk(terrain.getChunkAt(blockHit2.x, blockHit2.z).get());

            // redstone
            if (redstoneBlocks.count(b)) {
                terrain.setRedstoneItemAt(blockHit2.x, blockHit2.y, blockHit2.z, b);
            }

            if (b == GRASS){
                m_grass --;
            } if (b == DIRT){
                m_dirt --;
            } if (b == STONE){
                m_stone --;
            } if (b == WATER){
                m_water --;
            } if (b == SNOW){
                m_snow --;
            } if (b == LAVA){
                m_lava --;
            }
        } else if (blockType != BEDROCK) {
            if (blockType == REDSTONE_LEVER_OFF || blockType == REDSTONE_LEVER_ON) {
                terrain.toggleLever(blockHit.x, blockHit.y, blockHit.z);
                return void();
            }

            thunk ->play();
            // REMOVE BLOCK MODE --
            // remove blockHit
            terrain.setBlockAt(blockHit.x, blockHit.y, blockHit.z, EMPTY);
            terrain.updateChunk(terrain.getChunkAt(blockHit.x, blockHit.z).get());

            // redstone
            if (redstoneBlocks.count(b)) {
                terrain.removeRedstoneItemAt(blockHit.x, blockHit.y, blockHit.z, b);
            }


            if (blockType == GRASS){
                m_grass ++;
            } if (blockType == DIRT){
                m_dirt ++;
            } if (blockType == STONE){
                m_stone ++;
            } if (blockType == WATER){
                m_water ++;
            } if (blockType == SNOW){
                m_snow ++;
            } if (blockType == LAVA){
                m_lava ++;
            }
        }
    }
}

void Player::processInputs(InputBundle &inputs) {
    // KEYBOARD CHANGES
    float acc = 40;
    m_acceleration = glm::vec3(0.0f);
    if (inputs.wPressed && !inputs.sPressed){
        m_acceleration += m_forward * acc;
    } if (inputs.sPressed && !inputs.wPressed){
        m_acceleration -= m_forward * acc;
    } if (inputs.dPressed && !inputs.aPressed){
        m_acceleration += m_right * acc;
    } if (inputs.aPressed && !inputs.dPressed){
        m_acceleration -= m_right * acc;
    }
    if (flightMode) {
        if (inputs.ePressed && !inputs.qPressed){
            m_acceleration += m_up * acc;
        } if (inputs.qPressed && !inputs.ePressed){
            m_acceleration -= m_up * acc;
        }
    }
    if (!flightMode) {
        // discard y component and re-normalize IF LENGTH IS GREATER THAN 0 , OR if either x, z is nonzero
        if (m_acceleration.x > 0 || m_acceleration.z > 0){
            m_acceleration = glm::vec3(m_acceleration.x, 0, m_acceleration.z);
        }
    }
    if ((!flightMode || waterMode) && inputs.spacePressed) {
        m_velocity.y += 5;
    }

    // MOUSE CHANGES
    m_camera.rotateOnRightLocal(inputs.mouseDeltaY * -0.1f);
    this -> rotateOnUpGlobal(inputs.mouseDeltaX * -0.1f);


}

void Player::computePhysics(float dT, const Terrain &terrain) {
    // TODO: Update the Player's position based on its acceleration
    // and velocity, and also perform collision detection.
    // Apply friction and drag

    //    float friction = 0.7f; // coefficient of friction
    //    float drag = 0.4f; // coefficient of drag
    //    glm::vec3 frictionForce = -m_velocity * friction;
    //    glm::vec3 dragForce = -m_velocity * glm::length(m_velocity) * drag;
    glm::vec3 gravity = glm::vec3(0, -100.f, 0);
    //    m_acceleration += (frictionForce + dragForce);
    if (glm::length(m_velocity) > m_maxSpeed) {
        m_velocity = glm::normalize(m_velocity);
        m_velocity *= m_maxSpeed;
    }


    m_acceleration += !flightMode ? gravity : glm::vec3();
    m_velocity += m_acceleration * dT;
    m_velocity *= 0.9;
    glm::vec3 finalMovement = m_velocity *dT;

    for (double x = -0.5; x <= 0.5; x++) {
        for (double z = -0.5; z <= 0.5; z++) {
            for (double y = 0; y <= 2; y++) {
                glm::vec3 rayOrigin = m_position + glm::vec3(x, y, z);
                glm::vec3 blockHit;

                for (int i = 0; i <= 2; i ++){
                    float dist;
                    glm::vec3 blockHit;
                    glm::vec3 rayDirection = glm::vec3();
                    BlockType blockType;
                    rayDirection[i] = glm::sign(finalMovement[i]);
                    if (gridMarch(rayOrigin, rayDirection, terrain, &dist, &blockHit, &blockType)) {
                        // Player collided with terrain, move to the block hit point
                        if (blockType == WATER || blockType == LAVA){
                            if (glm::length(m_velocity) > 7) {
                                m_velocity = glm::normalize(m_velocity);
                                m_velocity *= 7;
                                finalMovement = m_velocity *dT;
                            } if (blockType == WATER) {
                                waterMode = true;
                                swimming->play();
                            }
                        } else if (!flightMode){
                            waterMode = false;
                            if (dist < 0.02){
                                finalMovement[i] = 0;
                                m_velocity[i] = 0;
                            } else {
                                finalMovement[i] = glm::min(glm::abs(finalMovement[i]), glm::abs(dist)) - 0.02;
                                finalMovement[i] *= glm::sign(rayDirection)[i];
                            }
                        }
                    }
                }
            }
        }
    }
    moveAlongVector(finalMovement);
    // ALWAYS CALL MOVE ALONG VECTOR SO EVERYTHING IS SYNCED
}



bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::vec3 *out_blockHit, BlockType *out_blockType) {
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::vec3 currCell = glm::vec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if(axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if(interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin += rayDirection * min_t;
        glm::vec3 offset = glm::vec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::vec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return
        // curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        if(cellType != EMPTY) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            *out_blockType = cellType;
            return true;
        }
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

bool Player::gridMarchBlockBefore(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::vec3 *out_blockHit) {
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::vec3 currCell = glm::vec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if(axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if(interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin += rayDirection * min_t;
        glm::vec3 offset = glm::vec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        glm::vec3 nextCell = glm::vec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return
        // curr_t
        BlockType cellType = terrain.getBlockAt(nextCell.x, nextCell.y, nextCell.z);
        if(cellType != EMPTY) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
        currCell = nextCell;
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}
