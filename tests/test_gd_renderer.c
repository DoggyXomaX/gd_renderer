#include "test.h"

#include "gd_renderer.h"

void OnUpdate(GDRenderer* renderer) {
  (void)renderer;
  // GDRenderer_Render(&renderer, &root, &camera)
}

void test_gd_renderer() {
  testName("gd_renderer");

  GDRenderer renderer = GDRenderer_New("Test Window", 800, 600);
  GDRenderer_Init(&renderer);
  GDRenderer_StartUpdate(&renderer, OnUpdate);

  test(1, "TODO: renderer test");
}
