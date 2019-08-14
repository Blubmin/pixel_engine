#version 330

layout(location = 0) in vec2 a_pos;

uniform vec4 roi;

out vec2 v_uv;

void main() {
	vec2 pos = a_pos;
	pos.x = clamp(pos.x, roi.x, roi.z);
	pos.y = clamp(pos.y, roi.y, roi.w);
//	if (a_pos.x < 0) {
//		pos.x = (roi.x * 2) - 1;
//	} else {
//		pos.x = (roi.z * 2) - 1;
//	}
//
//	if (a_pos.y < 0) {
//		pos.y = (roi.y * 2) - 1;
//	} else {
//		pos.y = (roi.w * 2) - 1;
//	}
	gl_Position = vec4(pos, 0, 1);
	v_uv = (a_pos + vec2(1, 1)) / 2;
}