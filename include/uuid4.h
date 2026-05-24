#ifndef UUID4_HEADER
#define UUID4_HEADER

#include <stddef.h>
#include <string.h>

#define UUID4_TEMPLATE "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx"
#define UUID4_LENGTH (sizeof(UUID4_TEMPLATE) - 1)

void uuid4(char* out, size_t offset);

#endif
/* =============================== */
#ifdef UUID4_SOURCE

#include <time.h>
#include <stdlib.h>

#define HEX_DIGITS "0123456789abcdef"

void uuid4(char* out, size_t offset) {
  srand(time(NULL));

  for (size_t i = 0; i < UUID4_LENGTH; i++) {
    // clang-format off
    switch (UUID4_TEMPLATE[i]) {
      case 'x': out[offset + i] = HEX_DIGITS[rand() % 16]; break;
      case 'y': out[offset + i] = HEX_DIGITS[rand() % 4 + 8]; break;
      default : out[offset + i] = UUID4_TEMPLATE[i]; break;
    }
    // clang-format on
  }
}

#undef UUID4_SOURCE
#endif
