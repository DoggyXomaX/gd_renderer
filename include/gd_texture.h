#ifndef GDTEXTURE_HEADER
#define GDTEXTURE_HEADER

#include <GL/gl3w.h>
#include <stb_image.h>

typedef struct GDTexture_s {
  const char* Name;
  GLuint Texture;
  GLenum MinFilter;
  GLenum MagFilter;
  GLenum WrapS;
  GLenum WrapT;
} GDTexture;

GDTexture GDTexture_Create(const char* name);
GDTexture GDTexture_Load(const char* name, const char* path);

#endif
/* =============================== */
#ifdef GDTEXTURE_SOURCE

GDTexture GDTexture_Create(const char* name) {
  GDTexture texture = {
    .Name = name,
    .Texture = 0,
    .MinFilter = GL_LINEAR,
    .MagFilter = GL_LINEAR,
    .WrapS = GL_REPEAT,
    .WrapT = GL_REPEAT
  };

  glGenTextures(1, &texture.Texture);
  glBindTexture(GL_TEXTURE_2D, texture.Texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture.MinFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture.MagFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture.WrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture.WrapT);
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

GDTexture GDTexture_Load(const char* name, const char* path) {
  GDTexture texture = {
    .Name = name,
    .Texture = 0,
    .MinFilter = GL_LINEAR,
    .MagFilter = GL_LINEAR,
    .WrapS = GL_REPEAT,
    .WrapT = GL_REPEAT
  };

  glGenTextures(1, &texture.Texture);
  glBindTexture(GL_TEXTURE_2D, texture.Texture);

  glTexImage2D

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture.MinFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture.MagFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture.WrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture.WrapT);

  // TODO: stb_image loader
  // 1. Load the texture
  // 2. Send it to GPU using glTexture2D or something else
  // 3. Set the Texture field of GDTexture to this GPU ptr
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

#undef GDTEXTURE_SOURCE
#endif
