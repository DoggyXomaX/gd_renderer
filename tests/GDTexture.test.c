#include "test.h"

#define GDTEXTURE_TEST
#include "GDTexture.h"

#include <string.h>

void GDTexture_test() {
  testName("GDTexture");

  GDTexture texture = GDTexture_Load("Zabyl", "textures/test.png");

  test(strcmp(texture.Name, "Zabyl") == 0, "GDTexture.Name");
  test(texture.Width == 256 && texture.Height == 256, "GDTexture.Width/GDTexture.Height");
  test(texture.Flags & GDTEXTURE_INIT_FLAG, "GDTexture init flag");
}
