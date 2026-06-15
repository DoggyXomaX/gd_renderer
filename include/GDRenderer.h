#ifndef GDRENDERER_HEADER
#define GDRENDERER_HEADER

#include "GDGeometry.h"
#include "GDMaterial.h"
#include "GDMesh.h"
#include "GDObjLoader.h"
#include "GDShader.h"
#include "GDTexture.h"
#include "GDCamera.h"

#include <GL/gl3w.h>
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>

#define GDR_INIT_FLAG (1 << 0)
#define GDR_RUNNING_FLAG (1 << 1)
#define GDR_API_INIT_FLAG (1 << 2)

#define __USE_MISC
#include <math.h>
#define DEG2RAD (M_PI / 180.0)

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
void GDRenderer_StartUpdate(GDRenderer* this, void (*onUpdate)(GDRenderer* this));
void GDRenderer_Render(GDRenderer* this, GDObject* object, GDCamera* camera);

#endif
/* =============================== */
#ifdef GDRENDERER_SOURCE

#include <stdio.h>
#include "m4f.h"

static bool isGL3WInit = false;

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

void GDRenderer_StartUpdate(GDRenderer* this, void (*onUpdate)(GDRenderer* this)) {
  if (!(this->Flags & GDR_INIT_FLAG)) {
    return;
  }

  glEnable(GL_DEPTH_TEST);

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
          break;
        }
        case SDL_EVENT_KEY_DOWN: {
          // TODO: event handling
        }
      }
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int pixelWidth, pixelHeight;
    SDL_GetWindowSizeInPixels(this->Impl.Window, &pixelWidth, &pixelHeight);
    glViewport(0, 0, pixelWidth, pixelHeight);

    if (onUpdate != NULL) {
      onUpdate(this);
    }

    SDL_GL_SwapWindow(this->Impl.Window);
  }
}

void GDRenderer_Render(GDRenderer* this, GDObject* object, GDCamera* camera) {
  if (camera->Flags & GDCAMERA_NEEDUPDATEMATRIX_FLAG) {
    GDCamera_UpdateMatrix(camera);
  }

  if (object->Flags & GDOBJECT_NEEDUPDATEMATRIX_FLAG) {
    GDObject_UpdateMatrix(object);
  }

  if (object->Type & GDOBJECT_TYPE_MESH) {
    GDMesh* mesh = (GDMesh*)object;
    if (GDMaterial_Has(mesh->Material, "projectionMatrix")) {
      GDMaterial_SetMat4(mesh->Material, "projectionMatrix", camera->ProjectionMatrix.raw);
    }
    if (GDMaterial_Has(mesh->Material, "viewMatrix")) {
      GDMaterial_SetMat4(mesh->Material, "viewMatrix", camera->Object.WorldMatrix.raw);
    }
    if (GDMaterial_Has(mesh->Material, "modelMatrix")) {
      GDMaterial_SetMat4(mesh->Material, "modelMatrix", mesh->Object.WorldMatrix.raw);
    }
    if (GDMaterial_Has(mesh->Material, "time")) {
      GDMaterial_SetFloat(mesh->Material, "time", this->Time);
    }
    if (GDMaterial_Has(mesh->Material, "color")) {
      v4f color;
      if (strcmp(object->ID, "AxisX") == 0) {
        v4f_set(&color, 1.0f, 0.0f, 0.0f, 1.0f);
      } else if (strcmp(object->ID, "AxisY") == 0) {
        v4f_set(&color, 0.0f, 1.0f, 0.0f, 1.0f);
      } else if (strcmp(object->ID, "AxisZ") == 0) {
        v4f_set(&color, 0.0f, 0.0f, 1.0f, 1.0f);
      } else {
        v4f_set(&color, 0.6f, 0.6f, 0.6f, 0.0f);
      }
      GDMaterial_SetVec4(mesh->Material, "color", color.raw);
    }
    GDMaterial_Use(mesh->Material);
    GDGeometry_Draw(mesh->Geometry);
  }

  for (size_t i = 0; i < object->ChildrenCount; i++) {
    GDRenderer_Render(this, object->Children[i], camera);
  }
}

#undef GDRENDERER_SOURCE
#endif
