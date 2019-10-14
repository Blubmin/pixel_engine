#version 330

// Input
in vec2 v_uv;

// Constants
const int kKernelSize = 16;
const float radius = 1;
const float bias = .025;

// Uniforms
uniform sampler2D u_position_texture;
uniform sampler2D u_normal_texture;

uniform mat4 u_view;
uniform mat4 u_perspective;

uniform vec3 u_samples[kKernelSize];
uniform vec2 u_noise_scale;
uniform sampler2D u_ssao_noise;

// Output
out float final_color;

void main() {
	// Pull values from textures
	vec3 pos = (u_view * vec4(texture2D(u_position_texture, v_uv).xyz, 1)).xyz;
	vec3 normal = (u_view * vec4(texture2D(u_normal_texture, v_uv).xyz, 0)).xyz;
	vec3 random_vec = texture2D(u_ssao_noise, v_uv * u_noise_scale).xyz;

	// Construct ortho basis matrix
	vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);

	// Iterate over kernel samples
	float occlusion = 0.0;
	for (int i = 0; i < kKernelSize; ++i) {
		// Get sample position
		vec3 smpl = tbn * u_samples[i];
		smpl = pos + smpl * radius;

		// Project sample depth
		vec4 offset = vec4(smpl, 1.0);
		offset = u_perspective * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * .5 + .5;

		// Get sample depth
		float sample_depth = (u_view * vec4(texture2D(u_position_texture, offset.xy).xyz, 1)).z;

		// Accumulate values
		float range_check = smoothstep(0.0, 1.0, radius / abs(pos.z - sample_depth));
		occlusion += (sample_depth >= smpl.z + bias ? 1.0 : 0.0) * range_check;
	}
	occlusion = 1.f - (occlusion / kKernelSize);

	final_color = occlusion;
}