#ifndef ROAD_H
#define ROAD_H

#include <GL/freeglut.h>
#include <vector>
#include <cmath>

// =============================================
// JALAN RAYA OVAL + TROTOAR (di luar stadion)
// =============================================
// Tiroar dalam : R = 81 → 85  (lebar 4m)
// Jalan aspal  : R = 85 → 97  (lebar 12m, 2 lajur)
// Tiroar luar  : R = 97 → 101 (lebar 4m)

static float roadInnerSidewalkInner = 81.0f;
static float roadInnerSidewalkOuter = 85.0f;
static float roadOuterSidewalkInner = 97.0f;
static float roadOuterSidewalkOuter = 101.0f;

// --- Helper: gambar triangle strip dari 2 boundary ---
static void drawStripFromBoundaries(
    const std::vector<Pt>& inner,
    const std::vector<Pt>& outer,
    float y)
{
    size_t n = inner.size();
    glBegin(GL_TRIANGLE_STRIP);
    for (size_t i = 0; i <= n; i++) {
        size_t idx = i % n;
        glVertex3f(outer[idx].x, y, outer[idx].z);
        glVertex3f(inner[idx].x, y, inner[idx].z);
    }
    glEnd();
}

// --- Trotoar bergaris ubin (checkerboard pattern) ---
static void drawTiledSidewalk(
    const std::vector<Pt>& inner,
    const std::vector<Pt>& outer,
    float y)
{
    size_t n = inner.size();
    for (size_t i = 0; i < n; i++) {
        size_t next = (i + 1) % n;

        // checkerboard: ganjil = abu terang, genap = abu gelap
        bool light = (i % 2 == 0);
        if (light)
            glColor3f(0.82f, 0.82f, 0.80f);
        else
            glColor3f(0.62f, 0.62f, 0.60f);

        glBegin(GL_QUADS);
            glVertex3f(inner[i].x,      y, inner[i].z);
            glVertex3f(inner[next].x,   y, inner[next].z);
            glVertex3f(outer[next].x,   y, outer[next].z);
            glVertex3f(outer[i].x,      y, outer[i].z);
        glEnd();
    }

    // List pinggir trotoar dalam (curb) — lebih tinggi sedikit
    glColor3f(0.75f, 0.75f, 0.73f);
    float curbY = y + 0.06f;
    float curbW = 0.35f;
    for (size_t i = 0; i < n; i += 2) {
        size_t next = (i + 1) % n;
        float mx1 = (inner[i].x    + outer[i].x)    / 2.0f;
        float mz1 = (inner[i].z    + outer[i].z)    / 2.0f;
        float mx2 = (inner[next].x + outer[next].x) / 2.0f;
        float mz2 = (inner[next].z + outer[next].z) / 2.0f;

        glBegin(GL_QUADS);
            glVertex3f(mx1 - curbW, curbY, mz1);
            glVertex3f(mx2 - curbW, curbY, mz2);
            glVertex3f(mx2 + curbW, curbY, mz2);
            glVertex3f(mx1 + curbW, curbY, mz1);
        glEnd();
    }
}

// --- Jalan aspal + garis-garis ---
static void drawAsphalt(
    const std::vector<Pt>& inner,
    const std::vector<Pt>& outer,
    float y)
{
    size_t n = inner.size();

    // Aspal dasar
    glColor3f(0.28f, 0.28f, 0.30f);
    drawStripFromBoundaries(inner, outer, y);

    // Garis tepi putih solid (inner & outer edge)
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < n; i++)
        glVertex3f(inner[i].x, y + 0.005f, inner[i].z);
    glEnd();

    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < n; i++)
        glVertex3f(outer[i].x, y + 0.005f, outer[i].z);
    glEnd();

    // Garis putus-putus tengah (center line)
    glColor3f(1.0f, 1.0f, 0.7f);
    glLineWidth(2.5f);
    int skip = 3;
    glBegin(GL_LINES);
    for (size_t i = 0; i < n; i++) {
        if (i % (skip * 2) >= skip) continue; // putus
        size_t next = (i + 1) % n;
        float mx1 = (inner[i].x    + outer[i].x)    / 2.0f;
        float mz1 = (inner[i].z    + outer[i].z)    / 2.0f;
        float mx2 = (inner[next].x + outer[next].x) / 2.0f;
        float mz2 = (inner[next].z + outer[next].z) / 2.0f;
        glVertex3f(mx1, y + 0.006f, mz1);
        glVertex3f(mx2, y + 0.006f, mz2);
    }
    glEnd();
}

// =============================================
// drawRoad() — panggil dari display()
// =============================================
void drawRoad()
{
    // Build boundary titik-titik oval
    std::vector<Pt> bSidewalkIn  = buildBoundary(roadInnerSidewalkInner, L);
    std::vector<Pt> bSidewalkOut = buildBoundary(roadInnerSidewalkOuter, L);
    std::vector<Pt> bRoadIn      = bSidewalkOut;  // sama dengan outer trotoar dalam
    std::vector<Pt> bRoadOut     = buildBoundary(roadOuterSidewalkInner, L);
    std::vector<Pt> bOuterSidIn  = bRoadOut;       // sama dengan outer jalan
    std::vector<Pt> bOuterSidOut = buildBoundary(roadOuterSidewalkOuter, L);

    // 1. Trotoar dalam (bergaris ubin)
    drawTiledSidewalk(bSidewalkIn, bSidewalkOut, 0.04f);

    // 2. Jalan aspal + garis
    drawAsphalt(bRoadIn, bRoadOut, 0.005f);

    // 3. Trotoar luar (bergaris ubin)
    drawTiledSidewalk(bOuterSidIn, bOuterSidOut, 0.04f);
}

#endif
