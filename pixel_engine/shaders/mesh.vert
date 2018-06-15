#version 450

layout(location = 0) in vec3 in_position;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

int main() {
	gl_Position = P * V * M * vec4(in_position, 1);
}