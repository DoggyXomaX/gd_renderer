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

#endif
/* =============================== */
#ifdef GDSHADER_SOURCE

#include <stdio.h>

static GLuint createShader(GLenum type, const char* source) {
  printf("DEBUG: create shader\n");
  GLuint shader = glCreateShader(type);

  printf("DEBUG: connect source\n");
  glShaderSource(shader, 1, &source, NULL);

  printf("DEBUG: compile shader\n");
  glCompileShader(shader);

  GLint isOK = 0;
  printf("DEBUG: get compile status\n");
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isOK);

  if (!isOK) {
    char log[GDSHADER_MAX_LOG_LEN] = { 0 };

    printf("DEBUG: get compile status info log\n");
    glGetShaderInfoLog(shader, sizeof(log), NULL, log);
    fprintf(stderr, "Shader compile error: %s\n", log);

    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

static GLuint createProgram(const char* vertexSource, const char* fragmentSource) {
  printf("DEBUG: create vertex shader\n");
  GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexSource);
  printf("DEBUG: create fragment shader\n");
  GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSource);

  if (vertexShader == 0 || fragmentShader == 0) {
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return 0;
  }

  printf("DEBUG: create program\n");
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

  printf("Vertex shader:\n{{{%s}}}\n\nFragment shader:\n{{{%s}}}\n", vertexShaderBuffer, fragmentShaderBuffer);

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

#undef GDSHADER_SOURCE
#endif
