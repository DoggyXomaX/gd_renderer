#ifndef GDRENDERER_HEADER
#define GDRENDERER_HEADER

#include "gd_geometry.h"
#include "gd_material.h"
#include "gd_shader.h"

#include <GL/gl3w.h>
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

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
static GDVertex testVertices[] = {
  // Front, normal +Z
  -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,

  // Back, normal -Z
   0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,

  // Left, normal -X
  -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

  // Right, normal +X
   0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,

  // Top, normal +Y
  -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,

  // Bottom, normal -Y
  -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
};

static uint32_t testIndices[] = {
   0,  1,  2,   2,  3,  0,
   4,  5,  6,   6,  7,  4,
   8,  9, 10,  10, 11,  8,
  12, 13, 14,  14, 15, 12,
  16, 17, 18,  18, 19, 16,
  20, 21, 22,  22, 23, 20,
};
// clang-format on

float testAngle = 0.0f;
m4f view, model, projection;
m4f rotationX, rotationY, viewModel, mvp;

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
    printf("Warn: Failed to enable VSync: %s\n", SDL_GetError());
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

  GDGeometry geometry = GDGeometry_Create(
    "Box",
    testVertices, sizeof(testVertices) / sizeof(GDVertex),
    testIndices, sizeof(testIndices) / sizeof(uint32_t));

  GDShader shader = GDShader_Load("Basic shader btw", "shaders/Basic.shader");
  if (!(shader.Flags & GDSHADER_INIT_FLAG)) {
    return;
  }

  GDMaterial basicMaterial = GDMaterial_Create("Basic material btw", &shader);
  GDMaterial_RegisterMat4(&basicMaterial, "projection");
  GDMaterial_RegisterMat4(&basicMaterial, "view");
  GDMaterial_RegisterMat4(&basicMaterial, "model");
  GDMaterial_RegisterFloat(&basicMaterial, "time");

  glEnable(GL_DEPTH_TEST);
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
    m4f_rotx(&rotationX, testAngle * 0.7f);
    m4f_roty(&rotationY, testAngle);
    m4f_project(&projection, 70.0f, aspect, 0.1f, 100.0f);
    m4f_move(&view, 0.0f, 0.0f, -3.0f);
    m4f_mul(&model, &rotationY, &rotationX);
    GDMaterial_SetMat4(&basicMaterial, "projection", projection.raw);
    GDMaterial_SetMat4(&basicMaterial, "view", view.raw);
    GDMaterial_SetMat4(&basicMaterial, "model", model.raw);
    GDMaterial_SetFloat(&basicMaterial, "time", sinf(this->Time * 10.0f));
    testAngle += this->DeltaTime;

    GDMaterial_Use(&basicMaterial);
    GDGeometry_Draw(&geometry);

    SDL_GL_SwapWindow(this->Impl.Window);
  }

  // <TEST>
  GDGeometry_Destroy(&geometry);
  GDShader_Destroy(&shader);
  GDMaterial_Destroy(&basicMaterial);
  // </TEST>
}

#undef GDRENDERER_SOURCE
#endif
