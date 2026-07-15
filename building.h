#ifndef BUILDING_H
#define BUILDING_H

#include <GL/freeglut.h>

// =============================================
// KERANGKA BANGUNAN — TEKSTUR BETON
// =============================================

static float bldgLenX  = 60.0f;
static float bldgLenZ  = 30.0f;
static float bldgH     = 15.0f;
static float bldgPosX  = 0.0f;
static float bldgPosZ  = 120.0f;

static float colSize   = 0.8f;
static float beamH     = 0.6f;

static int   colsX     = 7;
static int   colsZ     = 4;

static float floorY[3] = {5.0f, 10.0f, 15.0f};

static GLuint bldgTex = 0;

void initBuilding(GLuint tex) {
    bldgTex = tex;
}

// Gambar kubus bertekstur (manual, menggantikan glutSolidCube)
static void drawTexturedCube(float sx, float sy, float sz) {
    float hx = sx / 2.0f;
    float hy = sy / 2.0f;
    float hz = sz / 2.0f;

    glBegin(GL_QUADS);
    // depan (+Z)
    glTexCoord2f(0, 0); glVertex3f(-hx, -hy,  hz);
    glTexCoord2f(1, 0); glVertex3f( hx, -hy,  hz);
    glTexCoord2f(1, 1); glVertex3f( hx,  hy,  hz);
    glTexCoord2f(0, 1); glVertex3f(-hx,  hy,  hz);
    // belakang (-Z)
    glTexCoord2f(0, 0); glVertex3f( hx, -hy, -hz);
    glTexCoord2f(1, 0); glVertex3f(-hx, -hy, -hz);
    glTexCoord2f(1, 1); glVertex3f(-hx,  hy, -hz);
    glTexCoord2f(0, 1); glVertex3f( hx,  hy, -hz);
    // kiri (-X)
    glTexCoord2f(0, 0); glVertex3f(-hx, -hy, -hz);
    glTexCoord2f(1, 0); glVertex3f(-hx, -hy,  hz);
    glTexCoord2f(1, 1); glVertex3f(-hx,  hy,  hz);
    glTexCoord2f(0, 1); glVertex3f(-hx,  hy, -hz);
    // kanan (+X)
    glTexCoord2f(0, 0); glVertex3f( hx, -hy,  hz);
    glTexCoord2f(1, 0); glVertex3f( hx, -hy, -hz);
    glTexCoord2f(1, 1); glVertex3f( hx,  hy, -hz);
    glTexCoord2f(0, 1); glVertex3f( hx,  hy,  hz);
    // atas (+Y)
    glTexCoord2f(0, 0); glVertex3f(-hx,  hy,  hz);
    glTexCoord2f(1, 0); glVertex3f( hx,  hy,  hz);
    glTexCoord2f(1, 1); glVertex3f( hx,  hy, -hz);
    glTexCoord2f(0, 1); glVertex3f(-hx,  hy, -hz);
    // bawah (-Y)
    glTexCoord2f(0, 0); glVertex3f(-hx, -hy, -hz);
    glTexCoord2f(1, 0); glVertex3f( hx, -hy, -hz);
    glTexCoord2f(1, 1); glVertex3f( hx, -hy,  hz);
    glTexCoord2f(0, 1); glVertex3f(-hx, -hy,  hz);
    glEnd();
}

static void drawTColumn(float x, float y0, float y1, float z) {
    float h = y1 - y0;
    glPushMatrix();
    glTranslatef(x, y0 + h / 2.0f, z);
    drawTexturedCube(colSize, h, colSize);
    glPopMatrix();
}

static void drawTBeamX(float x0, float x1, float y, float z, float depth) {
    float len = x1 - x0;
    glPushMatrix();
    glTranslatef((x0 + x1) / 2.0f, y, z);
    drawTexturedCube(len, beamH, depth);
    glPopMatrix();
}

