#ifndef GDTEXTURE_HEADER
#define GDTEXTURE_HEADER

#include <GL/gl3w.h>

typedef struct GDTexture_s {
  const char* Name;
  GLuint Texture;
} GDTexture;

GDTexture GDTexture_Create(const char* name);
GDTexture GDTexture_Load(const char* name, const char* path);

#endif
/* =============================== */
#ifdef GDTEXTURE_SOURCE

GDTexture GDTexture_Create(const char* name) {
  GDTexture texture = { .Name = name, .Texture = 0 };
  return texture;
}

GDTexture GDTexture_Load(const char* name, const char* path) {
  GDTexture texture = { .Name = name, .Texture = 0 };
  // TODO: stb_image loader
  // 1. Load the texture
  // 2. Send it to GPU using glTexture2D or something else
  // 3. Set the Texture field of GDTexture to this GPU ptr
  return texture;
}

#undef GDTEXTURE_SOURCE
#endif
