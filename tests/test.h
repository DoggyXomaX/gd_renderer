#ifndef TEST_HEADER
#define TEST_HEADER

void testName(const char* name);
void test(int condition, const char* message);
int testStatus();

#endif
/* =============================== */
#ifdef TEST_SOURCE

#include <stdio.h>

#define SUCCESS_COLOR "\033[32m"
#define ERROR_COLOR "\033[31m"
#define RESET_COLOR "\033[0m"

static int _testStatus = 0;

void testName(const char* name) {
  printf("\nTesting \"%s\":\n", name);
}

void test(int condition, const char* message) {
  if (condition) {
    printf("[  " SUCCESS_COLOR "OK" RESET_COLOR "  ]: %s\n", message);
  } else {
    fprintf(stderr, "[ " ERROR_COLOR "FAIL" RESET_COLOR " ]: " ERROR_COLOR "%s" RESET_COLOR "\n", message);
    _testStatus = 1;
  }
}

int testStatus() {
  return _testStatus;
}

#undef TEST_SOURCE
#endif
