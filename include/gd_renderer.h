#ifndef GDRENDERER_HEADER
#define GDRENDERER_HEADER

#include "SDL3/SDL_events.h"
#include "gd_geometry.h"
#include "gd_material.h"
#include "gd_mesh.h"
#include "gd_shader.h"
#include "gd_camera.h"

#include <GL/gl3w.h>
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define GDR_INIT_FLAG (1 << 0)
#define GDR_RUNNING_FLAG (1 << 1)
#define GDR_API_INIT_FLAG (1 << 2)

#define M_PI 3.1415920
static const float DEG2RAD = (float)M_PI / 180.0f;

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

  // !!!!NEED REMOVE!!!!!
  GDGeometry cubeGeometry = GDGeometry_Create(
    "Box",
    testVertices, sizeof(testVertices) / sizeof(GDVertex),
    testIndices, sizeof(testIndices) / sizeof(uint32_t));

  GDShader basicShader = GDShader_Load("Basic", "shaders/Basic.shader");
  GDShader gridShader = GDShader_Load("Grid", "shaders/Grid.shader");
  if (!(basicShader.Flags & GDSHADER_INIT_FLAG) || !(gridShader.Flags & GDSHADER_INIT_FLAG)) {
    return;
  }

  GDMaterial basicMaterial = GDMaterial_Create("Basic", &basicShader);
  GDMaterial_RegisterMat4(&basicMaterial, "projectionMatrix");
  GDMaterial_RegisterMat4(&basicMaterial, "viewMatrix");
  GDMaterial_RegisterMat4(&basicMaterial, "modelMatrix");
  GDMaterial_RegisterVec4(&basicMaterial, "color");
  GDMaterial_RegisterFloat(&basicMaterial, "time");

  GDMaterial gridMaterial = GDMaterial_Create("Grid", &gridShader);
  GDMaterial_RegisterMat4(&gridMaterial, "projectionMatrix");
  GDMaterial_RegisterMat4(&gridMaterial, "viewMatrix");
  GDMaterial_RegisterMat4(&gridMaterial, "modelMatrix");

  GDObject scene = GDObject_Create("Scene1", GDOBJECT_TYPE_EMPTY);

  GDMesh grid = GDMesh_Create("Grid", &cubeGeometry, &gridMaterial);
  GDObject_AddChild(&scene, &grid.Object);
  GDObject_SetPosition(&grid.Object, 0.0f, -0.01f, 0.0f);
  GDObject_SetScale(&grid.Object, 10.0f, 0.01f, 10.0f);

  GDObject cameraWrapper = GDObject_Create("CameraWrapper1", GDOBJECT_TYPE_EMPTY);
  GDObject_AddChild(&scene, &cameraWrapper);

  GDCamera camera = GDCamera_Create("Camera1", 70.0f, 1.0f, 0.1f, 100.0f);
  GDObject_SetPosition(&camera.Object, 0.0f, 0.0f, -3.0f);
  GDObject_AddChild(&cameraWrapper, &camera.Object);

  float lineWidth = 0.01f;

  GDMesh axisX = GDMesh_Create("AxisX", &cubeGeometry, &basicMaterial);
  GDObject_AddChild(&scene, &axisX.Object);
  GDObject_SetPosition(&axisX.Object, 0.5f, 0.0f, 0.0f);
  GDObject_SetScale(&axisX.Object, 1.0f, lineWidth, lineWidth);

  GDMesh axisY = GDMesh_Create("AxisY", &cubeGeometry, &basicMaterial);
  GDObject_AddChild(&scene, &axisY.Object);
  GDObject_SetPosition(&axisY.Object, 0.0f, 0.5f, 0.0f);
  GDObject_SetScale(&axisY.Object, lineWidth, 1.0f, lineWidth);

  GDMesh axisZ = GDMesh_Create("AxisZ", &cubeGeometry, &basicMaterial);
  GDObject_AddChild(&scene, &axisZ.Object);
  GDObject_SetPosition(&axisZ.Object, 0.0f, 0.0f, 0.5f);
  GDObject_SetScale(&axisZ.Object, lineWidth, lineWidth, 1.0f);

  GDMesh mesh1 = GDMesh_Create("Mesh1", &cubeGeometry, &basicMaterial);
  GDObject_AddChild(&scene, &mesh1.Object);
  GDObject_SetPosition(&mesh1.Object, 0.5f, 0.5f, 0.5f);
  GDObject_SetScale(&mesh1.Object, 0.5f, 0.5f, 0.5f);

  GDMesh mesh2 = GDMesh_Create("Mesh2", &cubeGeometry, &basicMaterial);
  GDObject_SetPosition(&mesh2.Object, 0.0f, 0.75f, 0.0f);
  GDObject_SetScale(&mesh2.Object, 0.5f, 0.5f, 0.5f);
  GDObject_AddChild(&mesh1.Object, &mesh2.Object);

  GDMesh mesh3 = GDMesh_Create("Mesh3", &cubeGeometry, &basicMaterial);
  GDObject_SetPosition(&mesh3.Object, 0.0f, 0.0f, 0.0f);
  GDObject_SetScale(&mesh3.Object, 1.0f, 0.99f, 1.0f);
  GDObject_SetEuler(&mesh3.Object, 0.0f, 45.0f * DEG2RAD, 0.0f);
  GDObject_AddChild(&mesh2.Object, &mesh3.Object);

  GDMesh bullet = GDMesh_Create("Bullet", &cubeGeometry, &basicMaterial);
  GDObject_SetPosition(&bullet.Object, 0.0f, 2.0f, 0.0f);
  GDObject_SetScale(&bullet.Object, 0.05f, 0.1f, 0.05f);
  GDObject_AddChild(&mesh1.Object, &bullet.Object);

  GDMesh guns[6];
  float deltaAngle = (M_PI * 2.0f) / 6.0f;
  float x = 0.0f;
  float y = 0.0f;
  for (int i = 0; i < 6; i++) {
    x = cosf(deltaAngle * (float)i);
    y = sinf(deltaAngle * (float)i);
    guns[i] = GDMesh_Create("G0", &cubeGeometry, &basicMaterial);
    GDObject_SetPosition(&guns[i].Object, x * 0.3f, 0.75f, y * 0.3f);
    GDObject_SetScale(&guns[i].Object, 0.1f, 0.5f, 0.1f);
    GDObject_AddChild(&mesh2.Object, &guns[i].Object);
  }
  // /!!!!NEED REMOVE!!!!!

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
        }
      }
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int pixelWidth, pixelHeight;
    SDL_GetWindowSizeInPixels(this->Impl.Window, &pixelWidth, &pixelHeight);
    float aspect = (float)pixelWidth / (float)pixelHeight;
    glViewport(0, 0, pixelWidth, pixelHeight);

    if (onUpdate != NULL) {
      onUpdate(this);
    }

    // !!!!!!!!!!NEEED REMOVE !!!!!!
    {
      GDObject_SetEuler(&cameraWrapper, 45.0f * DEG2RAD, testAngle, 0.0f * DEG2RAD);
      GDObject_SetEuler(&mesh1.Object, testAngle * 0.1f, 0.0f, 0.0f);
      GDObject_SetEuler(&mesh2.Object, 0.0f, testAngle * 2.0f, 0.0f);
      GDObject_SetPosition(&bullet.Object, 0.0f, fmod(this->Time * 4.0f, 0.5f) * 20.0f, 0.0f);
      testAngle += this->DeltaTime;

      if (camera.Aspect != aspect) {
        camera.Aspect = aspect;
        camera.Flags |= GDCAMERA_NEEDUPDATEMATRIX_FLAG;
      }
      GDRenderer_Render(this, &scene, &camera);
    }
    SDL_GL_SwapWindow(this->Impl.Window);
  }

  // !!!!!!!!!!NEEED REMOVE !!!!!!
  {
    GDGeometry_Destroy(&cubeGeometry);
    GDShader_Destroy(&basicShader);
    GDShader_Destroy(&gridShader);
    GDMaterial_Destroy(&basicMaterial);
    GDMaterial_Destroy(&gridMaterial);
    GDObject_Destroy(&scene);
    GDCamera_Destroy(&camera);
    GDMesh_Destroy(&axisX);
    GDMesh_Destroy(&axisY);
    GDMesh_Destroy(&axisZ);
    GDMesh_Destroy(&mesh1);
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
