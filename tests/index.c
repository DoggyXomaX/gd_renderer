#include "../deps/gl3w/src/gl3w.c"

#include "uuid4.test.c"
#include "GDTexture.test.c"
#include "GDRenderer.test.c"

#define TEST_SOURCE
#include "test.h"

#define M4F_SOURCE
#include "m4f.h"

#define V3F_SOURCE
#include "v3f.h"

#define V4F_SOURCE
#include "v4f.h"

#define GDRENDERER_SOURCE
#include "GDRenderer.h"

#define GDMATERIAL_SOURCE
#include "GDMaterial.h"

#define GDOBJECT_SOURCE
#include "GDObject.h"

#define GDMESH_SOURCE
#include "GDMesh.h"

#define GDCAMERA_SOURCE
#include "GDCamera.h"

#define GDSHADER_SOURCE
#include "GDShader.h"

#define GDGEOMETRY_SOURCE
#include "GDGeometry.h"

#define GDTEXTURE_SOURCE
#include "GDTexture.h"

#define GLHELPER_SOURCE
#include "GLHelper.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define UUID4_SOURCE
#include "uuid4.h"

int main() {
  uuid4_test();
  // GDTexture_test();
  GDRenderer_test();

  return testStatus();
}
