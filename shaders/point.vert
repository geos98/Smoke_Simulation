#version 330

layout (location = 0) in vec4 in_position;
layout (location = 1) in float in_size;
layout (location = 2) in vec4 in_color;

out vec4 vertexColor;

void main() {
  gl_Position = in_position;
  gl_PointSize = in_size;
  vertexColor = in_color;
}