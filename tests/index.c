#define TEST_SOURCE
#include "test.h"

#define M4F_SOURCE
#include "m4f.h"

#include "test_gd_renderer.c"

int main() {
  test_gd_renderer();

  return testStatus();
}
