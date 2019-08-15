#version 330

layout(location = 0) in vec2 a_pos;

uniform vec4 roi;

out vec2 v_uv;

void main() {
	vec2 pos = a_pos;
	pos.x = clamp(pos.x, roi.x, roi.z);
	pos.y = clamp(pos.y, roi.y, roi.w);
	gl_Position = vec4(pos, 0, 1);
	v_uv = (a_pos + vec2(1, 1)) / 2;
}