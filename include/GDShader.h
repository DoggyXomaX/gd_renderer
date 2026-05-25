#ifndef GDSHADER_HEADER
#define GDSHADER_HEADER

#include <GL/gl3w.h>
#include <stdint.h>

#define GDSHADER_INIT_FLAG (1 << 0)

#define GDSHADER_MAX_LEN 65536
#define GDSHADER_MAX_LINE_LEN 1024
#define GDSHADER_MAX_LOG_LEN 1024

#define GDSHADER_VERTEX_PRAGMA "#pragma vertex"
#define GDSHADER_FRAGMENT_PRAGMA "#pragma fragment"

typedef struct GDShader_s {
  const char* Name; // 8
  const char* Path; // 8
  uint32_t Flags;   // 4
  GLuint Program;   // 4
} GDShader;

GDShader GDShader_Load(const char* name, const char* path);
void GDShader_Destroy(GDShader* this);
void GDShader_Use(GDShader* this);
void GDShader_SetTexture(GDShader* this, const char* name, int32_t value, int32_t slot);
void GDShader_SetInt32(GDShader* this, const char* name, int32_t value);
void GDShader_SetFloat(GDShader* this, const char* name, float value);
void GDShader_SetVec2(GDShader* this, const char* name, float value[2]);
void GDShader_SetVec3(GDShader* this, const char* name, float value[3]);
void GDShader_SetVec4(GDShader* this, const char* name, float value[4]);
void GDShader_SetMat2(GDShader* this, const char* name, float value[4]);
void GDShader_SetMat3(GDShader* this, const char* name, float value[9]);
void GDShader_SetMat4(GDShader* this, const char* name, float value[16]);

#endif
/* =============================== */
#ifdef GDSHADER_SOURCE

#include <stdio.h>
#include <string.h>

static GLuint createShader(GLenum type, const char* source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint isOK = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isOK);

  if (!isOK) {
    char log[GDSHADER_MAX_LOG_LEN] = { 0 };
    glGetShaderInfoLog(shader, sizeof(log), NULL, log);
    fprintf(stderr, "Shader compile error: %s\n", log);

    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

static GLuint createProgram(const char* vertexSource, const char* fragmentSource) {
  GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSource);

  if (vertexShader == 0 || fragmentShader == 0) {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return 0;
  }

  GLuint program = glCreateProgram();

  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  GLint isOK = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &isOK);

  if (!isOK) {
    char log[GDSHADER_MAX_LOG_LEN] = { 0 };

    glGetProgramInfoLog(program, sizeof(log), NULL, log);
    fprintf(stderr, "Program link error: %s\n", log);

    glDeleteProgram(program);
    return 0;
  }

  return program;
}

static inline GLuint getUniformLocation(GDShader* this, const char* name) {
  return glGetUniformLocation(this->Program, name);
}

