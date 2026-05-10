#ifndef GDRENDERER_HEADER
#define GDRENDERER_HEADER

#include "GL/glcorearb.h"
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_video.h"
#include <GL/gl3w.h>
#include <SDL3/SDL.h>
#include <stdbool.h>

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

#include <stdbool.h>
#include <stdio.h>

static bool isGL3WInit = false;

int gl3wInit();

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

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, this->Width, this->Height);
  glClearColor(0.0, 1.0, 0.0, 1.0);

  this->Flags |= GDR_RUNNING_FLAG;

  double prevSecondTime = 1;
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
        glViewport(0, 0, event->window.data1, event->window.data2);
      }
      }
    }

    if (prevSecondTime < this->Time) {
      prevSecondTime = this->Time + 1.0;
      printf("FPS: %8.2lf, %ux%u\n", 1.0 / this->DeltaTime, this->Width, this->Height);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: real rendering

    SDL_GL_SwapWindow(this->Impl.Window);
  }
}

#undef GDRENDERER_SOURCE
#endif
