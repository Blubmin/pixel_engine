#version 330

in vec2 v_uv;

uniform sampler2D u_texture;
uniform float u_gamma;

out vec3 f_color;

void main() {
    float inv_gamma = 1 / u_gamma;
    f_color = pow(texture(u_texture, v_uv).rgb, vec3(inv_gamma, inv_gamma, inv_gamma));
}