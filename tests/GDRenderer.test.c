#include "test.h"

#include "GDRenderer.h"

void OnUpdate(GDRenderer* renderer) {
  (void)renderer;
  // GDRenderer_Render(&renderer, &root, &camera)
}

void GDRenderer_test() {
  testName("GDRenderer");

  GDRenderer renderer = GDRenderer_New("Test Window", 800, 600);
  GDRenderer_Init(&renderer);
  GDRenderer_StartUpdate(&renderer, OnUpdate);

  test(1, "TODO: renderer test");
}
