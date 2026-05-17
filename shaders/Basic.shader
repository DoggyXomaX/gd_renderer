#pragma vertex
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

out vec3 vColor;

void main() {
  vColor = vec3(aUV, 1.0);
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPosition, 1.0);
}
// ===============================================
#pragma fragment
#version 330 core
in vec3 vColor;
out vec4 FragColor;

uniform vec4 color;

void main() {
  vec4 outColor = vec4(color.rgb, 1.0);
  FragColor = mix(vec4(vColor, 1.0), outColor, color.a);
}
