#version 330

in vec2 v_uv;

uniform sampler2D texture_sampler;

out vec4 f_color;

void main() {
    f_color = texture(texture_sampler, v_uv);
}