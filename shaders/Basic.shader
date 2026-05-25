#pragma vertex
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec2 vUV;

void main() {
  vUV = aUV;
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPosition, 1.0);
}
// ===============================================
#pragma fragment
#version 330 core

in vec2 vUV;
out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec4 color;

void main() {
  vec4 textureColor = texture(diffuse, vUV);
  FragColor = textureColor * color;
}
