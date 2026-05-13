#ifndef GDMATERIAL_HEADER
#define GDMATERIAL_HEADER

typedef struct GDMaterial_s {
  const char* Name;

  GDShader* Shader;
} GDMaterial;

#endif
/* =============================== */
#ifdef GDMATERIAL_SOURCE

#undef GDMATERIAL_SOURCE
#endif
