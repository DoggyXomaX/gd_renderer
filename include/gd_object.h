#ifndef GDOBJECT_HEADER
#define GDOBJECT_HEADER

#include "v3f.h"
#include "v4f.h"
#include "m4f.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define GDOBJECT_TYPE_EMPTY 0
#define GDOBJECT_TYPE_CAMERA 1
#define GDOBJECT_TYPE_MESH 2

#define GDOBJECT_NEEDUPDATEMATRIX_FLAG (uint32_t)(1 << 0)

#define GDOBJECT_MAX_CHILDREN (size_t)16

typedef struct GDObject_s {
  uint32_t Type;
  uint32_t Flags;

  const char* ID;
  v3f Position;
  v4f Rotation;
  v3f Scale;
  m4f Matrix;
  m4f WorldMatrix;

  struct GDObject_s* Parent;
  struct GDObject_s* Children[GDOBJECT_MAX_CHILDREN];
  size_t ChildrenCount;
} GDObject;

GDObject GDObject_Create(const char* id, uint32_t type);
void GDObject_Destroy(GDObject* this);
void GDObject_AddChild(GDObject* this, GDObject* child);
void GDObject_RemoveChild(GDObject* this, GDObject* child);
GDObject* GDObject_GetByID(GDObject* this, const char* id);
void GDObject_RemoveByID(GDObject* this, const char* id);
void GDObject_SetParent(GDObject* this, GDObject* parent);
void GDObject_SetPosition(GDObject* this, float x, float y, float z);
void GDObject_SetScale(GDObject* this, float x, float y, float z);
void GDObject_SetRotation(GDObject* this, float x, float y, float z, float w);
void GDObject_SetEuler(GDObject* this, float pitch, float yaw, float roll);
v3f GDObject_GetForward(GDObject* this);
v3f GDObject_GetRight(GDObject* this);
v3f GDObject_GetUp(GDObject* this);
void GDObject_UpdateMatrix(GDObject* this);
void GDObject_MarkDirty(GDObject* this);

#endif
/* =============================== */
#ifdef GDOBJECT_SOURCE

#ifndef V3_SOURCE
#define V3_SOURCE
#include "v3.h"
#endif

#ifndef V4_SOURCE
#define V4_SOURCE
#include "v4.h"
#endif

#ifndef M4_SOURCE
#define M4_SOURCE
#include "m4.h"
#endif

#include <stddef.h>

GDObject GDObject_Create(const char* id, uint32_t type) {
  GDObject newObject = {
    .Type = type,
    .Flags = GDOBJECT_NEEDUPDATEMATRIX_FLAG,

    .ID = id,
    .Position = v3f_new_0(),
    .Scale = v3f_new_1(),
    .Rotation = v4f_new(0, 0, 0, 1),
    .Matrix = m4f_new_identity(),
    .WorldMatrix = m4f_new_identity(),

    .Parent = NULL,
    .ChildrenCount = 0,
  };

  return newObject;
}

void GDObject_Destroy(GDObject* this) {
  this->Parent = NULL;
  this->ChildrenCount = 0;
}

void GDObject_AddChild(GDObject* this, GDObject* child) {
  if (this->ChildrenCount >= GDOBJECT_MAX_CHILDREN) {
    fprintf(stderr, "Failed to add child to \"%s\": max child count exceeded!\n", this->ID);
    return;
  }

  this->Children[this->ChildrenCount] = child;
  this->ChildrenCount++;

  child->Parent = this;
}

void GDObject_RemoveChild(GDObject* this, GDObject* child) {
  for (size_t i = 0; i < this->ChildrenCount; i++) {
    if (this->Children[i] != child)
      continue;
    this->Children[i] = this->Children[this->ChildrenCount - 1];
    this->ChildrenCount--;
  }
}

GDObject* GDObject_GetByID(GDObject* this, const char* id) {
  for (size_t i = 0; i < this->ChildrenCount; i++) {
    if (strcmp(this->Children[i]->ID, id) == 0) {
      return this->Children[i];
    }
  }
  return NULL;
}

void GDObject_RemoveByID(GDObject* this, const char* id) {
  GDObject* child = GDObject_GetByID(this, id);
  GDObject_RemoveChild(this, child);
}

void GDObject_SetParent(GDObject* this, GDObject* parent) {
  this->Parent = parent;
  GDObject_MarkDirty(this);
}

void GDObject_SetPosition(GDObject* this, float x, float y, float z) {
  v3f_set(&this->Position, x, y, z);
  GDObject_MarkDirty(this);
}

void GDObject_SetScale(GDObject* this, float x, float y, float z) {
  v3f_set(&this->Scale, x, y, z);
  GDObject_MarkDirty(this);
}

void GDObject_SetRotation(GDObject* this, float x, float y, float z, float w) {
  v4f_set(&this->Rotation, x, y, z, w);
  GDObject_MarkDirty(this);
}

void GDObject_SetEuler(GDObject* this, float pitch, float roll, float yaw) {
  v3f euler = { .x = pitch, .y = roll, .z = yaw };
  v4f_euler(&this->Rotation, &euler);
  GDObject_MarkDirty(this);
}

v3f GDObject_GetForward(GDObject* this) {
  m4f* m = &this->WorldMatrix;
  return v3f_new(m->a02, m->a12, m->a22);
}

v3f GDObject_GetRight(GDObject* this) {
  m4f* m = &this->WorldMatrix;
  return v3f_new(m->a00, m->a10, m->a20);
}

v3f GDObject_GetUp(GDObject* this) {
  m4f* m = &this->WorldMatrix;
  return v3f_new(m->a01, m->a11, m->a21);
}

void GDObject_UpdateMatrix(GDObject* this) {
  printf("Update matrix\n");
  // Create local matrix
  if (this->Flags & GDOBJECT_NEEDUPDATEMATRIX_FLAG) {
    this->Flags &= ~(GDOBJECT_NEEDUPDATEMATRIX_FLAG);

    m4f_compose(&this->Matrix, &this->Position, &this->Rotation, &this->Scale);
    if (this->Parent != NULL) {
      if (this->Type & GDOBJECT_TYPE_CAMERA) {
        m4f_premul(&this->WorldMatrix, &this->Parent->WorldMatrix, &this->Matrix);
      } else {
        m4f_mul(&this->WorldMatrix, &this->Parent->WorldMatrix, &this->Matrix);
      }
    } else {
      m4f_copy(&this->WorldMatrix, &this->Matrix);
    }
  }

  for (size_t i = 0; i < this->ChildrenCount; i++) {
    GDObject_UpdateMatrix(this->Children[i]);
  }
}

void GDObject_MarkDirty(GDObject* this) {
  this->Flags |= GDOBJECT_NEEDUPDATEMATRIX_FLAG;
  for (size_t i = 0; i < this->ChildrenCount; i++) {
    GDObject_MarkDirty(this->Children[i]);
  }
}

#undef GDOBJECT_SOURCE
#endif
