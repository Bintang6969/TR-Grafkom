/* =========================================================
   MODELING STADION ATLETIK - FREEGLUT
   3 model utama:
     1. Lintasan lari (track) berbentuk oval/stadium + lapangan rumput
     2. Tribun penonton (grandstand) dengan atap miring
     3. Pepohonan di sekeliling stadion
   Kamera bisa digerakkan bebas (fly camera)
   ========================================================= */

#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <cstdio>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------- Struct titik 2D (dipakai untuk denah, x = X dunia, z = Z dunia) ----------
struct Pt { float x, z; };

// ---------- Ukuran stadion ----------
float R_in   = 45.0f;   // radius setengah lingkaran bagian DALAM (lapangan)
float trackW = 12.0f;   // lebar lintasan lari
float R_out  = R_in + trackW; // radius luar lintasan
float L      = 130.0f;  // panjang bagian lurus (jarak antar pusat lingkaran)

// ---------- Lapangan sepak bola berbentuk persegi panjang, di dalam oval lintasan ----------
float pitchHalfLen = 60.0f; // setengah panjang lapangan (arah X)
float pitchHalfWid = 40.0f; // setengah lebar lapangan (arah Z)
float goalX  = pitchHalfLen; // gawang tepat di garis gawang (ujung pendek persegi panjang)

int segCircle  = 40; // jumlah segmen tiap setengah lingkaran
int segStraight= 20; // jumlah segmen tiap sisi lurus

vector<Pt> innerPts; // batas dalam lintasan / tepi lapangan
vector<Pt> outerPts; // batas luar lintasan
GLuint wallTexture;
// ---------- Kamera (fly camera) ----------
float camX = 0.0f, camY = 25.0f, camZ = 140.0f;
float yaw   = -90.0f; // hadap ke -Z awalnya
float pitch = -12.0f;
float moveSpeed = 0.02f;
float turnSpeed = 0.06f;

// status tombol
bool key_w=false, key_s=false, key_a=false, key_d=false, key_q=false, key_e=false;
bool key_left=false, key_right=false, key_up=false, key_down=false;

// =========================================================
// Membentuk denah stadion (stadium shape / discorectangle)
// Dipakai 2x dengan radius berbeda (R_in untuk lapangan, R_out untuk luar lintasan)
// supaya titik-titiknya sejajar index-nya (buat triangle strip)
// =========================================================
vector<Pt> buildBoundary(float R, float Lstraight)
{
    vector<Pt> pts;
    // 1) setengah lingkaran kanan, sudut -90 -> 90 derajat
    for (int i = 0; i <= segCircle; i++) {
        float t = -M_PI/2.0f + (float)M_PI * i / segCircle;
        Pt p; p.x = Lstraight/2.0f + R*cos(t); p.z = R*sin(t);
        pts.push_back(p);
    }
    // 2) sisi lurus atas: dari (L/2, R) ke (-L/2, R)
    for (int i = 1; i <= segStraight; i++) {
        float frac = (float)i / segStraight;
        Pt p; p.x = Lstraight/2.0f - Lstraight*frac; p.z = R;
        pts.push_back(p);
    }
    // 3) setengah lingkaran kiri, sudut 90 -> 270 derajat
    for (int i = 0; i <= segCircle; i++) {
        float t = M_PI/2.0f + (float)M_PI * i / segCircle;
        Pt p; p.x = -Lstraight/2.0f + R*cos(t); p.z = R*sin(t);
        pts.push_back(p);
    }
    // 4) sisi lurus bawah: dari (-L/2,-R) ke (L/2,-R)
    for (int i = 1; i <= segStraight; i++) {
        float frac = (float)i / segStraight;
        Pt p; p.x = -Lstraight/2.0f + Lstraight*frac; p.z = -R;
        pts.push_back(p);
    }
    return pts;
}

