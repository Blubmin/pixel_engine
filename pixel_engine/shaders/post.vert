#version 330

layout(location = 0) in vec2 a_pos;

out vec2 v_uv;

void main() {
    gl_Position = vec4(a_pos, 0, 1);
    v_uv = (a_pos + vec2(1, 1)) / 2.0;
}