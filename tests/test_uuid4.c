#include "test.h"
#include "uuid4.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#define TEST_UUID_TEMPLATE "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx"
#define TEST_UUID_LENGTH (sizeof(TEST_UUID_TEMPLATE) - 1)
#define ITER_COUNT 8
#define BUFFER_SIZE 512

bool isX(char c) {
  return isxdigit(c);
}

bool isY(char c) {
  return c == '8' || c == '9' || c == 'a' || c == 'b' || c == 'A' || c == 'B';
}

void test_uuid4() {
  testName("uuid4");

  bool isOk = true;
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);
  for (size_t i = 0; i < ITER_COUNT; i++) {
    size_t offset = i * (BUFFER_SIZE / ITER_COUNT);
    uuid4(buffer, offset);
    if (strlen(buffer + offset) != TEST_UUID_LENGTH) {
      isOk = false;
      break;
    }
  }
  test(isOk, "uuid4 length");

  isOk = true;
  for (size_t i = 0; i < ITER_COUNT; i++) {
    uuid4(buffer, 0);
    for (size_t j = 0; j < TEST_UUID_LENGTH; j++) {
      char templateChar = TEST_UUID_TEMPLATE[j];
      char bufferChar = buffer[j];
      if (
        (templateChar == 'x' && !isX(bufferChar)) ||
        (templateChar == 'y' && !isY(bufferChar)) ||
        (templateChar != 'x' && templateChar != 'y' && bufferChar != templateChar)) {
        isOk = false;
        goto endTemplateLoop;
      }
    }
  }
endTemplateLoop:
  test(isOk, "uuid4 template");
}
