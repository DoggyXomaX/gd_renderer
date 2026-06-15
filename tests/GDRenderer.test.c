#include "GDGeometry.h"
#include "GDShader.h"
#include "GDTexture.h"
#include "test.h"

#include "GDRenderer.h"

static float yaw = 0.0f;

static GDRenderer renderer;
static GDGeometry cubeGeometry, catGeometry;
static GDShader basicShader, gridShader;
static GDTexture catTexture, forgetTexture, whiteTexture;
static GDMaterial catMaterial, basicMaterial, axesMaterial, gridMaterial;

static GDObject scene, cameraWrapper;
static GDCamera camera;
static GDMesh cat, grid, axisX, axisY, axisZ, mesh1, mesh2, mesh3, bullet;
static GDMesh guns[6];
static GDObjLoader loader;

static void OnUpdate(GDRenderer* this) {
  float aspect = (float)this->Width / (float)this->Height;

  GDObject_SetEuler(&cameraWrapper, 45.0f * DEG2RAD, yaw, 0.0f * DEG2RAD);
  GDObject_SetEuler(&mesh1.Object, yaw * 0.1f, 0.0f, 0.0f);
  GDObject_SetEuler(&mesh2.Object, 0.0f, yaw * 2.0f, 0.0f);

  GDObject_SetEuler(&cat.Object, cosf(this->Time * 10.0f) * 0.1f, 0.0f, 0.0f);

  GDObject_SetPosition(&bullet.Object, 0.0f, fmod(this->Time * 4.0f, 0.5f) * 20.0f, 0.0f);
  yaw += this->DeltaTime;

  if (camera.Aspect != aspect) {
    camera.Aspect = aspect;
    camera.Flags |= GDCAMERA_NEEDUPDATEMATRIX_FLAG;
  }
  GDRenderer_Render(this, &scene, &camera);
}

static void destroyAll() {
  GDGeometry_Destroy(&cubeGeometry);
  GDGeometry_Destroy(&catGeometry);
  GDShader_Destroy(&basicShader);
  GDShader_Destroy(&gridShader);
  GDMaterial_Destroy(&basicMaterial);
  GDMaterial_Destroy(&gridMaterial);
  GDMaterial_Destroy(&catMaterial);
  GDTexture_Destroy(&forgetTexture);
  GDTexture_Destroy(&whiteTexture);
  GDTexture_Destroy(&catTexture);
  GDObject_Destroy(&scene);
  GDObject_Destroy(&cameraWrapper);
  GDCamera_Destroy(&camera);
  GDMesh_Destroy(&axisX);
  GDMesh_Destroy(&axisY);
  GDMesh_Destroy(&axisZ);
  GDMesh_Destroy(&mesh1);
  GDMesh_Destroy(&mesh2);
  GDMesh_Destroy(&mesh3);
  GDMesh_Destroy(&bullet);
  for (int i = 0; i < 6; i++) {
    GDMesh_Destroy(&(guns[i]));
  }
  GDRenderer_Destroy(&renderer);
}

