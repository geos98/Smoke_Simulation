#version 330
// uniform sampler2D u_smoke_texture;
in vec2 v_uv;
in vec4 vertexColor;
out vec4 FragColor;


void main() {
    // Sample the smoke texture using the interpolated UV coordinates
    // float alpha = texture(u_smoke_texture, v_uv).r;

    // // Multiply the texture alpha with the particle color alpha
    // alpha *= vertexColor.a;

    // // If alpha is below a threshold, discard the fragment
    // if (alpha < 0.01) {
    //     discard;
    // }

    // Set the final fragment color
    FragColor = vec4(vertexColor.rgb, vertexColor.a);
}


