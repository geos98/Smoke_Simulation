#version 330

in vec4 in_position;

void main() {
  gl_Position = in_position;
  gl_PointSize = 10;
}