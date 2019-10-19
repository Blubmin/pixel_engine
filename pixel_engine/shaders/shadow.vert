#version 330

// Input
layout (location = 0) in vec3 a_pos;

// Uniforms
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_perspective;

// Output
out float v_depth;

void main() {
	vec4 view_pos = u_view * u_model * vec4(a_pos, 1);
	v_depth = -view_pos.z;
	gl_Position = u_perspective * view_pos;
}