// =========================================================
// Rumput bermotif garis potong rumput (mowing stripes) di dalam lapangan persegi panjang
// =========================================================
void drawGrassStripes()
{
    int stripes = 16;
    float stripeW = (2.0f*pitchHalfLen) / stripes;

    for (int s = 0; s < stripes; s++) {
        float x0 = -pitchHalfLen + s*stripeW;
        float x1 = x0 + stripeW;

        if (s % 2 == 0) glColor3f(0.17f, 0.60f, 0.17f);
        else            glColor3f(0.13f, 0.50f, 0.13f);

        glBegin(GL_QUADS);
            glVertex3f(x0, 0.021f, -pitchHalfWid);
            glVertex3f(x1, 0.021f, -pitchHalfWid);
            glVertex3f(x1, 0.021f,  pitchHalfWid);
            glVertex3f(x0, 0.021f,  pitchHalfWid);
        glEnd();
    }
}

// =========================================================
// Kotak gawang, kotak penalti, dan titik penalti di satu ujung lapangan
// goalLineX = posisi garis gawang, inward = arah menuju tengah lapangan (+1/-1)
// =========================================================
void drawGoalBoxes(float goalLineX, float inward)
{
    float smallHalfW = 7.0f,  smallDepth = 5.0f;  // kotak gawang (goal area)
    float bigHalfW   = 10.0f, bigDepth   = 14.0f;  // kotak penalti (penalty area)
    float spotOffset = 9.0f;                        // jarak titik penalti dari garis gawang

    glColor3f(1,1,1);
    glLineWidth(2.0f);

    // kotak gawang
    glBegin(GL_LINE_LOOP);
        glVertex3f(goalLineX,                     0.03f, -smallHalfW);
        glVertex3f(goalLineX + inward*smallDepth,  0.03f, -smallHalfW);
        glVertex3f(goalLineX + inward*smallDepth,  0.03f,  smallHalfW);
        glVertex3f(goalLineX,                      0.03f,  smallHalfW);
    glEnd();

    // kotak penalti
    glBegin(GL_LINE_LOOP);
        glVertex3f(goalLineX,                    0.03f, -bigHalfW);
        glVertex3f(goalLineX + inward*bigDepth,   0.03f, -bigHalfW);
        glVertex3f(goalLineX + inward*bigDepth,   0.03f,  bigHalfW);
        glVertex3f(goalLineX,                     0.03f,  bigHalfW);
    glEnd();

    // titik penalti
    glPointSize(4.0f);
    glBegin(GL_POINTS);
        glVertex3f(goalLineX + inward*spotOffset, 0.03f, 0.0f);
    glEnd();
}

// =========================================================
// Semua garis putih penanda lapangan sepak bola
// =========================================================
void drawFieldLines()
{
    glColor3f(1,1,1);
    glLineWidth(2.0f);

    // garis pinggir lapangan (touchline) - persegi panjang
    glBegin(GL_LINE_LOOP);
        glVertex3f(-pitchHalfLen, 0.03f, -pitchHalfWid);
        glVertex3f( pitchHalfLen, 0.03f, -pitchHalfWid);
        glVertex3f( pitchHalfLen, 0.03f,  pitchHalfWid);
        glVertex3f(-pitchHalfLen, 0.03f,  pitchHalfWid);
    glEnd();

    // lingkaran tengah
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i <= 40; i++) {
            float t = 2.0f*M_PI*i/40;
            glVertex3f(9.0f*cos(t), 0.03f, 9.0f*sin(t));
        }
    glEnd();

    // titik tengah
    glPointSize(4.0f);
    glBegin(GL_POINTS);
        glVertex3f(0.0f, 0.03f, 0.0f);
    glEnd();

    // garis tengah (halfway line) tegak lurus sumbu panjang lapangan
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.03f, -pitchHalfWid);
        glVertex3f(0.0f, 0.03f,  pitchHalfWid);
    glEnd();

    // kotak gawang & kotak penalti di kedua ujung lapangan
    drawGoalBoxes( goalX, -1.0f);
    drawGoalBoxes(-goalX,  1.0f);
}

// =========================================================
// MODEL 1: Lapangan rumput (poligon hijau, cembung jadi aman pakai GL_POLYGON)
// =========================================================
void drawField()
{
    // dasar rumput hijau
    glColor3f(0.15f, 0.55f, 0.15f);
    glBegin(GL_POLYGON);
        for (size_t i = 0; i < innerPts.size(); i++)
            glVertex3f(innerPts[i].x, 0.02f, innerPts[i].z);
    glEnd();

    // motif rumput bergaris (mowing stripes)
    drawGrassStripes();

    // semua garis putih lapangan sepak bola
    drawFieldLines();
}

