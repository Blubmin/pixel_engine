#version 330

#include "types.glsl"

// Input
in float v_depth;

// Output
out float final_color;

void main() {
	final_color = v_depth;
}