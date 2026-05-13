#include "test.h"

#define GDRENDERER_SOURCE
#include "gd_renderer.h"

#define GDMATERIAL_SOURCE
#include "gd_material.h"

#define GDSHADER_SOURCE
#include "gd_shader.h"

void test_gd_renderer() {
  testName("gd_renderer");

  GDRenderer renderer = GDRenderer_New("Test Window", 800, 600);

  GDRenderer_Init(&renderer);
  GDRenderer_StartUpdate(&renderer);

  test(1, "TODO: renderer test");
}