GDShader GDShader_Load(const char* name, const char* path) {
  GDShader shader = {
    .Name = name,
    .Path = path,
    .Flags = 0,
    .Program = 0,
  };

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Failed to load open \"%s\" file\n", path);
    return shader;
  }

  size_t vertexPragmaLength = strlen(GDSHADER_VERTEX_PRAGMA);
  size_t fragmentPragmaLength = strlen(GDSHADER_FRAGMENT_PRAGMA);
  char vertexShaderBuffer[GDSHADER_MAX_LEN] = { 0 };
  char fragmentShaderBuffer[GDSHADER_MAX_LEN] = { 0 };
  char lineBuffer[GDSHADER_MAX_LINE_LEN] = { 0 };
  size_t vertexShaderLength = 0, fragmentShaderLength = 0;
  int shaderType = 0;

  while (fgets(lineBuffer, sizeof(lineBuffer), file) != NULL) {
    size_t rawLength = strlen(lineBuffer);
    size_t lineLength = strcspn(lineBuffer, "\r\n");

    if (
      lineLength == vertexPragmaLength &&
      strncmp(lineBuffer, GDSHADER_VERTEX_PRAGMA, lineLength) == 0) {
      continue;
    }

    if (
      lineLength == fragmentPragmaLength &&
      strncmp(lineBuffer, GDSHADER_FRAGMENT_PRAGMA, lineLength) == 0) {
      shaderType = 1;
      continue;
    }

    if (shaderType == 0) {
      if (vertexShaderLength + rawLength + 1 > sizeof(vertexShaderBuffer)) {
        fprintf(stderr, "Error: vertex shader buffer overflow!\n");
        fclose(file);
        return shader;
      }

      memcpy(vertexShaderBuffer + vertexShaderLength, lineBuffer, rawLength + 1);
      vertexShaderLength += rawLength;
    }
    if (shaderType == 1) {
      if (fragmentShaderLength + rawLength + 1 > sizeof(fragmentShaderBuffer)) {
        fprintf(stderr, "Error: fragment shader buffer overflow!\n");
        fclose(file);
        return shader;
      }

      memcpy(fragmentShaderBuffer + fragmentShaderLength, lineBuffer, rawLength + 1);
      fragmentShaderLength += rawLength;
    }
  }

  if (ferror(file)) {
    fprintf(stderr, "Error: failed to read shader module!\n");
    fclose(file);
    return shader;
  }

  fclose(file);

  GLuint program = createProgram(vertexShaderBuffer, fragmentShaderBuffer);
  if (!program) {
    return shader;
  }

  shader.Program = program;
  shader.Flags |= GDSHADER_INIT_FLAG;

  return shader;
}

void GDShader_Destroy(GDShader* this) {
  glDeleteProgram(this->Program);
}

void GDShader_Use(GDShader* this) {
  if (!(this->Flags & GDSHADER_INIT_FLAG) || !this->Program) {
    printf("Warn: can't use uninitialized shader! Skip\n");
    return;
  }

  glUseProgram(this->Program);
}

void GDShader_SetTexture(GDShader* this, const char* name, int32_t value, int32_t slot) {
  GLint location = getUniformLocation(this, name);
  if (location >= 0) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, value);
    glUniform1i(location, slot);
  }
}

void GDShader_SetInt32(GDShader* this, const char* name, int32_t value) {
  GLint location = getUniformLocation(this, name);
  if (location >= 0) {
    glUniform1i(location, value);
  }
}

void GDShader_SetFloat(GDShader* this, const char* name, float value) {
  GLint location = getUniformLocation(this, name);
  if (location >= 0) {
    glUniform1f(location, value);
  }
}

void GDShader_SetVec2(GDShader* this, const char* name, float value[2]) {
  GLint location = getUniformLocation(this, name);
  if (location >= 0) {
    glUniform2fv(location, 1, value);
  }
}

void GDShader_SetVec3(GDShader* this, const char* name, float value[3]) {
  GLint location = getUniformLocation(this, name);
  if (location >= 0) {
    glUniform3fv(location, 1, value);
  }
}

void GDShader_SetVec4(GDShader* this, const char* name, float value[4]) {
  GLint location = getUniformLocation(this, name);
  if (location >= 0) {
    glUniform4fv(location, 1, value);
  }
}

void GDShader_SetMat2(GDShader* this, const char* name, float value[4]) {
  GLint location = getUniformLocation(this, name);
  if (location >= 0) {
    glUniformMatrix2fv(location, 1, GL_FALSE, value);
  }
}

void GDShader_SetMat3(GDShader* this, const char* name, float value[9]) {
  GLint location = getUniformLocation(this, name);
  if (location >= 0) {
    glUniformMatrix3fv(location, 1, GL_FALSE, value);
  }
}

void GDShader_SetMat4(GDShader* this, const char* name, float value[16]) {
  GLint location = getUniformLocation(this, name);
  if (location >= 0) {
    glUniformMatrix4fv(location, 1, GL_FALSE, value);
  }
}

#undef GDSHADER_SOURCE
#endif
