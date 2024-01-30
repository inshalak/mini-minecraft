#include "mygl.h"
#include <glm_includes.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_worldAxes(this),
      m_progLambert(this), m_progFlat(this), m_progInstanced(this), m_progLava(this), m_progWater(this), m_progNothing(this),
      m_terrain(this),m_player(glm::vec3(48.f, 129.f, 48.f), m_terrain),
      m_inventory(false), m_previousTime(QDateTime::currentMSecsSinceEpoch()),
      m_frameBuffer(this, this->width(), this->height(), this->devicePixelRatio()), m_quad(this), m_texture(this), m_time(0), m_grass(10), m_dirt(10), m_stone(10), m_water(10),
      m_snow(10), m_lava(10), m_inventorySelectedBlock(EMPTY)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_quad.destroyVBOdata();
}


void MyGL::moveMouseToCenter() {
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);
    m_frameBuffer.create();

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of the world axes
    m_worldAxes.createVBOdata();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    m_progInstanced.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");

    m_progLava.create(":/glsl/overlay.vert.glsl", ":/glsl/lava.frag.glsl");
    m_progWater.create(":/glsl/overlay.vert.glsl", ":/glsl/water.frag.glsl");
    m_progNothing.create(":/glsl/overlay.vert.glsl", ":/glsl/nothing.frag.glsl");

    // Set a color with which to draw geometry.
    // This will ultimately not be used when you change
    // your program to render Chunks with vertex colors
    // and UV coordinates
    m_progLambert.setGeometryColor(glm::vec4(0,1,0,1));

    // Adding in post processing overlays for


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

//    m_terrain.CreateTestScene();

    //Texture and Texture paths
     m_texture.create("://textures/minecraft_textures_all.png");
     m_texture.load(0);

     m_quad.destroyVBOdata();
     m_quad.createVBOdata();

}

void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    // Resizing frame buffer WITh FIX FOR DUMMY APPLE RENDER
    int width, height;

    #ifdef __APPLE__
        m_frameBuffer.resize(w, h, this->devicePixelRatio()*0.5);
    #endif
    #ifndef __APPLE__
        m_frameBuffer.resize(w, h, this->devicePixelRatio());
    #endif


    m_frameBuffer.destroy();
    m_frameBuffer.create();

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    float deltaTime = static_cast<float>(currentTime - m_previousTime) / 1000.0f; // convert to seconds
    m_previousTime = currentTime;

    glm::vec3 playerPosPrev = m_player.mcr_position;
    m_player.tick(deltaTime, m_inputs);
    // expand terrain every 10 ticks
    m_terrain.expandTerrain(m_player.mcr_position, playerPosPrev);
    m_terrain.checkThreadResults();
    update();
}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
    emit sig_sendInvGrass(m_grass);
    emit sig_sendInvDirt(m_dirt);
    emit sig_sendInvStone(m_stone);
    emit sig_sendInvWater(m_water);
    emit sig_sendInvSnow(m_snow);
    emit sig_sendInvLava(m_lava);
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    m_time++;
    m_progLambert.setTime(m_time);
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLambert.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progInstanced.setViewProjMatrix(m_player.mcr_camera.getViewProj());

    //MS2: Animate
       //MS2: texturing
     m_texture.bind(0);
     m_progLambert.setTextureSlot(0);
     m_progWater.setTextureSlot(1);
     m_progLava.setTextureSlot(1);
     m_progNothing.setTextureSlot(1);

     m_frameBuffer.bindFrameBuffer();
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    renderTerrain();


    // FRAME BUFFER: //
   glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
   m_frameBuffer.bindToTextureSlot(1);

     if (m_terrain.hasChunkAt(m_player.mcr_camera.mcr_position.x, m_player.mcr_camera.mcr_position.z) && m_terrain.getBlockAt(m_player.mcr_camera.mcr_position.x, m_player.mcr_camera.mcr_position.y , m_player.mcr_camera.mcr_position.z) == WATER) {
             m_progWater.draw(m_quad, 1);
     } else if (m_terrain.hasChunkAt(m_player.mcr_camera.mcr_position.x, m_player.mcr_camera.mcr_position.z) && m_terrain.getBlockAt(m_player.mcr_camera.mcr_position.x, m_player.mcr_camera.mcr_position.y , m_player.mcr_camera.mcr_position.z) == LAVA) {
             m_progLava.draw(m_quad, 1);
     }
     else {
          m_progNothing.draw(m_quad, 1);

     }


    glDisable(GL_DEPTH_TEST);
    m_progFlat.setModelMatrix(glm::mat4());
    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());

    glEnable(GL_DEPTH_TEST);
}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generatedTerrain
// for more info)
void MyGL::renderTerrain() {
    m_terrain.updateRedstone();
    m_terrain.draw(m_player.mcr_position, &m_progLambert);
}


