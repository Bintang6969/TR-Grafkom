#ifndef PLAYERS_H
#define PLAYERS_H

#include <GL/freeglut.h>
#include "obj_loader.h"

static ObjModel gPandaModel;

static void drawBall(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidSphere(0.12f, 24, 18);

    glColor3f(0.1f, 0.1f, 0.1f);
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glRotatef(72.0f * i, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 0.09f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glutSolidSphere(0.04f, 10, 8);
        glPopMatrix();
    }

    glPopMatrix();
}

static void drawPlayerShadow(float x, float z) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.18f);
    glPushMatrix();
    glTranslatef(x, 0.025f, z);
    glScalef(0.22f, 0.005f, 0.12f);
    glutSolidSphere(1.0, 12, 8);
    glPopMatrix();
    glDisable(GL_BLEND);
}

static void renderModel(const ObjModel& m, float x, float z, float rotY) {
    float cx = (m.minX + m.maxX) / 2.0f;
    float cy = m.minY;
    float cz = (m.minZ + m.maxZ) / 2.0f;
    float modelH = m.maxY - m.minY;
    if (modelH < 0.001f) modelH = 1.0f;
    float normScale = 1.8f / modelH;

    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotY, 0, 1, 0);
    glScalef(normScale, normScale, normScale);
    glTranslatef(-cx, -cy, -cz);

    glColor3f(1.0f, 1.0f, 1.0f);
    glCallList(m.displayList);

    glPopMatrix();
}

static void drawMessi(float x, float z, float rotY) {
    renderModel(gPandaModel, x, z, rotY);
}

static void drawRonaldo(float x, float z, float rotY) {
    renderModel(gPandaModel, x, z, rotY);
}

bool initPlayerModels() {
    bool ok = objLoad("assets/uploads_files_5159816_panda_HP.obj", gPandaModel);
    if (ok) {
        objLoadMtl("assets/uploads_files_5159816_panda_HP.obj", gPandaModel);
        objCompileDisplayList(gPandaModel);
    }
    return ok;
}

void drawPlayers() {
    drawPlayerShadow(-3.0f, 1.0f);
    drawPlayerShadow(3.0f, -1.0f);

    drawMessi(-3.0f, 1.0f, -30.0f);
    drawRonaldo(3.0f, -1.0f, 150.0f);
    drawBall(0.5f, 0.12f, 0.0f);
}

#endif