// =========================================================
// MODEL 1b: Lintasan lari (ring merah-bata, triangle strip antara outer & inner)
// =========================================================
void drawTrack()
{
    glColor3f(0.72f, 0.25f, 0.18f); // warna merah bata khas lintasan atletik
    glBegin(GL_TRIANGLE_STRIP);
        size_t n = outerPts.size();
        for (size_t i = 0; i <= n; i++) {
            size_t idx = i % n;
            glVertex3f(outerPts[idx].x, 0.01f, outerPts[idx].z);
            glVertex3f(innerPts[idx].x, 0.01f, innerPts[idx].z);
        }
    glEnd();

    // garis lane putih di tengah lintasan
    glColor3f(1,1,1);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        for (size_t i = 0; i < n; i++) {
            float mx = (outerPts[i].x + innerPts[i].x)/2.0f;
            float mz = (outerPts[i].z + innerPts[i].z)/2.0f;
            glVertex3f(mx, 0.04f, mz);
        }
    glEnd();
}

// gawang lengkap: 2 tiang + mistar + jaring (net) belakang/samping/atas
void drawGoal(float x, float z, float dirX)
{
    float postZ = 3.5f, postH = 2.4f, netDepth = 1.3f;

    glPushMatrix();
    glTranslatef(x, 0, z);

    // tiang & mistar
    glColor3f(1,1,1);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex3f(0,0,-postZ); glVertex3f(0,postH,-postZ);
        glVertex3f(0,0, postZ); glVertex3f(0,postH, postZ);
        glVertex3f(0,postH,-postZ); glVertex3f(0,postH,postZ);
    glEnd();

    // jaring gawang (net): panel belakang, dua panel samping, dan panel atas
    float xb = dirX * netDepth; // sisi belakang gawang, menjauhi tengah lapangan
    int stepsZ = 8, stepsY = 5, stepsX = 4;

    glColor3f(0.92f, 0.92f, 0.92f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        // panel belakang (bidang z-y di x = xb)
        for (int i = 0; i <= stepsZ; i++) {
            float zz = -postZ + (2*postZ)*i/stepsZ;
            glVertex3f(xb, 0, zz); glVertex3f(xb, postH, zz);
        }
        for (int j = 0; j <= stepsY; j++) {
            float yy = postH*j/stepsY;
            glVertex3f(xb, yy, -postZ); glVertex3f(xb, yy, postZ);
        }
        // panel samping kiri & kanan (bidang x-y di z = -postZ dan z = postZ)
        for (int i = 0; i <= stepsX; i++) {
            float xx = xb*i/stepsX;
            glVertex3f(xx, 0, -postZ); glVertex3f(xx, postH, -postZ);
            glVertex3f(xx, 0,  postZ); glVertex3f(xx, postH,  postZ);
        }
        for (int j = 0; j <= stepsY; j++) {
            float yy = postH*j/stepsY;
            glVertex3f(0, yy, -postZ); glVertex3f(xb, yy, -postZ);
            glVertex3f(0, yy,  postZ); glVertex3f(xb, yy,  postZ);
        }
        // panel atas (bidang x-z di y = postH)
        for (int i = 0; i <= stepsX; i++) {
            float xx = xb*i/stepsX;
            glVertex3f(xx, postH, -postZ); glVertex3f(xx, postH, postZ);
        }
        for (int i = 0; i <= stepsZ; i++) {
            float zz = -postZ + (2*postZ)*i/stepsZ;
            glVertex3f(0, postH, zz); glVertex3f(xb, postH, zz);
        }
    glEnd();

    glPopMatrix();
}

// =========================================================
// MODEL 2: Tribun penonton (grandstand) - anak tangga bertingkat + atap miring
// =========================================================
void drawKridanggoStand(float side, bool isMain)

{

    float standX = 75.0f;

    float baseZ  = side * (R_out + 6.0f);

    int tiers = 12;

    float tierH = 0.8f, tierD = 1.4f;

    float depth = tiers * tierD;

    float lorongHeight = 4.0f;

    float blockW = standX / 3.0f;



    glPushMatrix();

    glTranslatef(0.0f, 0.0f, baseZ);

    if (side < 0) glRotatef(180, 0, 1, 0);



    // 1. Tangga & Lorong

    for (int i = 0; i < tiers; i++) {

        float currentH = (i + 1) * tierH;

        float zCenter  = (i * tierD) + (tierD / 2.0f);



        // Sayap Kiri & Kanan

        if (isMain) glColor3f(0.8f, 0.2f, 0.22f); else glColor3f(0.3f, 0.7f, 0.4f);

        glPushMatrix();

        glTranslatef(-blockW, currentH/2.0f, zCenter);

        glScalef(blockW, currentH, tierD);

        glutSolidCube(1.0);

        glPopMatrix();



        glPushMatrix();

        glTranslatef(blockW, currentH/2.0f, zCenter);

        glScalef(blockW, currentH, tierD);

        glutSolidCube(1.0);

        glPopMatrix();



        // Tengah (Ada Lorong)

        bool isTunnelArea = (currentH <= lorongHeight);

        bool isCommentatorBoxArea = (isMain && i >= tiers - 4);



        if (!isTunnelArea && !isCommentatorBoxArea) {

            if (isMain) glColor3f(0.9f, 0.85f, 0.1f); else glColor3f(0.3f, 0.7f, 0.4f);

            float centerBlockH = currentH - lorongHeight;

            float centerBlockY = lorongHeight + (centerBlockH / 2.0f);

            glPushMatrix();

            glTranslatef(0.0f, centerBlockY, zCenter);

            glScalef(blockW, centerBlockH, tierD);

            glutSolidCube(1.0);

            glPopMatrix();

        }

    }



    // 2. FIXED COMMENTATOR BOX HEIGHT

    if (isMain) {

        float boxDepth = 4 * tierD;

        float boxZ = (tiers - 4) * tierD + (boxDepth / 2.0f);

        float boxH = 4.0f; // Diturunkan dari 4.0 ke 3.0

        float baseFloor = lorongHeight + ((tiers - 10) * tierH);

        float boxY = baseFloor + (boxH / 2.0f);



        glPushMatrix();

        glTranslatef(0.0f, boxY, boxZ);

        glColor3f(0.85f, 0.92f, 0.88f);

        glPushMatrix();

        glScalef(blockW, boxH, boxDepth);

        glutSolidCube(1.0);

        glPopMatrix();



        // Kaca Depan

        glColor3f(0.3f, 0.7f, 0.9f); // Biru jendela

        glPushMatrix();

        glTranslatef(0.0f, 0.5f, -boxDepth/2.0f - 0.05f);

        glScalef(blockW - 1.5f, boxH - 1.5f, 0.1f);

        glutSolidCube(1.0);

        glPopMatrix();



        // Bingkai Kaca

        glColor3f(0.15f, 0.15f, 0.15f);

        glLineWidth(2.0f);

        glPushMatrix();

        glTranslatef(0.0f, 0.5f, -boxDepth/2.0f - 0.05f);

        glScalef(blockW - 1.5f, boxH - 1.5f, 0.1f);

        glutWireCube(1.01f);

        glPopMatrix();



        glPopMatrix();

    }





    // 3. FIXED CLIPPING (Offset 0.05f)

    float maxStairH = tiers * tierH;

    glColor3f(0.85f, 0.92f, 0.88f);



    // Tembok Belakang (Offset 0.05 agar tidak clipping dengan tangga)

    glPushMatrix();

    glTranslatef(0.0f, maxStairH/2.0f, depth - 0.5f + 0.05f);

    glScalef(standX, maxStairH, 1.0f);

    glutSolidCube(1.0);

    glPopMatrix();



    // Samping (Offset 0.05)

    glPushMatrix();

    glTranslatef(-standX/2.0f + 0.5f - 0.05f, maxStairH/2.0f, depth/2.0f);

    glScalef(1.0f, maxStairH, depth);

    glutSolidCube(1.0);

    glPopMatrix();



    glPushMatrix();

    glTranslatef(standX/2.0f - 0.5f + 0.05f, maxStairH/2.0f, depth/2.0f);

    glScalef(1.0f, maxStairH, depth);

    glutSolidCube(1.0);

    glPopMatrix();



    // --- 4. GAP TERBUKA & ATAP KANTILEVER (Kotak Biru) ---

    float gapHeight = 2.8f; // Celah terbuka untuk sirkulasi udara

    float roofY = maxStairH + gapHeight;



    // Tiang penyangga (hanya berada di dalam area celah terbuka)

    glColor3f(0.35f, 0.35f, 0.35f);

    for (float px = -standX/2.0f + 3.0f; px <= standX/2.0f; px += (standX-6.0f)/5.0f) {

        glPushMatrix();

        glTranslatef(px, maxStairH + (gapHeight/2.0f), depth - 1.0f);

        glScalef(0.6f, gapHeight, 0.6f);

        glutSolidCube(1.0);

        glPopMatrix();

    }



    // Atap

    float roofDepth = depth + 12.0f;

    float roofZ = depth/2.0f - 2.0f;

    float roofX = standX + 4.0f;



    // Seng Atas (Karat/Merah Tua)

    glColor3f(0.55f, 0.35f, 0.3f);

    glPushMatrix();

    glTranslatef(0.0f, roofY + 0.2f, roofZ);

    glRotatef(7.0f, 1, 0, 0);

    glScalef(roofX, 0.2f, roofDepth);

    glutSolidCube(1.0);

    glPopMatrix();



    // Plafon Bawah (Putih)

    glColor3f(0.88f, 0.88f, 0.88f);

    glPushMatrix();

    glTranslatef(0.0f, roofY, roofZ);

    glRotatef(7.0f, 1, 0, 0);

    glScalef(roofX, 0.6f, roofDepth);

    glutSolidCube(1.0);

    glPopMatrix();



    glPopMatrix();

}

// =========================================================
// MODEL 3: Pohon (kerucut + silinder sebagai batang)
// =========================================================
void drawTree(float x, float z, float scale)
{
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(scale, scale, scale);

    // batang
    glColor3f(0.40f, 0.26f, 0.13f);
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        GLUquadric* q1 = gluNewQuadric();
        gluCylinder(q1, 0.35f, 0.3f, 3.0f, 8, 4);
        gluDeleteQuadric(q1);
    glPopMatrix();

    // daun (3 tingkat kerucut biar rimbun)
    glColor3f(0.10f, 0.45f, 0.15f);
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(0, 3.0f + i*1.4f, 0);
        glRotatef(-90, 1, 0, 0);
        glutSolidCone(1.6f - i*0.35f, 2.0f, 10, 4);
        glPopMatrix();
    }
    glPopMatrix();
}

