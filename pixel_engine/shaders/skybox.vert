#version 330

layout(location = 0) in vec3 a_pos;
layout(location = 3) in vec2 a_uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_perspective;

out vec2 v_uv;

void main() {
	v_uv = a_uv;
	gl_Position = u_perspective * u_view * u_model * vec4(a_pos, 1);
}