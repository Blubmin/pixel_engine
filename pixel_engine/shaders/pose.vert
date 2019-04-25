#version 330

// Attributes
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_color;

// Uniforms
uniform mat4 u_view;

// Output
out vec3 v_color;

void main() {
    gl_Position = u_view * vec4(a_pos * .8f, 1);
    gl_Position.z = -gl_Position.z;
    v_color = a_color;
}