// helper kecil: batas X aman biar pohon tidak nabrak tribun
float standXHalfSafe() { return 50.0f; }

void drawTreesAround()
{
    vector<Pt> ring = buildBoundary(R_out + 7.0f, L);
    for (size_t i = 0; i < ring.size(); i += 3) {
        // lewati area yang ketutup tribun (kedua sisi Z, dekat tengah)
        if (fabs(ring[i].z) > R_out - 2.0f && fabs(ring[i].x) < standXHalfSafe()) continue;
        float s = 1.0f + 0.3f * sin(i*0.7f);
        drawTree(ring[i].x, ring[i].z, s);
    }
}

// =========================================================
// MODEL 4: Tembok pembatas di sekeliling stadion
// =========================================================
void drawWall()
{
float wallR = R_out + 20.0f;
    float wallH = 4.5f;
    float wallThick = 0.6f;
    vector<Pt> wallPts = buildBoundary(wallR, L);
    size_t n = wallPts.size();

    // Aktifkan Tekstur
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    // Ulangi tekstur berdasarkan keliling (tiling)
    float tilingFactor = 15.0f;

    glBegin(GL_QUAD_STRIP);
    for (size_t i = 0; i <= n; i++) {
        size_t idx = i % n;

        // Menghitung U agar proporsional dengan panjang wallPts
        float u = (float)i / (float)n * tilingFactor;

        // Vertex Atas (v=1.0)
        glTexCoord2f(u, 1.0f);
        glVertex3f(wallPts[idx].x, wallH, wallPts[idx].z);

        // Vertex Bawah (v=0.0)
        glTexCoord2f(u, 0.0f);
        glVertex3f(wallPts[idx].x, 0.0f, wallPts[idx].z);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    // pilar-pilar penguat sepanjang tembok
    glColor3f(0.2f, 0.66f, 0.32f);
    for (size_t i = 0; i < n; i += 4) {
        glPushMatrix();
        glTranslatef(wallPts[i].x, wallH/2.0f, wallPts[i].z);
        glScalef(wallThick*2.0f, wallH, wallThick*2.0f);
        glutSolidCube(1.0);
        glPopMatrix();
    }

    // list/pinggiran atas tembok
    glColor3f(0.85f, 0.83f, 0.78f);
    glBegin(GL_QUAD_STRIP);
        for (size_t i = 0; i <= n; i++) {
            size_t idx = i % n;
            glVertex3f(wallPts[idx].x, wallH + 0.25f, wallPts[idx].z);
            glVertex3f(wallPts[idx].x, wallH,          wallPts[idx].z);
        }
    glEnd();
}

// =========================================================
// Lantai / tanah dasar dan langit
// =========================================================
void drawGround()
{
    glColor3f(0.55f, 0.55f, 0.5f);
    glBegin(GL_QUADS);
        glVertex3f(-400, -0.05f, -400);
        glVertex3f( 400, -0.05f, -400);
        glVertex3f( 400, -0.05f,  400);
        glVertex3f(-400, -0.05f,  400);
    glEnd();
}

// =========================================================
// Update posisi kamera berdasarkan tombol yang sedang ditekan
// =========================================================
void updateCamera()
{
    if (key_left)  yaw   -= turnSpeed;
    if (key_right) yaw   += turnSpeed;
    if (key_up)    pitch += turnSpeed;
    if (key_down)  pitch -= turnSpeed;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    float radYaw = yaw * M_PI / 180.0f;

    // arah hadap (forward) di bidang X-Z
    float fwdX = cos(radYaw);
    float fwdZ = sin(radYaw);
    // arah kanan (strafe) tegak lurus forward
    float rightX = cos(radYaw + M_PI/2.0f);
    float rightZ = sin(radYaw + M_PI/2.0f);

    if (key_w) { camX += fwdX*moveSpeed; camZ += fwdZ*moveSpeed; }
    if (key_s) { camX -= fwdX*moveSpeed; camZ -= fwdZ*moveSpeed; }
    if (key_d) { camX += rightX*moveSpeed; camZ += rightZ*moveSpeed; }
    if (key_a) { camX -= rightX*moveSpeed; camZ -= rightZ*moveSpeed; }
    if (key_e) { camY += moveSpeed; }
    if (key_q) { camY -= moveSpeed; if (camY < 1.0f) camY = 1.0f; }
}

// =========================================================
// Display
// =========================================================
void display()
{
    glClearColor(0.55f, 0.75f, 0.92f, 1.0f); // langit biru cerah
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float radYaw   = yaw   * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;
    float lookX = camX + cos(radYaw) * cos(radPitch);
    float lookY = camY + sin(radPitch);
    float lookZ = camZ + sin(radYaw) * cos(radPitch);

    gluLookAt(camX, camY, camZ,
              lookX, lookY, lookZ,
              0.0f, 1.0f, 0.0f);

    drawGround();
    drawField();
    drawTrack();
    // gawang diletakkan di pinggir lapangan (ujung lengkung), pas di dekat garis merah lintasan
    drawGoal( goalX, 0.0f,  1.0f); // jaring menjorok ke arah +X (menjauhi tengah)
    drawGoal(-goalX, 0.0f, -1.0f); // jaring menjorok ke arah -X (menjauhi tengah)
    drawKridanggoStand(1.0f, true);   // Tribun Utama (Merah-Kuning)
    drawKridanggoStand(-1.0f, false); // Tribun Seberang (Hijau)
    drawTreesAround();
    drawWall();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w/(double)h, 0.5, 1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void idle()
{
    updateCamera();
    glutPostRedisplay();
}

// =========================================================
// Input keyboard
// =========================================================
void keyDown(unsigned char key, int, int)
{
    switch (key) {
        case 'w': case 'W': key_w = true; break;
        case 's': case 'S': key_s = true; break;
        case 'a': case 'A': key_a = true; break;
        case 'd': case 'D': key_d = true; break;
        case 'q': case 'Q': key_q = true; break;
        case 'e': case 'E': key_e = true; break;
        case 27: exit(0); break; // ESC keluar
    }
}
void keyUp(unsigned char key, int, int)
{
    switch (key) {
        case 'w': case 'W': key_w = false; break;
        case 's': case 'S': key_s = false; break;
        case 'a': case 'A': key_a = false; break;
        case 'd': case 'D': key_d = false; break;
        case 'q': case 'Q': key_q = false; break;
        case 'e': case 'E': key_e = false; break;
    }
}
void specialDown(int key, int, int)
{
    if (key == GLUT_KEY_LEFT)  key_left  = true;
    if (key == GLUT_KEY_RIGHT) key_right = true;
    if (key == GLUT_KEY_UP)    key_up    = true;
    if (key == GLUT_KEY_DOWN)  key_down  = true;
}
void specialUp(int key, int, int)
{
    if (key == GLUT_KEY_LEFT)  key_left  = false;
    if (key == GLUT_KEY_RIGHT) key_right = false;
    if (key == GLUT_KEY_UP)    key_up    = false;
    if (key == GLUT_KEY_DOWN)  key_down  = false;
}

// Pastikan fungsi ini dipanggil di dalam init()
void initTexture() {
    // Buat texture checkerboard sebagai fallback jika file tidak ada
    const int texWidth = 256;
    const int texHeight = 256;
    unsigned char* textureData = new unsigned char[texWidth * texHeight * 3];

    // Coba load dari file dulu
    int width, height, nrChannels;
    unsigned char *data = stbi_load("D:\\Kridanggo kridanggo daikazoku\\wall_texture.jpg", &width, &height, &nrChannels, 0);

    glGenTextures(1, &wallTexture);
    glBindTexture(GL_TEXTURE_2D, wallTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data) {
        printf("Texture loaded successfully: %dx%d, channels: %d\n", width, height, nrChannels);

        // Tentukan format berdasarkan jumlah channel
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        gluBuild2DMipmaps(GL_TEXTURE_2D, format, width, height, format, GL_UNSIGNED_BYTE, data);
    } else {
        printf("Failed to load texture, using checkerboard pattern\n");

        // Buat pola checkerboard
        for (int y = 0; y < texHeight; y++) {
            for (int x = 0; x < texWidth; x++) {
                int index = (y * texWidth + x) * 3;
                bool isWhite = ((x / 32) + (y / 32)) % 2 == 0;

                if (isWhite) {
                    textureData[index] = 200;     // R
                    textureData[index + 1] = 200; // G
                    textureData[index + 2] = 200; // B
                } else {
                    textureData[index] = 100;     // R
                    textureData[index + 1] = 100; // G
                    textureData[index + 2] = 100; // B
                }
            }
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texWidth, texHeight, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    }

    stbi_image_free(data);
    delete[] textureData;
}

// =========================================================
void init()
{
    glEnable(GL_DEPTH_TEST);
    innerPts = buildBoundary(R_in,  L);
    outerPts = buildBoundary(R_out, L);
    initTexture();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 700);
    glutCreateWindow("Modeling Stadion - Freeglut (WASD + Panah + Q/E)");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);

    printf("Kontrol kamera:\n");
    printf(" W/A/S/D : maju/kiri/mundur/kanan\n");
    printf(" Q/E     : turun/naik\n");
    printf(" Panah   : lihat kiri/kanan/atas/bawah\n");
    printf(" ESC     : keluar\n");

    glutMainLoop();
    return 0;
}
