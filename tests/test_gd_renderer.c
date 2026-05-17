#include "test.h"

#define GDRENDERER_SOURCE
#include "gd_renderer.h"

#define GDMATERIAL_SOURCE
#include "gd_material.h"

#define GDOBJECT_SOURCE
#include "gd_object.h"

#define GDMESH_SOURCE
#include "gd_mesh.h"

#define GDCAMERA_SOURCE
#include "gd_camera.h"

#define GDSHADER_SOURCE
#include "gd_shader.h"

#define GDGEOMETRY_SOURCE
#include "gd_geometry.h"

void OnUpdate(GDRenderer* renderer) {
  (void)renderer;
  // GDRenderer_Render(&renderer, &root, &camera)
}

void test_gd_renderer() {
  testName("gd_renderer");

  printf("GDRenderer_New\n");
  GDRenderer renderer = GDRenderer_New("Test Window", 800, 600);

  printf("GDRenderer_Init\n");
  GDRenderer_Init(&renderer);

  printf("GDRenderer_StartUpdate\n");
  GDRenderer_StartUpdate(&renderer, OnUpdate);

  test(1, "TODO: renderer test");
}
