#ifndef GDGEOMETRY_HEADER
#define GDGEOMETRY_HEADER

#include <GL/gl3w.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef union GDVertex_s {
  struct {
    float Position[3];
    float Normal[3];
    float UV[2];
  };
  float Buffer[8];
} GDVertex;

typedef struct GDGeometry_s {
  const char* Name;
  GLuint VAO;
  GLuint VBO;
  GLuint EBO;
  size_t IndicesCount;
} GDGeometry;

GDGeometry GDGeometry_Create(
  const char* name,
  GDVertex* vertices, size_t verticesCount,
  uint32_t* indices, size_t indicesCount);
void GDGeometry_Destroy(GDGeometry* this);
void GDGeometry_Draw(GDGeometry* this);

#endif
/* =============================== */
#ifdef GDGEOMETRY_SOURCE

#include <stddef.h>
#include <stdlib.h>

GDGeometry GDGeometry_Create(
  const char* name,
  GDVertex* vertices, size_t verticesCount,
  uint32_t* indices, size_t indicesCount) {
  GDGeometry geometry = {
    .Name = name,
    .IndicesCount = indicesCount,
    .VAO = 0,
    .VBO = 0,
    .EBO = 0
  };

  glGenVertexArrays(1, &geometry.VAO);
  glGenBuffers(1, &geometry.VBO);
  glGenBuffers(1, &geometry.EBO);

  glBindVertexArray(geometry.VAO);

  glBindBuffer(GL_ARRAY_BUFFER, geometry.VBO);
  glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeof(GDVertex), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(uint32_t), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GDVertex), (void*)offsetof(GDVertex, Position));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GDVertex), (void*)offsetof(GDVertex, Normal));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GDVertex), (void*)offsetof(GDVertex, UV));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);

  return geometry;
}

void GDGeometry_Destroy(GDGeometry* this) {
  glDeleteBuffers(1, &this->EBO);
  glDeleteBuffers(1, &this->VBO);
  glDeleteVertexArrays(1, &this->VAO);
  this->EBO = 0;
  this->VBO = 0;
  this->VAO = 0;
  this->IndicesCount = 0;
}

void GDGeometry_Draw(GDGeometry* this) {
  glBindVertexArray(this->VAO);
  glDrawElements(GL_TRIANGLES, this->IndicesCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

#undef GDGEOMETRY_SOURCE
#endif
