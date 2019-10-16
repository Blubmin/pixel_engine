#version 330

// Vertex Attributes
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec2 a_uv;

// Uniforms
uniform mat4 u_perspective;
uniform mat4 u_view;
uniform mat4 u_model;

// Output
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_pos;
out vec2 v_uv;

void main() {
    gl_Position = u_perspective * u_view * u_model * vec4(a_pos, 1);
    v_normal = (u_model * vec4(a_normal, 0)).xyz;
	v_tangent = (u_model * vec4(a_tangent, 0)).xyz;
    v_pos = (u_model * vec4(a_pos, 1)).xyz;
	v_uv = a_uv;
}