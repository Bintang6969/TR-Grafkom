#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <GL/freeglut.h>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cmath>

// stb_image functions already defined in main.cpp
extern "C" {
    unsigned char *stbi_load(const char *filename, int *x, int *y, int *comp, int req_comp);
    void stbi_image_free(void *retval_from_stbi_load);
}

struct ObjVec3 { float x, y, z; };
struct ObjVec2 { float u, v; };
struct ObjFace { int v[4], vt[4], vn[4]; int count; };

struct ObjModel {
    std::vector<ObjVec3> verts;
    std::vector<ObjVec2> uvs;
    std::vector<ObjVec3> norms;
    std::vector<ObjFace> faces;
    GLuint displayList;
    GLuint textureID;
    float minX, minY, minZ, maxX, maxY, maxZ;
    float kdR, kdG, kdB;
};

// Parse satu vertex: "v/vt/vn" atau "v//vn" atau "v/vt" atau "v"
static void parseVertex(const char* s, int& v, int& vt, int& vn) {
    v = vt = vn = 0;
    vn = sscanf(s, "%d/%d/%d", &v, &vt, &vn);
    if (vn == 3) return;
    int n2 = sscanf(s, "%d//%d", &v, &vn);
    if (n2 == 2) { vt = 0; return; }
    int n3 = sscanf(s, "%d/%d", &v, &vt);
    if (n3 == 2) { vn = 0; return; }
    sscanf(s, "%d", &v);
    vt = vn = 0;
}

static bool objLoad(const char* path, ObjModel& m) {
    FILE* f = fopen(path, "r");
    if (!f) { printf("OBJ: Cannot open %s\n", path); return false; }

    char line[512];
    m.verts.reserve(50000);
    m.uvs.reserve(50000);
    m.norms.reserve(50000);
    m.faces.reserve(50000);

    m.minX = m.minY = m.minZ = 1e9f;
    m.maxX = m.maxY = m.maxZ = -1e9f;
    m.kdR = m.kdG = m.kdB = 1.0f;
    m.textureID = 0;

    int lineCount = 0;
    int quadCount = 0;
    printf("Loading OBJ: %s ... ", path);
    fflush(stdout);

    while (fgets(line, sizeof(line), f)) {
        lineCount++;
        if (line[0] == '#') continue;

        if (line[0] == 'v' && line[1] == ' ') {
            ObjVec3 v;
            if (sscanf(line + 2, "%f %f %f", &v.x, &v.y, &v.z) == 3) {
                m.verts.push_back(v);
                if (v.x < m.minX) m.minX = v.x;
                if (v.y < m.minY) m.minY = v.y;
                if (v.z < m.minZ) m.minZ = v.z;
                if (v.x > m.maxX) m.maxX = v.x;
                if (v.y > m.maxY) m.maxY = v.y;
                if (v.z > m.maxZ) m.maxZ = v.z;
            }
        }
        else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ') {
            ObjVec2 vt;
            if (sscanf(line + 3, "%f %f", &vt.u, &vt.v) == 2)
                m.uvs.push_back(vt);
        }
        else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ') {
            ObjVec3 vn;
            if (sscanf(line + 3, "%f %f %f", &vn.x, &vn.y, &vn.z) == 3)
                m.norms.push_back(vn);
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            // Parse semua vertex dalam face (bisa 3 atau 4)
            ObjFace fc;
            fc.count = 0;

            char* ptr = line + 2;
            while (*ptr && *ptr != '\n' && *ptr != '\r' && fc.count < 4) {
                while (*ptr == ' ') ptr++;
                if (*ptr == '\0' || *ptr == '\n' || *ptr == '\r') break;
                parseVertex(ptr, fc.v[fc.count], fc.vt[fc.count], fc.vn[fc.count]);
                fc.count++;
                while (*ptr && *ptr != ' ' && *ptr != '\n' && *ptr != '\r') ptr++;
            }

            if (fc.count == 3) {
                m.faces.push_back(fc);
            }
            else if (fc.count == 4) {
                // Split quad → 2 triangles: (0,1,2) dan (0,2,3)
                ObjFace t1, t2;
                t1.count = 3;
                t2.count = 3;
                for (int j = 0; j < 3; j++) {
                    t1.v[j] = fc.v[j]; t1.vt[j] = fc.vt[j]; t1.vn[j] = fc.vn[j];
                }
                t2.v[0] = fc.v[0]; t2.vt[0] = fc.vt[0]; t2.vn[0] = fc.vn[0];
                t2.v[1] = fc.v[2]; t2.vt[1] = fc.vt[2]; t2.vn[1] = fc.vn[2];
                t2.v[2] = fc.v[3]; t2.vt[2] = fc.vt[3]; t2.vn[2] = fc.vn[3];
                m.faces.push_back(t1);
                m.faces.push_back(t2);
                quadCount++;
            }
        }
    }

    fclose(f);
    printf("done! (%d verts, %d faces [%d quads split], %d lines)\n",
           (int)m.verts.size(), (int)m.faces.size(), quadCount, lineCount);
    printf("  Bounds: X[%.2f,%.2f] Y[%.2f,%.2f] Z[%.2f,%.2f]\n",
           m.minX, m.maxX, m.minY, m.maxY, m.minZ, m.maxZ);
    return true;
}

