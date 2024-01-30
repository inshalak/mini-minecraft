#include "drawable.h"
#include <glm_includes.h>

Drawable::Drawable(OpenGLContext* context)
    : m_count(-1), m_countOpq(-1), m_countTra(-1),
      m_bufIdx(),
      m_bufIdxOpq(), m_bufIdxTra(),
      m_bufPos(), m_bufNor(), m_bufCol(),
      m_bufInterleaved(),
      m_bufInterleavedOpq(), m_bufInterleavedTra(),
      m_idxGenerated(false),
      m_idxGeneratedOpq(false), m_idxGeneratedTra(false),
      m_posGenerated(false), m_norGenerated(false), m_colGenerated(false),
      m_uvGenerated(false),
      m_interleavedGenerated(false),
      m_interleavedGeneratedOpq(false), m_interleavedGeneratedTra(false),
      mp_context(context)
{}

Drawable::~Drawable()
{}


void Drawable::destroyVBOdata()
{
    mp_context->glDeleteBuffers(1, &m_bufIdx);
    mp_context->glDeleteBuffers(1, &m_bufPos);
    mp_context->glDeleteBuffers(1, &m_bufNor);
    mp_context->glDeleteBuffers(1, &m_bufCol);
    mp_context->glDeleteBuffers(1, &m_bufUV); // MS2: UV Changed
    m_idxGenerated = m_posGenerated = m_norGenerated = m_colGenerated = false;
    m_count = -1;
}

GLenum Drawable::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

int Drawable::elemCount()
{
    return m_count;
}

int Drawable::opqCount() {
    return m_countOpq;
}

int Drawable::traCount() {
    return m_countTra;
}

void Drawable::generateIdx()
{
    m_idxGenerated = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &m_bufIdx);
}

void Drawable::generateIdxOpq() {
    m_idxGeneratedOpq = true;
    mp_context->glGenBuffers(1, &m_bufIdxOpq);
}

void Drawable::generateIdxTra() {
    m_idxGeneratedTra = true;
    mp_context->glGenBuffers(1, &m_bufIdxTra);
}

void Drawable::generatePos()
{
    m_posGenerated = true;
    // Create a VBO on our GPU and store its handle in bufPos
    mp_context->glGenBuffers(1, &m_bufPos);
}

void Drawable::generateNor()
{
    m_norGenerated = true;
    // Create a VBO on our GPU and store its handle in bufNor
    mp_context->glGenBuffers(1, &m_bufNor);
}

void Drawable::generateCol()
{
    m_colGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mp_context->glGenBuffers(1, &m_bufCol);
}

void Drawable::generateUV()
{
    m_uvGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mp_context->glGenBuffers(1, &m_bufUV);
}

void Drawable::generateInterleaved() {
    m_interleavedGenerated = true;
    mp_context->glGenBuffers(1, &m_bufInterleaved);
}

void Drawable::generateInterleavedOpq() {
    m_interleavedGeneratedOpq = true;
    mp_context->glGenBuffers(1, &m_bufInterleavedOpq);
}

void Drawable::generateInterleavedTra() {
    m_interleavedGeneratedTra = true;
    mp_context->glGenBuffers(1, &m_bufInterleavedTra);
}

bool Drawable::bindIdx()
{
    if(m_idxGenerated) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    }
    return m_idxGenerated;
}

bool Drawable::bindIdxOpq() {
    if (m_idxGeneratedOpq) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxOpq);
    }
    return m_idxGeneratedOpq;
}

bool Drawable::bindIdxTra() {
    if (m_idxGeneratedTra) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdxTra);
    }
    return m_idxGeneratedTra;
}

bool Drawable::bindPos()
{
    if(m_posGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    }
    return m_posGenerated;
}

bool Drawable::bindNor()
{
    if(m_norGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    }
    return m_norGenerated;
}

bool Drawable::bindCol()
{
    if(m_colGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    }
    return m_colGenerated;
}


bool Drawable::bindUV()
{
    if(m_uvGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufUV);
    }
    return m_uvGenerated;
}


bool Drawable::bindInterleaved() {
    if (m_interleavedGenerated) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleaved);
    }
    return m_interleavedGenerated;
}

bool Drawable::bindInterleavedOpq() {
    if (m_interleavedGeneratedOpq) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleavedOpq);
    }
    return m_interleavedGeneratedOpq;
}

bool Drawable::bindInterleavedTra() {
    if (m_interleavedGeneratedTra) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleavedTra);
    }
    return m_interleavedGeneratedTra;
}

InstancedDrawable::InstancedDrawable(OpenGLContext *context)
    : Drawable(context), m_numInstances(0), m_bufPosOffset(-1), m_offsetGenerated(false)
{}

InstancedDrawable::~InstancedDrawable(){}

int InstancedDrawable::instanceCount() const {
    return m_numInstances;
}

void InstancedDrawable::generateOffsetBuf() {
    m_offsetGenerated = true;
    mp_context->glGenBuffers(1, &m_bufPosOffset);
}

bool InstancedDrawable::bindOffsetBuf() {
    if(m_offsetGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPosOffset);
    }
    return m_offsetGenerated;
}


void InstancedDrawable::clearOffsetBuf() {
    if(m_offsetGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufPosOffset);
        m_offsetGenerated = false;
    }
}
void InstancedDrawable::clearColorBuf() {
    if(m_colGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufCol);
        m_colGenerated = false;
    }
}
