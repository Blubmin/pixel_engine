#version 330

// Attributes
layout(location = 0) in vec2 a_pos;

// Uniform
uniform mat4 u_perspective;
uniform mat4 u_view;

void main() {
    gl_Position = u_perspective * u_view * vec4(a_pos.x, 0, a_pos.y, 1);
}