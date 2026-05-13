#ifndef GDMATERIAL_HEADER
#define GDMATERIAL_HEADER

#include "gd_shader.h"

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define GDPARAM_TEXTURE 1
#define GDPARAM_INT32 2
#define GDPARAM_FLOAT 3
#define GDPARAM_VEC2 4
#define GDPARAM_VEC3 5
#define GDPARAM_VEC4 6
#define GDPARAM_MAT2 7
#define GDPARAM_MAT3 8
#define GDPARAM_MAT4 9

#define GDMATERIAL_MAX_PARAMS 32

typedef struct GDMaterialParam_s {
  const char* Name; // 8
  uint32_t Type;    // 4
  union {
    struct {
      int32_t Value; // 4
      char _[60];
    } Texture;
    struct {
      int32_t Value; // 4
      char _[60];
    } Int32;
    struct {
      float Value; // 4
      char _[60];
    } Float;
    struct {
      float Value[2]; // 8
      char _[56];
    } Vec2;
    struct {
      float Value[3]; // 12
      char _[52];
    } Vec3;
    struct {
      float Value[4]; // 16
      char _[48];
    } Vec4;
    struct {
      float Value[4]; // 16
      char _[48];
    } Mat2;
    struct {
      float Value[9]; // 9 * 4 -> 36
      char _[28];
    } Mat3;
    struct {
      float Value[16]; // 16 * 4 -> 64
    } Mat4;
    char Raw[64];
  };
} GDMaterialParam;

typedef struct GDMaterial_s {
  const char* Name;
  GDShader* Shader;

  GDMaterialParam Params[GDMATERIAL_MAX_PARAMS];
  size_t ParamsCount;
} GDMaterial;

GDMaterial GDMaterial_Create(const char* name, GDShader* shader);
void GDMaterial_Destroy(GDMaterial* this);
void GDMaterial_Use(GDMaterial* this);

// TODO: REAL TEXTURE FUCDK
void GDMaterial_RegisterTexture(GDMaterial* this, const char* name);
void GDMaterial_RegisterInt32(GDMaterial* this, const char* name);
void GDMaterial_RegisterFloat(GDMaterial* this, const char* name);
void GDMaterial_RegisterVec2(GDMaterial* this, const char* name);
void GDMaterial_RegisterVec3(GDMaterial* this, const char* name);
void GDMaterial_RegisterVec4(GDMaterial* this, const char* name);
void GDMaterial_RegisterMat2(GDMaterial* this, const char* name);
void GDMaterial_RegisterMat3(GDMaterial* this, const char* name);
void GDMaterial_RegisterMat4(GDMaterial* this, const char* name);
void GDMaterial_SetTexture(GDMaterial* this, const char* name, int32_t value);
void GDMaterial_SetInt32(GDMaterial* this, const char* name, int32_t value);
void GDMaterial_SetFloat(GDMaterial* this, const char* name, float value);
void GDMaterial_SetVec2(GDMaterial* this, const char* name, float value[2]);
void GDMaterial_SetVec3(GDMaterial* this, const char* name, float value[3]);
void GDMaterial_SetVec4(GDMaterial* this, const char* name, float value[4]);
void GDMaterial_SetMat2(GDMaterial* this, const char* name, float value[4]);
void GDMaterial_SetMat3(GDMaterial* this, const char* name, float value[9]);
void GDMaterial_SetMat4(GDMaterial* this, const char* name, float value[16]);

#endif
/* =============================== */
#ifdef GDMATERIAL_SOURCE

static inline void registerParam(GDMaterial* this, GDMaterialParam param) {
  if (this->ParamsCount >= GDMATERIAL_MAX_PARAMS) {
    fprintf(stderr, "[%s]: Exceeded maximum count of params %u\n", this->Name, GDMATERIAL_MAX_PARAMS);
    return;
  }

  this->Params[this->ParamsCount] = param;
  this->ParamsCount++;
}

static inline GDMaterialParam* findParam(GDMaterial* this, uint32_t type, const char* name) {
  for (size_t i = 0; i < this->ParamsCount; i++) {
    GDMaterialParam* param = &this->Params[i];
    if (strcmp(name, param->Name) == 0) {
      if (param->Type != type) {
        printf("Warn: \"%s\" inconsistent type: %u requested, param type %u\n", name, type, param->Type);
        break;
      }
      return param;
    }
  }

  printf("Warn: param \"%s\" is not registered!\n", name);
  return NULL;
}

GDMaterial GDMaterial_Create(const char* name, GDShader* shader) {
  GDMaterial material = {
    .Name = name,
    .Shader = shader,
    .Params = { 0 },
    .ParamsCount = 0,
  };

  return material;
}

void GDMaterial_Destroy(GDMaterial* this) {
  (void)this;
}

void GDMaterial_Use(GDMaterial* this) {
  GDShader* shader = this->Shader;

  GDShader_Use(shader);
  for (size_t i = 0; i < this->ParamsCount; i++) {
    GDMaterialParam* param = &this->Params[i];
    switch (param->Type) {
      case GDPARAM_TEXTURE:
        GDShader_SetTexture(shader, param->Name, param->Texture.Value);
        break;
      case GDPARAM_INT32:
        GDShader_SetInt32(shader, param->Name, param->Int32.Value);
        break;
      case GDPARAM_FLOAT:
        GDShader_SetFloat(shader, param->Name, param->Float.Value);
        break;
      case GDPARAM_VEC2:
        GDShader_SetVec2(shader, param->Name, param->Vec2.Value);
        break;
      case GDPARAM_VEC3:
        GDShader_SetVec3(shader, param->Name, param->Vec3.Value);
        break;
      case GDPARAM_VEC4:
        GDShader_SetVec4(shader, param->Name, param->Vec4.Value);
        break;
      case GDPARAM_MAT2:
        GDShader_SetMat2(shader, param->Name, param->Mat2.Value);
        break;
      case GDPARAM_MAT3:
        GDShader_SetMat3(shader, param->Name, param->Mat3.Value);
        break;
      case GDPARAM_MAT4:
        GDShader_SetMat4(shader, param->Name, param->Mat4.Value);
        break;
    }
  }
}

