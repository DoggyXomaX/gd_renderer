#ifndef GDOBJLOADER_HEADER
#define GDOBJLOADER_HEADER

#include "GDGeometry.h"
#include "GDMesh.h"
#include <stdint.h>
#include <stddef.h>

typedef struct GDObjLoaderConfig_s {
} GDObjLoaderConfig;

typedef struct GDObjLoader_s {
  GDObjLoaderConfig Config;
} GDObjLoader;

GDObjLoader GDObjLoader_Create(GDObjLoaderConfig* config);
void GDObjLoader_LoadGeometry(GDObjLoader* this, const char* path, GDGeometry* geometry);

#endif
/* =============================== */
#ifdef GDOBJLOADER_SOURCE

#include <stdio.h>
#include <string.h>

GDObjLoader GDObjLoader_Create(GDObjLoaderConfig* config) {
  GDObjLoader loader;

  if (config != NULL) {
    loader.Config = *config;
  }

  return loader;
}

static inline bool isDigit(char x) {
  return (uint32_t)(x - '0') < 10;
}

static void skipSpaces(const char** p) {
  while (**p == ' ' || **p == '\t') {
    (*p)++;
  }
}

static void skipIndexSpaces(const char** p) {
  while (**p == ' ' || **p == '\t' || **p == '/') {
    (*p)++;
  }
}

static size_t lengthUntilSpace(const char* from) {
  const char* p = from;
  while (*p != ' ' && *p != '\t' && *p != '\r' && *p != '\0') {
    p++;
  }
  return (size_t)(p - from);
}

static size_t lengthUntilIndexSpace(const char* from) {
  const char* p = from;
  while (*p != ' ' && *p != '\t' && *p != '\r' && *p != '\0' && *p != '/') {
    p++;
  }
  return (size_t)(p - from);
}

static bool tryParseFloat(const char* from, const char* to, float* out) {
  *out = 0.0f;

  if (from >= to) {
    return false;
  }

  char sign = 1;
  const char* p = from;
  if (*p == '+' || *p == '-') {
    sign = *p == '-' ? -1 : 1;
    p++;
  } else if (!isDigit(*p)) {
    return false;
  }

  float value = 0.0f;
  int bytesRead = 0;
  bool isEndReached = p == to;
  while (!isEndReached && isDigit(*p)) {
    value *= 10.0f;
    value += (int)(*p - '0');
    p++;
    bytesRead++;
    isEndReached = p == to;
  }

  if (bytesRead == 0) {
    return false;
  }

  if (isEndReached || *p != '.') {
    *out = sign * value;
    return true;
  }

  p++;
  bytesRead = 1;
  isEndReached = p == to;

  while (!isEndReached && isDigit(*p)) {
    float multiplier = 1.0f;
    for (int i = 0; i < bytesRead; i++) {
      multiplier *= 0.1f;
    }

    value += (int)(*p - '0') * multiplier;
    p++;
    bytesRead++;
    isEndReached = p == to;
  }

  *out = sign * value;
  return true;
}

static void parseFloat(const char** p, float* x) {
  skipSpaces(p);

  const char* to = *p + lengthUntilSpace(*p);
  tryParseFloat(*p, to, x);
  *p = to;
}

static void parseVec2(const char** p, float* v) {
  parseFloat(p, v);
  parseFloat(p, v + 1);
}

static void parseVec3(const char** p, float* v) {
  parseFloat(p, v);
  parseFloat(p, v + 1);
  parseFloat(p, v + 2);
}

#define D(args...) printf("[DEBUG] " args)

static bool tryParseIndex(const char* from, const char* to, int* out) {
  *out = -1;

  if (from >= to) {
    return false;
  }

  const char* p = from;
  if (!isDigit(*p)) {
    return false;
  }

  int value = 0;
  int bytesRead = 0;
  bool isEndReached = p == to;
  while (!isEndReached && isDigit(*p)) {
    value *= 10;
    value += (int)(*p - '0');
    p++;
    bytesRead++;
    isEndReached = p == to;
  }

  if (bytesRead == 0) {
    return false;
  }

  *out = value - 1;
  return true;
}

