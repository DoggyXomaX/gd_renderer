#ifndef GDTEXTURE_HEADER
#define GDTEXTURE_HEADER

#include "GL/glcorearb.h"
#include "GLHelper.h"
#include <GL/gl3w.h>
#include <stb_image.h>
#include <stdbool.h>

#define GDTEXTURE_INIT_FLAG (1 << 0)

typedef struct GDTexture_s {
  uint32_t Flags;
  const char* Name;
  int32_t Width;
  int32_t Height;
  int32_t Channels;
  GLuint Texture;
  GLenum MinFilter;
  GLenum MagFilter;
  GLenum WrapS;
  GLenum WrapT;
} GDTexture;

GDTexture GDTexture_Create(const char* name);
GDTexture GDTexture_Load(const char* name, const char* path);
void GDTexture_ToString(GDTexture* this, char* output, size_t maxLength);

#endif
/* =============================== */
#ifdef GDTEXTURE_SOURCE

static bool isStbInit = false;

// Please don't use this method
GDTexture GDTexture_Create(const char* name) {
  GDTexture texture = {
    .Flags = 0,
    .Name = name,
    .Texture = 0,
    .Width = 0,
    .Height = 0,
    .Channels = 0,
    .MinFilter = GL_LINEAR,
    .MagFilter = GL_LINEAR,
    .WrapS = GL_REPEAT,
    .WrapT = GL_REPEAT
  };

  return texture;
}

GDTexture GDTexture_Load(const char* name, const char* path) {
  GDTexture texture = {
    .Flags = 0,
    .Name = name,
    .Width = 0,
    .Height = 0,
    .Channels = 0,
    .Texture = 0,
    .MinFilter = GL_LINEAR_MIPMAP_LINEAR,
    .MagFilter = GL_LINEAR,
    .WrapS = GL_REPEAT,
    .WrapT = GL_REPEAT
  };

  if (!isStbInit) {
    stbi_set_flip_vertically_on_load(true);
    isStbInit = true;
  }

  unsigned char* pixels = stbi_load(path, &texture.Width, &texture.Height, &texture.Channels, 4);
  if (!pixels) {
    fprintf(stderr, "Failed to load \"%s\"\n", path);
    return texture;
  }

  glGenTextures(1, &texture.Texture);
  glBindTexture(GL_TEXTURE_2D, texture.Texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.Width, texture.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture.MinFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture.MagFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture.WrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture.WrapT);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(pixels);

  texture.Flags |= GDTEXTURE_INIT_FLAG;

  return texture;
}

const char* GDTexture_FlagsToString(GDTexture* this) {
  if (this->Flags & GDTEXTURE_INIT_FLAG)
    return "GDTEXTURE_INIT_FLAG";
  return "GDTEXTURE_NO_FLAGS";
}

void GDTexture_ToString(GDTexture* this, char* output, size_t maxLength) {
  snprintf(
    output,
    maxLength,
    "{\n"
    "\t\"Flags\": \"%s\",\n"
    "\t\"Name\": \"%s\",\n"
    "\t\"Width\": %d,\n"
    "\t\"Height\": %d,\n"
    "\t\"Channels\": %d,\n"
    "\t\"Texture\": %d,\n"
    "\t\"MinFilter\": \"%s\",\n"
    "\t\"MagFilter\": \"%s\",\n"
    "\t\"WrapS\": \"%s\",\n"
    "\t\"WrapT\": \"%s\"\n"
    "}",
    GDTexture_FlagsToString(this),
    this->Name,
    this->Width,
    this->Height,
    this->Channels,
    this->Texture,
    GLTextureFilter_ToString(this->MinFilter),
    GLTextureFilter_ToString(this->MagFilter),
    GLTextureWrap_ToString(this->WrapS),
    GLTextureWrap_ToString(this->WrapT));
}

#undef GDTEXTURE_SOURCE
#endif
