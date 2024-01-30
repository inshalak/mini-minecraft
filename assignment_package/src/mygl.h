#ifndef MYGL_H
#define MYGL_H

#include "framebuffer.h"
#include "openglcontext.h"
#include "scene/quad.h"
#include "scene/texture.h"
#include "shaderprogram.h"
#include "scene/worldaxes.h"
#include "scene/camera.h"
#include "scene/terrain.h"
#include "scene/player.h"
#include <QDate>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <smartpointerhelp.h>


class MyGL : public OpenGLContext
{
    Q_OBJECT
private:
    WorldAxes m_worldAxes; // A wireframe representation of the world axes. It is hard-coded to sit centered at (32, 128, 32).
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram m_progInstanced;// A shader program that is designed to be compatible with instanced rendering

    //Post processing overlays
    ShaderProgram m_progLava;
    ShaderProgram m_progWater;
    ShaderProgram m_progNothing;

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Terrain m_terrain; // All of the Chunks that currently comprise the world.
    Player m_player; // The entity controlled by the user. Contains a camera to display what it sees as well.
    InputBundle m_inputs; // A collection of variables to be updated in keyPressEvent, mouseMoveEvent, mousePressEvent, etc.

    bool m_inventory; //status of inventory window
    QTimer m_timer; // Timer linked to tick(). Fires approximately 60 times per second.
    qint64 m_previousTime;

    // Post-processing overlays
    FrameBuffer m_frameBuffer;
    Quad m_quad;

    Texture m_texture; // MS2: Adding in texturing
    int m_time; //MS2:: NOT to be confused with m_timer!

    void moveMouseToCenter(); // Forces the mouse position to the screen's center. You should call this
                              // from within a mouse move event after reading the mouse movement so that
                              // your mouse stays within the screen bounds and is always read.

    void sendPlayerDataToGUI() const;


public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    int m_grass, m_dirt, m_stone, m_water, m_snow, m_lava;
    BlockType m_inventorySelectedBlock;


    // Called once when MyGL is initialized.
    // Once this is called, all OpenGL function
    // invocations are valid (before this, they
    // will cause segfaults)
    void initializeGL() override;
    // Called whenever MyGL is resized.
    void resizeGL(int w, int h) override;
    // Called whenever MyGL::update() is called.
    // In the base code, update() is called from tick().
    void paintGL() override;

    // Called from paintGL().
    // Calls Terrain::draw().
    void renderTerrain();

protected:
    // Automatically invoked when the user
    // presses a key on the keyboard
    void keyPressEvent(QKeyEvent *e);
    // Automatically invoked when the user
    // releases a key on the keyboard
    void keyReleaseEvent(QKeyEvent *e);
    // Automatically invoked when the user
    // moves the mouse
    void mouseMoveEvent(QMouseEvent *e);
    // Automatically invoked when the user
    // presses a mouse button
    void mousePressEvent(QMouseEvent *e);

private slots:
    void tick(); // Slot that gets called ~60 times per second by m_timer firing.

signals:
    void sig_sendPlayerPos(QString) const;
    void sig_sendPlayerVel(QString) const;
    void sig_sendPlayerAcc(QString) const;
    void sig_sendPlayerLook(QString) const;
    void sig_sendPlayerChunk(QString) const;
    void sig_sendPlayerTerrainZone(QString) const;

    void sig_openCloseInventory(bool);

    void sig_sendInvGrass(int) const;
    void sig_sendInvDirt(int) const;
    void sig_sendInvStone(int) const;
    void sig_sendInvWater(int) const;
    void sig_sendInvSnow(int) const;
    void sig_sendInvLava(int) const;
};


#endif // MYGL_H
