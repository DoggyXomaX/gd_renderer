#ifndef GDMESH_HEADER
#define GDMESH_HEADER

#include "gd_object.h"
#include "gd_material.h"
#include "gd_geometry.h"

typedef struct GDMesh_s {
  GDObject Object;

  GDMaterial* Material;
  GDGeometry* Geometry;
} GDMesh;

GDMesh GDMesh_Create(const char* id, GDGeometry* geometry, GDMaterial* material);
void GDMesh_Destroy(GDMesh* this);

#endif
/* =============================== */
#ifdef GDMESH_SOURCE

GDMesh GDMesh_Create(const char* id, GDGeometry* geometry, GDMaterial* material) {
  GDMesh mesh = {
    .Object = GDObject_Create(id, GDOBJECT_TYPE_MESH),
    .Geometry = geometry,
    .Material = material
  };
  return mesh;
}

void GDMesh_Destroy(GDMesh* this) {
  GDObject_Destroy(&this->Object);
}

#undef GDMESH_SOURCE
#endif
