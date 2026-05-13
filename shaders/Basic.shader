#pragma vertex
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float time;

out vec3 vColor;

void main() {
  vColor = aNormal;
  gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
// ===============================================
#pragma fragment
#version 330 core
in vec3 vColor;
out vec4 FragColor;

uniform float time;

void main() {
  FragColor = vec4(vec3(0.5) + vColor * 0.5, 1.0);
}
