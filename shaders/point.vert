#version 330

uniform mat4 u_view_projection;
uniform mat4 u_model;

in vec4 in_position;
in float in_size;
in vec4 in_color;
in vec2 in_uv;

// out vec4 v_position;
// out vec4 v_normal;
// out vec4 v_tangent;

out vec2 v_uv;
out vec4 vertexColor;

void main() {
    // Compute the final position of the particle
  vec4 pos = u_view_projection * u_model * in_position;
  // Compute the final size of the particle
  float finalSize = in_size * (1.0 / pos.w);

  // Set the output color of the fragment shader
  gl_PointSize = finalSize;
  gl_Position = pos;
  vertexColor = in_color;
  v_uv = in_uv;
}