void GDRenderer_test() {
  testName("GDRenderer");

  renderer = GDRenderer_New("Test Scene", 800, 600);
  GDRenderer_Init(&renderer);
  bool isRendererInitOk = (renderer.Flags & GDR_INIT_FLAG) != 0;
  test(isRendererInitOk, "renderer init");
  if (!isRendererInitOk) {
    destroyAll();
    return;
  }

  loader = GDObjLoader_Create(NULL);
  GDObjLoader_LoadGeometry(&loader, "models/cube.obj", &cubeGeometry);
  GDObjLoader_LoadGeometry(&loader, "models/cat.obj", &catGeometry);
  bool isCubeOk = cubeGeometry.VAO != 0 && cubeGeometry.IndicesCount != 0;
  bool isCatOk = catGeometry.VAO != 0 && catGeometry.IndicesCount != 0;
  test(isCubeOk, "cube geometry");
  test(isCatOk, "cat geometry");
  if (!isCubeOk || !isCatOk) {
    destroyAll();
    return;
  }

  // Shaders
  basicShader = GDShader_Load("Basic", "shaders/Basic.shader");
  gridShader = GDShader_Load("Grid", "shaders/Grid.shader");
  bool isBasicOk = (basicShader.Flags & GDSHADER_INIT_FLAG) != 0;
  bool isGridOk = (gridShader.Flags & GDSHADER_INIT_FLAG) != 0;
  test(isBasicOk, "basic shader");
  test(isGridOk, "grid shader");
  if (!isBasicOk || !isGridOk) {
    destroyAll();
    return;
  }

  // Textures
  catTexture = GDTexture_Load("Cat", "textures/cat.png");
  forgetTexture = GDTexture_Load("Forget", "textures/forget.png");
  isCatOk = (catTexture.Flags & GDTEXTURE_INIT_FLAG) != 0;
  bool isForgetOk = (forgetTexture.Flags & GDTEXTURE_INIT_FLAG) != 0;
  test(isCatOk, "cat texture");
  test(isForgetOk, "forget texture");
  if (!isCatOk || !isForgetOk) {
    destroyAll();
    return;
  }

  uint8_t whitePixel[3] = { 255, 255, 255 };
  whiteTexture = GDTexture_Create("White", whitePixel, 1, 1, 3);
  bool isWhiteOk = (whiteTexture.Flags & GDTEXTURE_INIT_FLAG) != 0;
  test(isWhiteOk, "manual texture creation (white)");
  if (!isWhiteOk) {
    destroyAll();
    return;
  }

  // Materials
  catMaterial = GDMaterial_Create("Cat", &basicShader);
  GDMaterial_RegisterMat4(&catMaterial, "projectionMatrix");
  GDMaterial_RegisterMat4(&catMaterial, "viewMatrix");
  GDMaterial_RegisterMat4(&catMaterial, "modelMatrix");
  GDMaterial_RegisterVec4(&catMaterial, "color");
  GDMaterial_RegisterTexture(&catMaterial, "diffuse", 0);
  GDMaterial_SetTexture(&catMaterial, "diffuse", catTexture.Texture);

  basicMaterial = GDMaterial_Create("Basic", &basicShader);
  GDMaterial_RegisterMat4(&basicMaterial, "projectionMatrix");
  GDMaterial_RegisterMat4(&basicMaterial, "viewMatrix");
  GDMaterial_RegisterMat4(&basicMaterial, "modelMatrix");
  GDMaterial_RegisterVec4(&basicMaterial, "color");
  GDMaterial_RegisterTexture(&basicMaterial, "diffuse", 0);
  GDMaterial_SetTexture(&basicMaterial, "diffuse", forgetTexture.Texture);

  axesMaterial = GDMaterial_Create("Axes", &basicShader);
  GDMaterial_RegisterMat4(&axesMaterial, "projectionMatrix");
  GDMaterial_RegisterMat4(&axesMaterial, "viewMatrix");
  GDMaterial_RegisterMat4(&axesMaterial, "modelMatrix");
  GDMaterial_RegisterVec4(&axesMaterial, "color");
  GDMaterial_RegisterTexture(&axesMaterial, "diffuse", 0);
  GDMaterial_SetTexture(&axesMaterial, "diffuse", whiteTexture.Texture);

  gridMaterial = GDMaterial_Create("Grid", &gridShader);
  GDMaterial_RegisterMat4(&gridMaterial, "projectionMatrix");
  GDMaterial_RegisterMat4(&gridMaterial, "viewMatrix");
  GDMaterial_RegisterMat4(&gridMaterial, "modelMatrix");

  // Scene
  scene = GDObject_Create("Scene1", GDOBJECT_TYPE_EMPTY);

  cat = GDMesh_Create("Cat", &catGeometry, &catMaterial);
  GDObject_SetScale(&cat.Object, 0.5f, 0.5f, 0.5f);
  GDObject_AddChild(&scene, &cat.Object);

  grid = GDMesh_Create("Grid", &cubeGeometry, &gridMaterial);
  GDObject_AddChild(&scene, &grid.Object);
  GDObject_SetPosition(&grid.Object, 0.0f, -0.01f, 0.0f);
  GDObject_SetScale(&grid.Object, 10.0f, 0.01f, 10.0f);

  cameraWrapper = GDObject_Create("CameraWrapper1", GDOBJECT_TYPE_EMPTY);
  GDObject_AddChild(&scene, &cameraWrapper);

  camera = GDCamera_Create("Camera1", 70.0f, 1.0f, 0.1f, 100.0f);
  GDObject_SetPosition(&camera.Object, 0.0f, 0.0f, -3.0f);
  GDObject_AddChild(&cameraWrapper, &camera.Object);

  const float axesThickness = 0.01f;

  axisX = GDMesh_Create("AxisX", &cubeGeometry, &axesMaterial);
  GDObject_AddChild(&scene, &axisX.Object);
  GDObject_SetPosition(&axisX.Object, 0.5f, 0.0f, 0.0f);
  GDObject_SetScale(&axisX.Object, 1.0f, axesThickness, axesThickness);

  axisY = GDMesh_Create("AxisY", &cubeGeometry, &axesMaterial);
  GDObject_AddChild(&scene, &axisY.Object);
  GDObject_SetPosition(&axisY.Object, 0.0f, 0.5f, 0.0f);
  GDObject_SetScale(&axisY.Object, axesThickness, 1.0f, axesThickness);

  axisZ = GDMesh_Create("AxisZ", &cubeGeometry, &axesMaterial);
  GDObject_AddChild(&scene, &axisZ.Object);
  GDObject_SetPosition(&axisZ.Object, 0.0f, 0.0f, 0.5f);
  GDObject_SetScale(&axisZ.Object, axesThickness, axesThickness, 1.0f);

  mesh1 = GDMesh_Create("Mesh1", &cubeGeometry, &basicMaterial);
  GDObject_AddChild(&scene, &mesh1.Object);
  GDObject_SetPosition(&mesh1.Object, 0.5f, 0.5f, 0.5f);
  GDObject_SetScale(&mesh1.Object, 0.5f, 0.5f, 0.5f);

  mesh2 = GDMesh_Create("Mesh2", &cubeGeometry, &basicMaterial);
  GDObject_SetPosition(&mesh2.Object, 0.0f, 0.75f, 0.0f);
  GDObject_SetScale(&mesh2.Object, 0.5f, 0.5f, 0.5f);
  GDObject_AddChild(&mesh1.Object, &mesh2.Object);

  mesh3 = GDMesh_Create("Mesh3", &cubeGeometry, &basicMaterial);
  GDObject_SetPosition(&mesh3.Object, 0.0f, 0.0f, 0.0f);
  GDObject_SetScale(&mesh3.Object, 1.0f, 0.99f, 1.0f);
  GDObject_SetEuler(&mesh3.Object, 0.0f, 45.0f * DEG2RAD, 0.0f);
  GDObject_AddChild(&mesh2.Object, &mesh3.Object);

  bullet = GDMesh_Create("Bullet", &cubeGeometry, &basicMaterial);
  GDObject_SetPosition(&bullet.Object, 0.0f, 2.0f, 0.0f);
  GDObject_SetScale(&bullet.Object, 0.05f, 0.1f, 0.05f);
  GDObject_AddChild(&mesh1.Object, &bullet.Object);

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

  GDRenderer_StartUpdate(&renderer, OnUpdate);
  test(1, "render");

  destroyAll();
}