static void objCompileDisplayList(ObjModel& m) {
    m.displayList = glGenLists(1);
    glNewList(m.displayList, GL_COMPILE);

    if (m.textureID) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m.textureID);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    for (size_t i = 0; i < m.faces.size(); i++) {
        const ObjFace& f = m.faces[i];

        glBegin(GL_TRIANGLES);
        for (int j = 0; j < 3; j++) {
            int vi  = f.v[j]  - 1;
            int vti = f.vt[j] - 1;
            int vni = f.vn[j] - 1;

            if (vni >= 0 && vni < (int)m.norms.size())
                glNormal3f(m.norms[vni].x, m.norms[vni].y, m.norms[vni].z);

            if (vti >= 0 && vti < (int)m.uvs.size())
                glTexCoord2f(m.uvs[vti].u, m.uvs[vti].v);

            if (vi >= 0 && vi < (int)m.verts.size())
                glVertex3f(m.verts[vi].x, m.verts[vi].y, m.verts[vi].z);
        }
        glEnd();
    }

    if (m.textureID) {
        glDisable(GL_TEXTURE_2D);
    }

    glEndList();
}

static void objDelete(ObjModel& m) {
    if (m.displayList) glDeleteLists(m.displayList, 1);
    m.verts.clear();
    m.uvs.clear();
    m.norms.clear();
    m.faces.clear();
    m.displayList = 0;
}

// Load .mtl — ambil Kd + map_Kd
static void mtlLoad(const char* path, ObjModel& m) {
    FILE* f = fopen(path, "r");
    if (!f) { printf("MTL: Cannot open %s (using white)\n", path); return; }

    char line[256];
    char mapKd[256] = "";
    printf("Loading MTL: %s ... ", path);
    fflush(stdout);

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;
        if (strncmp(line, "Kd ", 3) == 0) {
            float r, g, b;
            if (sscanf(line + 3, "%f %f %f", &r, &g, &b) == 3) {
                m.kdR = r; m.kdG = g; m.kdB = b;
            }
        }
        else if (strncmp(line, "map_Kd ", 7) == 0) {
            sscanf(line + 7, "%255s", mapKd);
        }
    }
    fclose(f);

    printf("Kd=(%.2f,%.2f,%.2f)\n", m.kdR, m.kdG, m.kdB);

    // Load diffuse texture
    if (mapKd[0]) {
        char texPath[512];
        snprintf(texPath, sizeof(texPath), "assets/%s", mapKd);
        printf("Loading texture: %s ... ", texPath);
        fflush(stdout);

        int w, h, ch;
        unsigned char* data = stbi_load(texPath, &w, &h, &ch, 0);
        if (data) {
            GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
            glGenTextures(1, &m.textureID);
            glBindTexture(GL_TEXTURE_2D, m.textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
            gluBuild2DMipmaps(GL_TEXTURE_2D, fmt, w, h, fmt, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
            printf("done! (%dx%d, %d ch)\n", w, h, ch);
        } else {
            printf("FAILED!\n");
            m.textureID = 0;
        }
    }
}

// Cari dan load .mtl dari dalam .obj (mtllib directive)
static void objLoadMtl(const char* objPath, ObjModel& m) {
    FILE* f = fopen(objPath, "r");
    if (!f) return;

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "mtllib ", 7) == 0) {
            char mtlName[256];
            sscanf(line + 7, "%255s", mtlName);
            // Build path: same dir as obj
            char mtlPath[512];
            snprintf(mtlPath, sizeof(mtlPath), "assets/%s", mtlName);
            mtlLoad(mtlPath, m);
            break;
        }
    }
    fclose(f);
}

#endif
