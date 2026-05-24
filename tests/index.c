#include "../deps/gl3w/src/gl3w.c"

#include "test_gd_renderer.c"
#include "test_uuid4.c"

#define TEST_SOURCE
#include "test.h"

#define M4F_SOURCE
#include "m4f.h"

#define V3F_SOURCE
#include "v3f.h"

#define V4F_SOURCE
#include "v4f.h"

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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define UUID4_SOURCE
#include "uuid4.h"

int main() {
  test_uuid4();
  // test_gd_renderer();

  return testStatus();
}
