#ifndef UUID4_HEADER
#define UUID4_HEADER

#include <stddef.h>
#include <string.h>

void uuid4(char* out, size_t offset);

#endif
/* =============================== */
#ifdef UUID4_SOURCE

#include <time.h>
#include <stdlib.h>

static const char template[] = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx";
static size_t templateLength = strlen(template);
static const char letters[] = "0123456789abcdef";
static size_t lettersCount = strlen(letters);

void uuid4(char* out, size_t offset) {
  srand(time(NULL));
  for (size_t i = 0; i < templateLength; i++) {
    out[offset + i] = template[i] != '-' ? letters[rand() % lettersCount] : '-';
  }
  out[offset + templateLength] = 0;
}

#undef UUID4_SOURCE
#endif
