#pragma vertex
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec4 vWorldPosition;

void main() {
  vWorldPosition = modelMatrix * vec4(aPosition, 1.0);
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPosition, 1.0);
}
// ===============================================
#pragma fragment
#version 330 core

in vec4 vWorldPosition;

out vec4 FragColor;

void main() {
  float x = fract(vWorldPosition.x);
  float z = fract(vWorldPosition.z);
  float t = 0.0;
  if (x < 0.01 || x > 0.99 || z < 0.01 || z > 0.99) t = 1.0;
  FragColor = vec4(t);
}
