#version 330

#include "types.glsl"

// Input
in vec2 v_uv;

// Uniforms
uniform float u_gamma;
uniform vec3 u_camera_pos;
uniform sampler2D u_albedo_spec_texture;
uniform sampler2D u_position_texture;
uniform sampler2D u_normal_texture;
uniform sampler2D u_ssao_texture;
uniform int u_num_point_lights;
uniform PointLight u_point_lights[8];
uniform DirectionalLight u_dir_light;

// Globals
vec3 pos;
vec3 normal;
vec3 albedo;
float shininess;
vec3 view_vec;

// Output
out vec3 final_color;

vec3 compute_point_light(PointLight light) {
	vec3 light_vec = normalize(light.pos - pos);
    vec3 half_vec = normalize(light_vec + view_vec);
    vec3 diffuse = max(dot(light_vec, normal), 0) * albedo;
	vec3 specular = pow(max(dot(half_vec, normal), 0), shininess) * vec3(1);

    float dist = length(light.pos - pos);
    float attenuation = 1 / (1 + light.linear_attenuation * dist + light.quadratic_attenuation * dist * dist);

    return (diffuse + specular) * attenuation * light.color;
}

vec3 compute_directional_light() {
	vec3 light_vec = -normalize(u_dir_light.direction);
	vec3 half_vec = normalize(light_vec + view_vec);
    vec3 diffuse = max(dot(light_vec, normal), 0) * albedo;
	vec3 specular = pow(max(dot(half_vec, normal), 0), shininess) * vec3(1);

    return (diffuse + specular) * u_dir_light.color * u_dir_light.strength;
}

void main() {
	albedo = texture2D(u_albedo_spec_texture, v_uv).rgb;
	pos = texture2D(u_position_texture, v_uv).rgb;
	normal = texture2D(u_normal_texture, v_uv).rgb;
	shininess = texture2D(u_albedo_spec_texture, v_uv).a;
	float ssao = texture2D(u_ssao_texture, v_uv).r;

	vec3 ambient = albedo * ssao * .3;
	view_vec = normalize(u_camera_pos - pos);

	final_color = ambient;
	for (int i = 0; i < u_num_point_lights; ++i) {
		final_color += compute_point_light(u_point_lights[i]);
	}

	final_color += compute_directional_light();
	final_color = pow(final_color, vec3(1 / u_gamma));
}