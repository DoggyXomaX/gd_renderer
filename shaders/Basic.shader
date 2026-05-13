#pragma vertex
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 vColor;

void main() {
  vColor = aColor;
  gl_Position = projection * view * model * vec4(aPosition, 1.0);
}

#pragma fragment
#version 330 core
in vec3 vColor;
out vec4 FragColor;

uniform float time;

void main() {
  vec2 pixelPosition = gl_FragCoord.xy;
  FragColor = vec4(pixelPosition.xy / 1000.0, vColor.b * time, 1.0);
}
