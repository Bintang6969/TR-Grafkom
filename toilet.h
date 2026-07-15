#ifndef TOILET_H
#define TOILET_H

#include <GL/freeglut.h>

static GLuint toiletWallTex = 0;

static float toiletWidth   = 8.0f;
static float toiletDepth   = 5.0f;
static float toiletHeight  = 3.5f;
static float doorWidth     = 2.0f;
static float doorHeight    = 2.8f;

void initToilets(GLuint wallTexture) {
    toiletWallTex = wallTexture;
}

static void drawTexturedQuad(float x0, float y0, float z0,
                              float x1, float y1, float z1,
                              float x2, float y2, float z2,
                              float x3, float y3, float z3,
                              float uRepeat, float vRepeat) {
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);       glVertex3f(x0, y0, z0);
    glTexCoord2f(uRepeat, 0); glVertex3f(x1, y1, z1);
    glTexCoord2f(uRepeat, vRepeat); glVertex3f(x2, y2, z2);
    glTexCoord2f(0, vRepeat); glVertex3f(x3, y3, z3);
    glEnd();
}

// ==========================================
// INTERIOR — perabot toilet
// ==========================================
static void drawToiletBowl(float x, float z) {
    // tangki air, menempel dinding belakang
    glColor3f(0.95f, 0.95f, 0.95f);
    glPushMatrix();
    glTranslatef(x, 1.1f, z - 0.28f);
    glScalef(0.5f, 0.6f, 0.22f);
    glutSolidCube(1.0);
    glPopMatrix();

    // badan/pipa bawah
    glPushMatrix();
    glTranslatef(x, 0.35f, z);
    glScalef(0.35f, 0.7f, 0.35f);
    glutSolidCube(1.0);
    glPopMatrix();

    // mangkuk kloset (elips pipih)
    glPushMatrix();
    glTranslatef(x, 0.55f, z + 0.15f);
    glScalef(0.4f, 0.35f, 0.5f);
    glutSolidSphere(0.5, 12, 10);
    glPopMatrix();

    // dudukan (seat ring)
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(x, 0.72f, z + 0.12f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidTorus(0.06, 0.28, 8, 16);
    glPopMatrix();
}

static void drawWashbasin(float x, float z, float rotY) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // kaki wastafel (pedestal)
    glColor3f(0.85f, 0.85f, 0.85f);
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    glScalef(0.25f, 0.8f, 0.25f);
    glutSolidCube(1.0);
    glPopMatrix();

    // baskom
    glColor3f(0.95f, 0.95f, 0.95f);
    glPushMatrix();
    glTranslatef(0.0f, 0.85f, 0.0f);
    glScalef(0.7f, 0.2f, 0.5f);
    glutSolidCube(1.0);
    glPopMatrix();

    // cermin
    glColor3f(0.6f, 0.75f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, 1.6f, -0.05f);
    glScalef(0.6f, 0.6f, 0.03f);
    glutSolidCube(1.0);
    glPopMatrix();

    // --- KERAN AIR (faucet) ---
    // tiang keran (silinder vertikal)
    glColor3f(0.75f, 0.75f, 0.78f);
    glPushMatrix();
    glTranslatef(0.0f, 1.1f, -0.1f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, 0.04f, 0.04f, 0.25f, 8, 4);
    gluDeleteQuadric(q);
    glPopMatrix();

    // leher keran (melengkung ke depan atas)
    glPushMatrix();
    glTranslatef(0.0f, 1.3f, -0.1f);
    glRotatef(-25, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric();
    gluCylinder(q2, 0.035f, 0.035f, 0.22f, 8, 4);
    gluDeleteQuadric(q2);
    glPopMatrix();

    // mulut keran (ujung horizontal ke depan)
    glPushMatrix();
    glTranslatef(0.0f, 1.38f, 0.08f);
    glRotatef(70, 1, 0, 0);
    GLUquadric* q3 = gluNewQuadric();
    gluCylinder(q3, 0.03f, 0.03f, 0.15f, 8, 4);
    gluDeleteQuadric(q3);
    glPopMatrix();

    // gagang putar keran (kecil, di samping tiang)
    glColor3f(0.85f, 0.7f, 0.2f);
    glPushMatrix();
    glTranslatef(0.12f, 1.18f, -0.1f);
    glRotatef(90, 0, 1, 0);
    GLUquadric* q4 = gluNewQuadric();
    gluCylinder(q4, 0.025f, 0.025f, 0.1f, 6, 4);
    gluDeleteQuadric(q4);
    glPopMatrix();

    glPopMatrix();
}

// Pintu berengsel di sisi kanan gap pintu (x = +doorW2), terbuka ke arah dalam.
static void drawDoor(float doorW2, float doorW, float doorH, float hd, float openAngle) {
    glColor3f(0.45f, 0.28f, 0.15f);
    glPushMatrix();
    glTranslatef(doorW2, 0.0f, hd);
    glRotatef(-openAngle, 0.0f, 1.0f, 0.0f);   // negatif = ayun ke dalam ruangan
    glTranslatef(-doorW / 2.0f, doorH / 2.0f, 0.0f);
    glScalef(doorW, doorH, 0.08f);
    glutSolidCube(1.0);
    glPopMatrix();

    // gagang pintu
    glColor3f(0.85f, 0.7f, 0.2f);
    glPushMatrix();
    glTranslatef(doorW2, 0.0f, hd);
    glRotatef(-openAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(-doorW * 0.85f, doorH * 0.5f, 0.06f);
    glutSolidSphere(0.06, 8, 8);
    glPopMatrix();
}

static void drawToiletUnit(float posX, float posZ, float rotY) {
    float hw = toiletWidth  / 2.0f;
    float hd = toiletDepth  / 2.0f;
    float h  = toiletHeight;

    float doorW2 = doorWidth / 2.0f;

    glPushMatrix();
    glTranslatef(posX, 0.0f, posZ);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // ==========================================
    // DINDING — TEKSTUR
    // ==========================================
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, toiletWallTex);
    glColor3f(1.0f, 1.0f, 1.0f);

    // --- Depan (z = +hd), ada celah pintu ---
    float uW = toiletWidth / 4.0f;
    float uL = (hw - doorW2) / 4.0f;
    float uR = uW - uL; // koordinat s untuk sisi kanan panel atas pintu

    // kiri pintu
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);        glVertex3f(-hw, 0,     hd);
    glTexCoord2f(uL, 0);       glVertex3f(-doorW2, 0, hd);
    glTexCoord2f(uL, 1);       glVertex3f(-doorW2, h,  hd);
    glTexCoord2f(0, 1);        glVertex3f(-hw, h,      hd);
    glEnd();

    // kanan pintu
    glBegin(GL_QUADS);
    glTexCoord2f(uL, 0);       glVertex3f(doorW2, 0,  hd);
    glTexCoord2f(uW, 0);       glVertex3f(hw, 0,      hd);
    glTexCoord2f(uW, 1);       glVertex3f(hw, h,       hd);
    glTexCoord2f(uL, 1);       glVertex3f(doorW2, h,   hd);
    glEnd();

    // atas pintu
    glBegin(GL_QUADS);
    glTexCoord2f(uL, 0.85f);   glVertex3f(-doorW2, doorHeight, hd);
    glTexCoord2f(uR, 0.85f);   glVertex3f(doorW2,  doorHeight, hd);
    glTexCoord2f(uR, 1.0f);    glVertex3f(doorW2,  h,          hd);
    glTexCoord2f(uL, 1.0f);    glVertex3f(-doorW2, h,          hd);
    glEnd();

    // --- Belakang (z = -hd) ---
    drawTexturedQuad(-hw,0,-hd, hw,0,-hd, hw,h,-hd, -hw,h,-hd, 2, 1);

    // --- Kiri (x = -hw) ---
    drawTexturedQuad(-hw,0,-hd, -hw,0,hd, -hw,h,hd, -hw,h,-hd, 1.25f, 1);

    // --- Kanan (x = +hw) ---
    drawTexturedQuad(hw,0,hd, hw,0,-hd, hw,h,-hd, hw,h,hd, 1.25f, 1);

    glDisable(GL_TEXTURE_2D);

    // ==========================================
    // INTERIOR
    // ==========================================
    // lantai
    glColor3f(0.8f, 0.8f, 0.75f);
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, 0.0f);
    glScalef(toiletWidth, 0.04f, toiletDepth);
    glutSolidCube(1.0);
    glPopMatrix();

    // kloset di pojok belakang-kanan
    drawToiletBowl(hw - 1.4f, -hd + 0.7f);

    // wastafel menempel dinding kiri
    drawWashbasin(-hw + 0.35f, 0.0f, 90.0f);

    // daun pintu, terbuka sedikit ke dalam
    drawDoor(doorW2, doorWidth, doorHeight, hd, 55.0f);

    // ==========================================
    // ATAP — SOLID
    // ==========================================
    glColor3f(0.55f, 0.35f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, h + 0.15f, -0.5f);
    glRotatef(3.0f, 1.0f, 0.0f, 0.0f);
    glScalef(toiletWidth + 1.0f, 0.3f, toiletDepth + 2.0f);
    glutSolidCube(1.0);
    glPopMatrix();

    // List atap depan
    glColor3f(0.7f, 0.7f, 0.7f);
    glPushMatrix();
    glTranslatef(0.0f, h + 0.05f, hd + 0.55f);
    glScalef(toiletWidth + 1.0f, 0.2f, 0.2f);
    glutSolidCube(1.0);
    glPopMatrix();

    // ==========================================
    // TIANG PENYANGGA DEPAN
    // ==========================================
    glColor3f(0.35f, 0.35f, 0.35f);
    float pillarX = hw - 0.8f;
    for (int i = 0; i < 2; i++) {
        float px = (i == 0) ? -pillarX : pillarX;
        glPushMatrix();
        glTranslatef(px, h / 2.0f, hd + 0.5f);
        glScalef(0.5f, h, 0.5f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // ==========================================
    // PLANG "TOILET" — huruf geometris
    // ==========================================
    float signW = 5.0f, signH = 1.2f;
    float signZ = hd + 0.55f;
    float signY = h + 1.1f;

    // Panel latar biru
    glColor3f(0.1f, 0.3f, 0.8f);
    glPushMatrix();
    glTranslatef(0.0f, signY, signZ);
    glScalef(signW, signH, 0.15f);
    glutSolidCube(1.0);
    glPopMatrix();

    // Bingkai putih
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, signY, signZ + 0.08f);
    glScalef(signW, signH, 0.01f);
    glutWireCube(1.02f);
    glPopMatrix();

    // Huruf "TOILET" dari kubus-kubus kecil putih
    glColor3f(1.0f, 1.0f, 1.0f);
    float lx = -2.1f, ly = signY - 0.25f, lz = signZ + 0.1f;
    float cw = 0.12f, ch = 0.5f;

    // T — batang tegak penuh tinggi, palang atas TIPIS (dulu salah pakai tinggi = ch,
    // sehingga palang atas jadi setinggi seluruh huruf dan menutupi bentuk T)
    glPushMatrix(); glTranslatef(lx, ly+ch/2, lz); glScalef(cw*3, cw, 0.08f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(lx, ly, lz);       glScalef(cw, ch, 0.08f);  glutSolidCube(1.0); glPopMatrix();
    lx += 0.5f;

    // O
    glPushMatrix(); glTranslatef(lx-0.18f, ly, lz);       glScalef(cw, ch, 0.08f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(lx+0.18f, ly, lz);       glScalef(cw, ch, 0.08f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(lx, ly+ch/2, lz);        glScalef(cw*3, cw, 0.08f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(lx, ly-ch/2, lz);        glScalef(cw*3, cw, 0.08f); glutSolidCube(1.0); glPopMatrix();
    lx += 0.55f;

    // I
    glPushMatrix(); glTranslatef(lx, ly, lz); glScalef(cw, ch, 0.08f); glutSolidCube(1.0); glPopMatrix();
    lx += 0.3f;

    // L
    glPushMatrix(); glTranslatef(lx-0.15f, ly, lz);       glScalef(cw, ch, 0.08f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(lx, ly-ch/2, lz);        glScalef(cw*2.5f, cw, 0.08f); glutSolidCube(1.0); glPopMatrix();
    lx += 0.5f;

    // E
    glPushMatrix(); glTranslatef(lx-0.15f, ly, lz);       glScalef(cw, ch, 0.08f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(lx, ly+ch/2, lz);        glScalef(cw*2.5f, cw, 0.08f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(lx, ly, lz);              glScalef(cw*2, cw, 0.08f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(lx, ly-ch/2, lz);        glScalef(cw*2.5f, cw, 0.08f); glutSolidCube(1.0); glPopMatrix();
    lx += 0.5f;

    // T — sama seperti T pertama, palang atas dibetulkan
    glPushMatrix(); glTranslatef(lx, ly+ch/2, lz); glScalef(cw*3, cw, 0.08f); glutSolidCube(1.0); glPopMatrix();
    glPushMatrix(); glTranslatef(lx, ly, lz);       glScalef(cw, ch, 0.08f);  glutSolidCube(1.0); glPopMatrix();

    glPopMatrix();
}

void drawToilets() {
    drawToiletUnit(-50.0f, 65.0f, -180.0f);
    drawToiletUnit( 50.0, -65.0f, 0.0f);
}

#endif