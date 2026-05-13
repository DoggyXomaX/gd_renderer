#ifndef GDRENDERER_HEADER
#define GDRENDERER_HEADER

#include "gd_shader.h"

#include <GL/gl3w.h>
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define GDR_INIT_FLAG (1 << 0)
#define GDR_RUNNING_FLAG (1 << 1)
#define GDR_API_INIT_FLAG (1 << 2)

typedef struct GDRendererOpenGLImpl_s {
  SDL_Window* Window;
  SDL_Event Event;
  SDL_GLContext Context;
} GDRendererOpenGLImpl;

typedef GDRendererOpenGLImpl GDRendererImpl;

typedef struct GDRenderer_s {
  uint32_t Flags;
  uint32_t Width;
  uint32_t Height;
  const char* Title;
  double DeltaTime;
  double Time;

  GDRendererImpl Impl;
} GDRenderer;

GDRenderer GDRenderer_New(const char* title, uint32_t width, uint32_t height);
void GDRenderer_Init(GDRenderer* this);
void GDRenderer_Destroy(GDRenderer* this);
void GDRenderer_StartUpdate(GDRenderer* this);

#endif
/* =============================== */
#ifdef GDRENDERER_SOURCE

#include <stdio.h>
#include "m4f.h"

static bool isGL3WInit = false;

// <TEST>
// clang-format off
static float testVertices[] = {
  // front, red
  -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

  // back, green
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

  // left, blue
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,

  // right, yellow
   0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

  // top, cyan
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

  // bottom, magenta
  -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
};

static uint32_t testIndices[] = {
   0,  1,  2,   0,  2,  3,  // front
   4,  5,  6,   4,  6,  7,  // back
   8,  9, 10,   8, 10, 11,  // left
  12, 13, 14,  12, 14, 15,  // right
  16, 17, 18,  16, 18, 19,  // top
  20, 21, 22,  20, 22, 23,  // bottom
};
// clang-format on

GLuint testVAO = 0;
GLuint testVBO = 0;
GLuint testEBO = 0;
float testAngle = 0.0f;
m4f view, model, project;
m4f rotationX, rotationY, viewModel, mvp;

void testGL() {
  m4f_identity(&view);
  m4f_identity(&model);
  m4f_identity(&project);

  glEnable(GL_DEPTH_TEST);

  glGenVertexArrays(1, &testVAO);
  glGenBuffers(1, &testVBO);
  glGenBuffers(1, &testEBO);

  glBindVertexArray(testVAO);

  glBindBuffer(GL_ARRAY_BUFFER, testVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(testVertices), testVertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(testIndices), testIndices, GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
// </TEST>

GDRenderer GDRenderer_New(const char* title, uint32_t width, uint32_t height) {
  GDRenderer renderer = {
    .Flags = 0,
    .Title = title,
    .Width = width,
    .Height = height,
    .DeltaTime = 1.0 / 60.0,

    .Impl = { .Window = NULL }
  };

  if (!isGL3WInit) {
    if (!gl3wInit()) {
      fprintf(stderr, "Failed to init gl3w\n");
      return renderer;
    }

    isGL3WInit = true;
  }

  return renderer;
}

void GDRenderer_Init(GDRenderer* this) {
  if (this->Flags & GDR_INIT_FLAG) {
    return;
  }

  // Just init SDL Video
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Failed to init SDL: %s\n", SDL_GetError());
    return;
  }
  this->Flags |= GDR_API_INIT_FLAG;

  // OpenGL Core ES 3.3
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // Create window btw
  SDL_Window* window = SDL_CreateWindow(this->Title, this->Width, this->Height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    fprintf(stderr, "Failed to create window: %s", SDL_GetError());
    GDRenderer_Destroy(this);
    return;
  }
  this->Impl.Window = window;

  SDL_GLContext context = SDL_GL_CreateContext(window);
  if (context == NULL) {
    fprintf(stderr, "Failed to init SDL OpenGL context: %s\n", SDL_GetError());
    GDRenderer_Destroy(this);
    return;
  }
  this->Impl.Context = context;

  if (!SDL_GL_SetSwapInterval(1)) {
    printf("Warning: Failed to enable VSync: %s\n", SDL_GetError());
  }

  this->Flags |= GDR_INIT_FLAG;
}

void GDRenderer_Destroy(GDRenderer* this) {
  if (this->Impl.Context != NULL) {
    SDL_GL_DestroyContext(this->Impl.Context);
    this->Impl.Context = NULL;
  }
  if (this->Impl.Window != NULL) {
    SDL_DestroyWindow(this->Impl.Window);
    this->Impl.Window = NULL;
  }
  if (this->Flags & GDR_API_INIT_FLAG) {
    SDL_Quit();
  }

  this->Flags = 0;
}

void GDRenderer_StartUpdate(GDRenderer* this) {
  if (!(this->Flags & GDR_INIT_FLAG)) {
    return;
  }

  // <TEST>
  testGL();
  GDShader shader = GDShader_Load("Basic", "shaders/Basic.shader");
  if (!(shader.Flags & GDSHADER_INIT_FLAG)) {
    return;
  }
  GLint mvpLocation = glGetUniformLocation(shader.Program, "uMvp");
  // </TEST>

  glViewport(0, 0, this->Width, this->Height);
  glClearColor(0.0, 1.0, 0.0, 1.0);

  this->Flags |= GDR_RUNNING_FLAG;

  Uint64 prevTime = SDL_GetPerformanceCounter();
  Uint64 frequency = SDL_GetPerformanceFrequency();
  while (this->Flags & GDR_RUNNING_FLAG) {
    // Timing
    Uint64 time = SDL_GetPerformanceCounter();
    this->DeltaTime = (double)(time - prevTime) / (double)frequency;
    this->Time += this->DeltaTime;
    prevTime = time;

    // Events
    SDL_Event* event = &this->Impl.Event;
    while (SDL_PollEvent(event)) {
      switch (event->type) {
      case SDL_EVENT_QUIT: {
        this->Flags &= ~(GDR_RUNNING_FLAG);
        break;
      }
      case SDL_EVENT_WINDOW_RESIZED: {
        this->Width = event->window.data1;
        this->Height = event->window.data2;
      }
      }
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int pixelWidth, pixelHeight;
    SDL_GetWindowSizeInPixels(this->Impl.Window, &pixelWidth, &pixelHeight);
    float aspect = (float)pixelWidth / (float)pixelHeight;
    glViewport(0, 0, pixelWidth, pixelHeight);

    // <TEST>
    m4f_move(&view, 0.0f, 0.0f, -3.0f);
    m4f_project(&project, 70.0f, aspect, 0.1f, 100.0f);
    m4f_rotx(&rotationX, testAngle * 0.7f);
    m4f_roty(&rotationY, testAngle);
    m4f_mul(&model, &rotationY, &rotationX);
    m4f_mul(&viewModel, &view, &model);
    m4f_mul(&mvp, &project, &viewModel);
    testAngle += this->DeltaTime;

    glUseProgram(shader.Program);
    glBindVertexArray(testVAO);
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp.raw);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    // </TEST>

    SDL_GL_SwapWindow(this->Impl.Window);
  }

  // <TEST>
  glDeleteBuffers(1, &testEBO);
  glDeleteBuffers(1, &testVBO);
  glDeleteVertexArrays(1, &testVAO);
  GDShader_Destroy(&shader);
  // </TEST>
}

#undef GDRENDERER_SOURCE
#endif