void MyGL::keyPressEvent(QKeyEvent *e) {
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_player.rotateOnUpGlobal(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_player.rotateOnUpGlobal(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_player.rotateOnRightLocal(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_player.rotateOnRightLocal(amount);
    } else if (e->key() == Qt::Key_Space){
       m_inputs.spacePressed = true;
    } else if (e->key() == Qt::Key_W) {
       m_inputs.wPressed = true;
    } else if (e->key() == Qt::Key_S) {
       m_inputs.sPressed = true;
    } else if (e->key() == Qt::Key_D) {
       m_inputs.dPressed = true;
    } else if (e->key() == Qt::Key_A) {
       m_inputs.aPressed = true;
    } else if (e->key() == Qt::Key_Q) {
       m_inputs.qPressed = true;
    } else if (e->key() == Qt::Key_E) {
       m_inputs.ePressed = true;
    } else if (e->key() == Qt::Key_F) {
       m_player.flightMode = !m_player.flightMode;
    } else if (e->key() == Qt::Key_I) {
        m_inventory = !m_inventory;
        if (m_inventory) {
            std::cout << "show inventory" << std::endl;
        } else {
            std::cout << "hide inventory" << std::endl;
        }
        emit sig_openCloseInventory(m_inventory);
    }

    if (e->key() == Qt::Key_1) {
        m_player.editBlock(m_inputs, m_terrain, true, REDSTONE_TORCH_ON, m_grass, m_dirt, m_stone, m_water, m_snow, m_lava);
    } else if (e->key() == Qt::Key_2) {
        m_player.editBlock(m_inputs, m_terrain, true, REDSTONE_LEVER_OFF, m_grass, m_dirt, m_stone, m_water, m_snow, m_lava);
    } else if (e->key() == Qt::Key_3) {
        m_player.editBlock(m_inputs, m_terrain, true, REDSTONE_LAMP_OFF, m_grass, m_dirt, m_stone, m_water, m_snow, m_lava);
    } else if (e->key() == Qt::Key_4) {
        m_player.editBlock(m_inputs, m_terrain, true, REDSTONE_WIRE_OFF, m_grass, m_dirt, m_stone, m_water, m_snow, m_lava);
    } else if (e->key() == Qt::Key_5) {
        m_player.editBlock(m_inputs, m_terrain, true, CACTUS, m_grass, m_dirt, m_stone, m_water, m_snow, m_lava);
    }
}


// WRITE KEY RELEASE EVENT WHICH DOES THE SAME THING AS BEFORE BUT TURNS EVERYTHING FALSE
void MyGL::keyReleaseEvent(QKeyEvent *e) {
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Space){
       m_inputs.spacePressed = false;
    } else if (e->key() == Qt::Key_W) {
       m_inputs.wPressed = false;
    } else if (e->key() == Qt::Key_S) {
       m_inputs.sPressed = false;
    } else if (e->key() == Qt::Key_D) {
       m_inputs.dPressed = false;
    } else if (e->key() == Qt::Key_A) {
       m_inputs.aPressed = false;
    } else if (e->key() == Qt::Key_Q) {
       m_inputs.qPressed = false;
    } else if (e->key() == Qt::Key_E) {
       m_inputs.ePressed = false;
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    // teleport mouse back to the center after i update m_inputs
    // can calculate the distance different because i know how large the screen is and where the center is
    // calculate what ^ degree away this is & then rotate the camera that way
    // multiply it by the sensitivity and that is number or degrees that i call rotateON

    float centerX = width() / 2;
    float centerY= height() / 2;

    m_inputs.mouseDeltaX = e->pos().rx() - centerX;
    m_inputs.mouseDeltaY = e->pos().ry() - centerY;

    moveMouseToCenter();

}

void MyGL::mousePressEvent(QMouseEvent *e) {
//    if (e->button() == Qt::LeftButton) {
// find block that i want to destroy
    // in chunk information, take list of blocks and remove it
    // call chunk's create function
    // and recreate neighbors
    // setblockat
    // createvbo data
//    }

    if (e->button() == Qt::LeftButton) {
        m_player.editBlock(m_inputs, m_terrain, false, EMPTY, m_grass, m_dirt, m_stone, m_water, m_snow, m_lava);
    } else if (e->button() == Qt::RightButton) {
        m_player.editBlock(m_inputs, m_terrain, true, m_inventorySelectedBlock, m_grass, m_dirt, m_stone, m_water, m_snow, m_lava);
    }
}
