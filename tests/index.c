#define TEST_SOURCE
#include "test.h"

#define M4F_SOURCE
#include "m4f.h"

#define V3F_SOURCE
#include "v3f.h"

#define V4F_SOURCE
#include "v4f.h"

#include "test_gd_renderer.c"

int main() {
  test_gd_renderer();

  return testStatus();
}
