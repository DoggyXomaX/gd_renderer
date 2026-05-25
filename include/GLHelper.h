#ifndef GLHELPER_HEADER
#define GLHELPER_HEADER

#include "GL/glcorearb.h"
#include <GL/gl3w.h>

const char* GLTextureFilter_ToString(GLenum value);
const char* GLTextureWrap_ToString(GLenum value);

#endif
/* =============================== */
#ifdef GLHELPER_SOURCE

const char* GLTextureFilter_ToString(GLenum value) {
  // clang-format off
  switch (value) {
    case GL_NEAREST: return "GL_NEAREST";
    case GL_LINEAR: return "GL_LINEAR";
    case GL_NEAREST_MIPMAP_NEAREST: return "GL_NEAREST_MIPMAP_NEAREST";
    case GL_LINEAR_MIPMAP_NEAREST: return "GL_LINEAR_MIPMAP_NEAREST";
    case GL_NEAREST_MIPMAP_LINEAR: return "GL_NEAREST_MIPMAP_LINEAR";
    case GL_LINEAR_MIPMAP_LINEAR: return "GL_LINEAR_MIPMAP_LINEAR";
    default: return "GL_INVALID_ENUM";
  }
  // clang-format on
}

const char* GLTextureWrap_ToString(GLenum value) {
  // clang-format off
  switch (value) {
    case GL_CLAMP_TO_EDGE: return "GL_CLAMP_TO_EDGE";
    case GL_MIRRORED_REPEAT: return "GL_MIRRORED_REPEAT";
    case GL_REPEAT: return "GL_REPEAT";
    case GL_MIRROR_CLAMP_TO_EDGE: return "GL_MIRROR_CLAMP_TO_EDGE";
    default: return "GL_INVALID_ENUM";
  }
  // clang-format on
}

#undef GLHELPER_SOURCE
#endif
