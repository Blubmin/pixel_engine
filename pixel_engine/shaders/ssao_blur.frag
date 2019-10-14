#version 330

// Input
in vec2 v_uv;

// Uniforms
uniform sampler2D u_ssao_texture;

// Output
out float final_color;

void main() {
	vec2 texel_size = 1.f / vec2(textureSize(u_ssao_texture, 0));
	float result = 0.f;
	for (int x = -2; x < 2; ++x) {
		for (int y = -2; y < 2; ++y) {
			vec2 offset = vec2(x, y) * texel_size;
			result += texture(u_ssao_texture, v_uv + offset).r;
		}
	}
	final_color = result / (4 * 4);
}