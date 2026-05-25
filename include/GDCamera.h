#ifndef GDCAMERA_HEADER
#define GDCAMERA_HEADER

#include "GDMesh.h"
#include "GDObject.h"

#define GDCAMERA_NEEDUPDATEMATRIX_FLAG (uint32_t)(1 << 0)

typedef struct GDCamera_s {
  GDObject Object;

  uint32_t Flags;
  float FOV;
  float Aspect;
  float Near;
  float Far;

  m4f ProjectionMatrix;
} GDCamera;

GDCamera GDCamera_Create(const char* id, float fov, float aspect, float near, float far);
void GDCamera_Destroy(GDCamera* this);
void GDCamera_UpdateMatrix(GDCamera* this);

#endif
/* =============================== */
#ifdef GDCAMERA_SOURCE

GDCamera GDCamera_Create(const char* id, float fov, float aspect, float near, float far) {
  GDCamera camera = {
    .Flags = GDCAMERA_NEEDUPDATEMATRIX_FLAG,

    .Object = GDObject_Create(id, GDOBJECT_TYPE_CAMERA),
    .FOV = fov,
    .Aspect = aspect,
    .Near = near,
    .Far = far
  };

  return camera;
}

void GDCamera_Destroy(GDCamera* this) {
  GDObject_Destroy(&this->Object);
}

void GDCamera_UpdateMatrix(GDCamera* this) {
  GDObject_UpdateMatrix(&this->Object);

  if (this->Flags & GDCAMERA_NEEDUPDATEMATRIX_FLAG) {
    this->Flags &= ~(GDCAMERA_NEEDUPDATEMATRIX_FLAG);

    m4f_project(&this->ProjectionMatrix, this->FOV, this->Aspect, this->Near, this->Far);
  }
}

#undef GDCAMERA_SOURCE
#endif
