#ifndef GDTEXTURE_HEADER
#define GDTEXTURE_HEADER

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

GDTexture GDTexture_Create(const char* name, const uint8_t* pixels, int32_t width, int32_t height, int32_t channels);
void GDTexture_Destroy(GDTexture* this);
GDTexture GDTexture_Load(const char* name, const char* path);
void GDTexture_ToString(GDTexture* this, char* output, size_t maxLength);

#endif
/* =============================== */
#ifdef GDTEXTURE_SOURCE

static bool isStbInit = false;

GDTexture GDTexture_Create(const char* name, const uint8_t* pixels, int32_t width, int32_t height, int32_t channels) {
  GDTexture texture = {
    .Flags = 0,
    .Name = name,
    .Texture = 0,
    .Width = width,
    .Height = height,
    .Channels = channels,
    .MinFilter = GL_LINEAR_MIPMAP_LINEAR,
    .MagFilter = GL_LINEAR,
    .WrapS = GL_REPEAT,
    .WrapT = GL_REPEAT
  };

  glGenTextures(1, &texture.Texture);
  glBindTexture(GL_TEXTURE_2D, texture.Texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture.Width, texture.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture.MinFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture.MagFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture.WrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture.WrapT);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  texture.Flags |= GDTEXTURE_INIT_FLAG;

  return texture;
}

void GDTexture_Destroy(GDTexture* this) {
  if (this->Texture) {
    glDeleteTextures(1, &this->Texture);
  }
}

GDTexture GDTexture_Load(const char* name, const char* path) {
  GDTexture texture = { .Flags = 0 };

  if (!isStbInit) {
    stbi_set_flip_vertically_on_load(true);
    isStbInit = true;
  }

  int32_t width = 0;
  int32_t height = 0;
  int32_t channels = 0;
  unsigned char* pixels = stbi_load(path, &width, &height, &channels, 4);
  if (!pixels) {
    fprintf(stderr, "Failed to load \"%s\"\n", path);
    return texture;
  }

  texture = GDTexture_Create(name, pixels, width, height, channels);

  stbi_image_free(pixels);

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