void GDMaterial_RegisterTexture(GDMaterial* this, const char* name) {
  GDMaterialParam param = { .Name = name, .Type = GDPARAM_TEXTURE };
  param.Texture.Value = 0;
  registerParam(this, param);
}

void GDMaterial_RegisterInt32(GDMaterial* this, const char* name) {
  GDMaterialParam param = { .Name = name, .Type = GDPARAM_INT32 };
  param.Int32.Value = 0;
  registerParam(this, param);
}

void GDMaterial_RegisterFloat(GDMaterial* this, const char* name) {
  GDMaterialParam param = { .Name = name, .Type = GDPARAM_FLOAT };
  param.Float.Value = 0.0f;
  registerParam(this, param);
}

void GDMaterial_RegisterVec2(GDMaterial* this, const char* name) {
  GDMaterialParam param = { .Name = name, .Type = GDPARAM_VEC2 };
  for (int i = 0; i < 2; i++) {
    param.Vec2.Value[i] = 0.0f;
  }
  registerParam(this, param);
}

void GDMaterial_RegisterVec3(GDMaterial* this, const char* name) {
  GDMaterialParam param = { .Name = name, .Type = GDPARAM_VEC3 };
  for (int i = 0; i < 3; i++) {
    param.Vec3.Value[i] = 0.0f;
  }
  registerParam(this, param);
}

void GDMaterial_RegisterVec4(GDMaterial* this, const char* name) {
  GDMaterialParam param = { .Name = name, .Type = GDPARAM_VEC4 };
  for (int i = 0; i < 4; i++) {
    param.Vec3.Value[i] = 0.0f;
  }
  registerParam(this, param);
}

void GDMaterial_RegisterMat2(GDMaterial* this, const char* name) {
  GDMaterialParam param = { .Name = name, .Type = GDPARAM_MAT2 };
  for (int i = 0; i < 4; i++) {
    param.Mat2.Value[i] = 0.0f;
  }
  registerParam(this, param);
}

void GDMaterial_RegisterMat3(GDMaterial* this, const char* name) {
  GDMaterialParam param = { .Name = name, .Type = GDPARAM_MAT3 };
  for (int i = 0; i < 9; i++) {
    param.Mat3.Value[i] = 0.0f;
  }
  registerParam(this, param);
}

void GDMaterial_RegisterMat4(GDMaterial* this, const char* name) {
  GDMaterialParam param = { .Name = name, .Type = GDPARAM_MAT4 };
  for (int i = 0; i < 16; i++) {
    param.Mat4.Value[i] = 0.0f;
  }
  registerParam(this, param);
}

void GDMaterial_SetTexture(GDMaterial* this, const char* name, int32_t value) {
  GDMaterialParam* param = findParam(this, GDPARAM_TEXTURE, name);
  if (param) {
    param->Texture.Value = value;
  }
}

void GDMaterial_SetInt32(GDMaterial* this, const char* name, int32_t value) {
  GDMaterialParam* param = findParam(this, GDPARAM_INT32, name);
  if (param) {
    param->Int32.Value = value;
  }
}

void GDMaterial_SetFloat(GDMaterial* this, const char* name, float value) {
  GDMaterialParam* param = findParam(this, GDPARAM_FLOAT, name);
  if (param) {
    param->Float.Value = value;
  }
}

void GDMaterial_SetVec2(GDMaterial* this, const char* name, float value[2]) {
  GDMaterialParam* param = findParam(this, GDPARAM_VEC2, name);
  if (param) {
    for (int i = 0; i < 2; i++) {
      param->Vec2.Value[i] = value[i];
    }
  }
}

void GDMaterial_SetVec3(GDMaterial* this, const char* name, float value[3]) {
  GDMaterialParam* param = findParam(this, GDPARAM_VEC3, name);
  if (param) {
    for (int i = 0; i < 3; i++) {
      param->Vec3.Value[i] = value[i];
    }
  }
}

void GDMaterial_SetVec4(GDMaterial* this, const char* name, float value[4]) {
  GDMaterialParam* param = findParam(this, GDPARAM_VEC4, name);
  if (param) {
    for (int i = 0; i < 4; i++) {
      param->Vec4.Value[i] = value[i];
    }
  }
}

void GDMaterial_SetMat2(GDMaterial* this, const char* name, float value[4]) {
  GDMaterialParam* param = findParam(this, GDPARAM_MAT2, name);
  if (param) {
    for (int i = 0; i < 4; i++) {
      param->Mat2.Value[i] = value[i];
    }
  }
}

void GDMaterial_SetMat3(GDMaterial* this, const char* name, float value[9]) {
  GDMaterialParam* param = findParam(this, GDPARAM_MAT3, name);
  if (param) {
    for (int i = 0; i < 9; i++) {
      param->Mat3.Value[i] = value[i];
    }
  }
}

void GDMaterial_SetMat4(GDMaterial* this, const char* name, float value[16]) {
  GDMaterialParam* param = findParam(this, GDPARAM_MAT4, name);
  if (param) {
    for (int i = 0; i < 16; i++) {
      param->Mat4.Value[i] = value[i];
    }
  }
}

#undef GDMATERIAL_SOURCE
#endif