static void drawTBeamZ(float x, float z0, float z1, float y, float depth) {
    float len = z1 - z0;
    glPushMatrix();
    glTranslatef(x, y, (z0 + z1) / 2.0f);
    drawTexturedCube(depth, beamH, len);
    glPopMatrix();
}

static bool isPerimeter(int ix, int iz) {
    return ix == 0 || ix == colsX - 1 || iz == 0 || iz == colsZ - 1;
}

void drawBuilding() {
    float halfX = bldgLenX / 2.0f;
    float halfZ = bldgLenZ / 2.0f;
    float spacingX = bldgLenX / (colsX - 1);
    float spacingZ = bldgLenZ / (colsZ - 1);

    glPushMatrix();
    glTranslatef(bldgPosX, 0.0f, bldgPosZ);

    // ==========================================
    // LANTAI DASAR — HIJAU (solid)
    // ==========================================
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.20f, 0.55f, 0.20f);
    glPushMatrix();
    glTranslatef(0.0f, 0.03f, 0.0f);
    glScalef(bldgLenX, 0.06f, bldgLenZ);
    glutSolidCube(1.0);
    glPopMatrix();

    // ==========================================
    // KOLOM + BALOK — TEKSTUR BETON
    // ==========================================
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bldgTex);
    glColor3f(1.0f, 1.0f, 1.0f);

    // Kolom perimeter
    for (int ix = 0; ix < colsX; ix++) {
        for (int iz = 0; iz < colsZ; iz++) {
            if (!isPerimeter(ix, iz)) continue;
            float x = -halfX + ix * spacingX;
            float z = -halfZ + iz * spacingZ;
            drawTColumn(x, 0.0f, bldgH, z);
        }
    }

    // Balok perimeter di y=5 dan y=10
    for (int f = 0; f < 2; f++) {
        float y = floorY[f];
        drawTBeamX(-halfX, halfX, y, -halfZ, colSize);
        drawTBeamX(-halfX, halfX, y,  halfZ, colSize);
        drawTBeamZ(-halfX, -halfZ, halfZ, y, colSize);
        drawTBeamZ( halfX, -halfZ, halfZ, y, colSize);
    }

    glDisable(GL_TEXTURE_2D);

    // ==========================================
    // BIDANG MIRING DI SISI PENDEK (x = ±halfX)
    // Permukaan miring dari tepi ke tengah, di atas pilar penyangga
    // ==========================================
    float slopeWidth  = halfZ * 0.85f;  // lebar bidang miring (sebagian besar lebar)
    float slopeLowY   = 3.0f;           // ketinggian ujung bawah
    float slopeHighY  = 8.0f;           // ketinggian ujung atas (toward center)
    float slopeThick  = 0.4f;           // ketebalan bidang miring
    int   slopeSegsX  = 6;              // segmen sepanjang X (ketebalan)
    int   slopeSegsZ  = 10;             // segmen sepanjang Z (lebar)
    int   numPillars  = 5;              // jumlah pilar penyangga per sisi

    for (int side = 0; side < 2; side++) {
        float baseX = (side == 0) ? -halfX : halfX;
        float dirX  = (side == 0) ? 1.0f : -1.0f;  // arah ke tengah
        float xInset = 6.0f * dirX;  // ujung atas menjorok ke dalam

        // --- 1. Permukaan miring (solid) ---
        glColor3f(0.55f, 0.55f, 0.57f);
        for (int iz = 0; iz < slopeSegsZ; iz++) {
            float t0 = (float)iz / slopeSegsZ;
            float t1 = (float)(iz + 1) / slopeSegsZ;
            float z0 = -slopeWidth + 2.0f * slopeWidth * t0;
            float z1 = -slopeWidth + 2.0f * slopeWidth * t1;
            float yLow0  = slopeLowY + (slopeHighY - slopeLowY) * t0;
            float yLow1  = slopeLowY + (slopeHighY - slopeLowY) * t1;
            float yHigh0 = yLow0 + slopeThick;
            float yHigh1 = yLow1 + slopeThick;
            float xLow0  = baseX;
            float xHigh0 = baseX + xInset * t0;
            float xHigh1 = baseX + xInset * t1;

            glBegin(GL_QUADS);
            // sisi atas (permukaan miring)
            glVertex3f(xHigh0, yHigh0, z0);
            glVertex3f(xHigh1, yHigh1, z0);
            glVertex3f(xHigh1, yHigh1, z1);
            glVertex3f(xHigh0, yHigh0, z1);
            // sisi bawah
            glVertex3f(xLow0, yLow0, z0);
            glVertex3f(xLow0 + xInset * t1 / slopeSegsZ * slopeSegsZ,
                       yLow1, z0);
            glEnd();
        }

        // Bidang miring — satu quad besar di sisi atas
        glColor3f(0.58f, 0.58f, 0.60f);
        glBegin(GL_QUADS);
            glVertex3f(baseX,                  slopeLowY,  -slopeWidth);
            glVertex3f(baseX + xInset,         slopeHighY, -slopeWidth);
            glVertex3f(baseX + xInset,         slopeHighY,  slopeWidth);
            glVertex3f(baseX,                  slopeLowY,   slopeWidth);
        glEnd();

        // Sisi depan miring (tepi Z = -slopeWidth)
        glColor3f(0.50f, 0.50f, 0.52f);
        glBegin(GL_QUADS);
            glVertex3f(baseX,               slopeLowY,  -slopeWidth);
            glVertex3f(baseX + xInset,      slopeHighY, -slopeWidth);
            glVertex3f(baseX + xInset,      slopeHighY + slopeThick, -slopeWidth);
            glVertex3f(baseX,               slopeLowY + slopeThick,  -slopeWidth);
        glEnd();

        // Sisi belakang miring (tepi Z = +slopeWidth)
        glBegin(GL_QUADS);
            glVertex3f(baseX,               slopeLowY,  slopeWidth);
            glVertex3f(baseX + xInset,      slopeHighY, slopeWidth);
            glVertex3f(baseX + xInset,      slopeHighY + slopeThick, slopeWidth);
            glVertex3f(baseX,               slopeLowY + slopeThick,  slopeWidth);
        glEnd();

        // Sisi atas permukaan miring
        glColor3f(0.60f, 0.60f, 0.62f);
        glBegin(GL_QUADS);
            glVertex3f(baseX,               slopeLowY + slopeThick,  -slopeWidth);
            glVertex3f(baseX + xInset,      slopeHighY + slopeThick, -slopeWidth);
            glVertex3f(baseX + xInset,      slopeHighY + slopeThick,  slopeWidth);
            glVertex3f(baseX,               slopeLowY + slopeThick,   slopeWidth);
        glEnd();

        // --- 2. Pilar penyangga di bawah bidang miring ---
        glColor3f(0.38f, 0.38f, 0.40f);
        float pillarThick = 0.45f;

        for (int p = 0; p < numPillars; p++) {
            float tz = -slopeWidth + 2.0f * slopeWidth * (float)p / (numPillars - 1);
            float tFrac = (float)p / (numPillars - 1);
            float pillarTopY = slopeLowY + (slopeHighY - slopeLowY) * tFrac;
            float pillarBotY = 0.06f;  // di atas lantai hijau
            float pillarH = pillarTopY - pillarBotY;
            float pillarX = baseX + xInset * tFrac * 0.5f;

            glPushMatrix();
            glTranslatef(pillarX, pillarBotY + pillarH / 2.0f, tz);
            glScalef(pillarThick, pillarH, pillarThick);
            glutSolidCube(1.0);
            glPopMatrix();
        }

        // --- 3. Balok penyangga horizontal di bawah tepi miring ---
        glColor3f(0.42f, 0.42f, 0.44f);
        float beamDepth = 0.5f;

        // Balok bawah sepanjang Z (di bawah tepi rendah miring)
        glPushMatrix();
        glTranslatef(baseX + xInset * 0.25f, slopeLowY * 0.7f, 0.0f);
        glScalef(beamDepth, beamH, slopeWidth * 2.0f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // ==========================================
    // ATAP — KERANGKA MELENGKUNG + SENG
    // Dua sisi miring dari tepi ke center ridge
    // ==========================================
    float roofPeak  = 4.0f;   // tinggi ridge di atas bldgH
    float roofBaseY = bldgH;  // ketinggian tepi atap (di atas kolom)
    int   roofSegs  = 16;     // jumlah segmen kurva per rafter
    float arcBulge  = 1.2f;   // seberapa melengkung (overshoot kurva)
    int   numRafters = 9;     // jumlah rafter sepanjang X

    // -- Helper: titik Y pada slope dari tepi (z=±halfZ) ke center (z=0) --
    // slopeProfile(z) mengembalikan factor 0..1 (0 = di tepi, 1 = di center)
    // lalu Y = roofBaseY + roofPeak * factor + arcBulge * sin(pi * factor)

    // --- 1. SENG (metal sheet) — dua panel miring ---
    // Panel kiri: z dari -halfZ (bawah) ke 0 (atas ridge)
    // Panel kanan: z dari +halfZ (bawah) ke 0 (atas ridge)

    float sengColor[2][3] = {
        {0.45f, 0.48f, 0.52f}, // kiri — abu kebiruan
        {0.50f, 0.52f, 0.55f}  // kanan — abu lebih terang
    };

    for (int side = 0; side < 2; side++) {
        glColor3f(sengColor[side][0], sengColor[side][1], sengColor[side][2]);

        float zStart = (side == 0) ? -halfZ : halfZ;
        float zEnd   = 0.0f;
        int   segsS  = 12;

        for (int ix = 0; ix < numRafters - 1; ix++) {
            float x0 = -halfX + ix * (bldgLenX / (numRafters - 1));
            float x1 = -halfX + (ix + 1) * (bldgLenX / (numRafters - 1));

            for (int iz = 0; iz < segsS; iz++) {
                float t0 = (float)iz / segsS;
                float t1 = (float)(iz + 1) / segsS;

                float z0 = zStart + (zEnd - zStart) * t0;
                float z1 = zStart + (zEnd - zStart) * t1;

                float f0 = 1.0f - fabsf(t0 * 2.0f - 1.0f);
                float f1 = 1.0f - fabsf(t1 * 2.0f - 1.0f);
                float y0 = roofBaseY + roofPeak * t0 + arcBulge * sinf(M_PI * t0);
                float y1 = roofBaseY + roofPeak * t1 + arcBulge * sinf(M_PI * t1);

                glBegin(GL_QUADS);
                    glVertex3f(x0, y0, z0);
                    glVertex3f(x1, y0, z0);
                    glVertex3f(x1, y1, z1);
                    glVertex3f(x0, y1, z1);
                glEnd();
            }
        }
    }

    // --- 2. KERANGKA MELENGKUNG (rafters) ---
    glColor3f(0.38f, 0.38f, 0.40f);
    float beamThick = 0.35f;

    for (int ix = 0; ix < numRafters; ix++) {
        float x = -halfX + ix * (bldgLenX / (numRafters - 1));

        // Rafter sisi kiri: z dari -halfZ ke 0
        for (int iz = 0; iz < roofSegs; iz++) {
            float t0 = (float)iz / roofSegs;
            float t1 = (float)(iz + 1) / roofSegs;
            float z0 = -halfZ * (1.0f - t0);
            float z1 = -halfZ * (1.0f - t1);
            float y0 = roofBaseY + roofPeak * t0 + arcBulge * sinf(M_PI * t0);
            float y1 = roofBaseY + roofPeak * t1 + arcBulge * sinf(M_PI * t1);
            float dz = z1 - z0;
            float dy = y1 - y0;
            float segLen = sqrtf(dz * dz + dy * dy);
            float angle  = atan2f(dz, dy) * 180.0f / M_PI;

            glPushMatrix();
            glTranslatef(x, (y0 + y1) / 2.0f, (z0 + z1) / 2.0f);
            glRotatef(angle, 1.0f, 0.0f, 0.0f);
            glScalef(beamThick, segLen, beamThick);
            glutSolidCube(1.0);
            glPopMatrix();
        }

        // Rafter sisi kanan: z dari +halfZ ke 0
        for (int iz = 0; iz < roofSegs; iz++) {
            float t0 = (float)iz / roofSegs;
            float t1 = (float)(iz + 1) / roofSegs;
            float z0 = halfZ * (1.0f - t0);
            float z1 = halfZ * (1.0f - t1);
            float y0 = roofBaseY + roofPeak * t0 + arcBulge * sinf(M_PI * t0);
            float y1 = roofBaseY + roofPeak * t1 + arcBulge * sinf(M_PI * t1);
            float dz = z1 - z0;
            float dy = y1 - y0;
            float segLen = sqrtf(dz * dz + dy * dy);
            float angle  = atan2f(dz, dy) * 180.0f / M_PI;

            glPushMatrix();
            glTranslatef(x, (y0 + y1) / 2.0f, (z0 + z1) / 2.0f);
            glRotatef(angle, 1.0f, 0.0f, 0.0f);
            glScalef(beamThick, segLen, beamThick);
            glutSolidCube(1.0);
            glPopMatrix();
        }
    }

    // --- 3. RIDGE BEAM (balok puncak di z=0) ---
    glColor3f(0.45f, 0.45f, 0.47f);
    float ridgeY = roofBaseY + roofPeak;
    drawTBeamX(-halfX, halfX, ridgeY, 0.0f, beamThick);

    // --- 4. EAVE BEAMS (balok tepi di z=±halfZ) ---
    glColor3f(0.42f, 0.42f, 0.44f);
    drawTBeamX(-halfX, halfX, roofBaseY, -halfZ, beamThick);
    drawTBeamX(-halfX, halfX, roofBaseY,  halfZ, beamThick);

    // --- 5. STRUTS (penyangga miring di bawah sisi atap) ---
    glColor3f(0.35f, 0.35f, 0.37f);
    float strutThick = 0.25f;
    int   numStruts  = 5;

    for (int ix = 1; ix < numRafters; ix += 2) {
        float x = -halfX + ix * (bldgLenX / (numRafters - 1));

        // Strut sisi kiri: dari kolom bawah ke titik tengah slope
        float zMid = -halfZ * 0.5f;
        float fMid = 0.5f;
        float yMid = roofBaseY + roofPeak * fMid + arcBulge * sinf(M_PI * fMid);
        float strutDy = yMid - roofBaseY;
        float strutDz = zMid - (-halfZ);
        float strutLen = sqrtf(strutDy * strutDy + strutDz * strutDz);
        float strutAngle = atan2f(-strutDz, strutDy) * 180.0f / M_PI;

        glPushMatrix();
        glTranslatef(x, roofBaseY + strutDy / 2.0f, (-halfZ + zMid) / 2.0f);
        glRotatef(strutAngle, 1.0f, 0.0f, 0.0f);
        glScalef(strutThick, strutLen, strutThick);
        glutSolidCube(1.0);
        glPopMatrix();

        // Strut sisi kanan
        float zMidR = halfZ * 0.5f;
        float strutDyR = yMid - roofBaseY;
        float strutDzR = zMidR - halfZ;
        float strutLenR = sqrtf(strutDyR * strutDyR + strutDzR * strutDzR);
        float strutAngleR = atan2f(-strutDzR, strutDyR) * 180.0f / M_PI;

        glPushMatrix();
        glTranslatef(x, roofBaseY + strutDyR / 2.0f, (halfZ + zMidR) / 2.0f);
        glRotatef(strutAngleR, 1.0f, 0.0f, 0.0f);
        glScalef(strutThick, strutLenR, strutThick);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    glPopMatrix();
}

#endif