static bool tryParseTriplex(const char** p, int* v) {
  skipIndexSpaces(p);

  const char* to = *p + lengthUntilIndexSpace(*p);
  if (!tryParseIndex(*p, to, v)) {
    return false;
  }
  *p = to;

  return true;
}

static void parseFaces(const char** p, int* v, size_t* triplexCount) {
  *triplexCount = 0;
  for (int i = 0; i < 12; i++) {
    if (tryParseTriplex(p, v + i)) {
      (*triplexCount)++;
    }
  }
}

void GDObjLoader_LoadGeometry(GDObjLoader* this, const char* path, GDGeometry* geometry) {
  (void)this;

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Failed to load \"%s\"!\n", path);
    return;
  }

  static float positions[65536][3];
  static float normals[65536][3];
  static float uvs[65536][2];
  static GDVertex vertices[65536];
  static uint32_t indices[65536];
  uint32_t positionsIndex = 0;
  uint32_t normalsIndex = 0;
  uint32_t uvsIndex = 0;
  uint32_t verticesIndex = 0;
  uint32_t indicesIndex = 0;

  char line[512] = { 0 };
  while (fgets(line, sizeof(line), file) != NULL) {
    const char* p = line;

    if (strncmp(p, "v ", 2) == 0) { // v 0.123456 0.123456 0.123456
      p += 2;
      parseVec3(&p, positions[positionsIndex]);
      positionsIndex++;
    } else if (strncmp(p, "vn ", 3) == 0) { // vn 0.123456 0.123456 0.123456
      p += 3;
      parseVec3(&p, normals[normalsIndex]);
      normalsIndex++;
    } else if (strncmp(p, "vt ", 3) == 0) { // vt 0.123456 0.123456
      p += 3;
      parseVec2(&p, uvs[uvsIndex]);
      uvsIndex++;
    } else if (strncmp(p, "f ", 2) == 0) {
      p += 2;

      int triplexes[12];
      size_t triplexesCount;
      parseFaces(&p, triplexes, &triplexesCount);

      uint32_t firstVertexIndex = verticesIndex;
      memcpy(vertices[verticesIndex].Position, positions[triplexes[0]], 3 * sizeof(float));
      memcpy(vertices[verticesIndex].UV, uvs[triplexes[1]], 2 * sizeof(float));
      memcpy(vertices[verticesIndex].Normal, normals[triplexes[2]], 3 * sizeof(float));
      indices[indicesIndex] = verticesIndex;
      indicesIndex++;
      verticesIndex++;

      memcpy(vertices[verticesIndex].Position, positions[triplexes[3]], 3 * sizeof(float));
      memcpy(vertices[verticesIndex].UV, uvs[triplexes[4]], 2 * sizeof(float));
      memcpy(vertices[verticesIndex].Normal, normals[triplexes[5]], 3 * sizeof(float));
      indices[indicesIndex] = verticesIndex;
      indicesIndex++;
      verticesIndex++;

      memcpy(vertices[verticesIndex].Position, positions[triplexes[6]], 3 * sizeof(float));
      memcpy(vertices[verticesIndex].UV, uvs[triplexes[7]], 2 * sizeof(float));
      memcpy(vertices[verticesIndex].Normal, normals[triplexes[8]], 3 * sizeof(float));
      indices[indicesIndex] = verticesIndex;
      indicesIndex++;
      verticesIndex++;

      if (triplexesCount == 12) {
        indices[indicesIndex] = firstVertexIndex;
        indicesIndex++;
        indices[indicesIndex] = firstVertexIndex + 2;
        indicesIndex++;

        memcpy(vertices[verticesIndex].Position, positions[triplexes[9]], 3 * sizeof(float));
        memcpy(vertices[verticesIndex].UV, uvs[triplexes[10]], 2 * sizeof(float));
        memcpy(vertices[verticesIndex].Normal, normals[triplexes[11]], 3 * sizeof(float));
        indices[indicesIndex] = verticesIndex;
        indicesIndex++;
        verticesIndex++;
      }
    }
  }

  *geometry = GDGeometry_Create(path, vertices, verticesIndex, indices, indicesIndex);
}

// TODO: LoadMeshes

#undef GDOBJLOADER_SOURCE
#endif
