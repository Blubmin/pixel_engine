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
uniform sampler2D u_shadow_texture;
uniform int u_num_point_lights;
uniform PointLight u_point_lights[8];
uniform DirectionalLight u_dir_light;
uniform mat4 u_shadow;
uniform mat4 u_map;

// Constants
const float c_gaussian[5] = {0.06136, .24477, .38774, .24477, .06136};
const float c_gaussian_3[3] = {0.27901, .44198, .27901};

const float c_gaussian_7[7] = {0.00598, 0.060626,	0.241843, 0.383103, 0.241843, 0.060626, 0.00598};

vec2 c_poisson_disk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

// Globals
vec3 pos;
vec3 normal;
vec3 albedo;
float shininess;
vec3 view_vec;

// Output
out vec4 final_color;


float random(vec3 seed, int i) {
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

float inv_shadow() {
	vec4 pos_in_light = u_shadow * vec4(pos, 1);
	vec4 pos_in_map = u_map * pos_in_light;
	vec3 shadow_uv = ((pos_in_map.xyz / pos_in_map.w) + vec3(1)) / 2;
	if (shadow_uv.z > 1) {
		return 1;
	}
	float shadow_depth = texture2D(u_shadow_texture, shadow_uv.xy).r;
	float bias = max(.05 * (1.0 - dot(normal, -u_dir_light.direction)), 0.04);
	bias = 0;
	float shadow = 0.0;
	vec2 texel_size = 1.0 / textureSize(u_shadow_texture, 0);

//	for(int x = -2; x <= 2; ++x)
//	{
//		for(int y = -2; y <= 2; ++y)
//		{
//			float depth = texture(u_shadow_texture, shadow_uv.xy + vec2(x, y) * texel_size * .5).r; 
//			shadow += (-pos_in_light.z - bias) > depth ? c_gaussian[x + 2] * c_gaussian[y + 2] : 0.0;        
//		}    
//	}

//	for(int x = -2; x <= 2; ++x) {
//		for(int y = -2; y <= 2; ++y) {
//			float depth = texture2D(u_shadow_texture, shadow_uv.xy + vec2(x, y) * texel_size).r; 
//			shadow += (-pos_in_light.z - (bias)) > depth ? 1.0 : 0.0;        
//		}    
//	}
//	shadow /= 25;

//	for(int x = -1; x <= 1; ++x) {
//		for(int y = -1; y <= 1; ++y) {
//			float depth = texture(u_shadow_texture, shadow_uv.xy + vec2(x, y) * texel_size).r; 
//			shadow += (-pos_in_light.z - bias) > depth ? c_gaussian_3[x + 1] * c_gaussian_3[y + 1] : 0.0;        
//		}    
//	}

	for(int x = -3; x <= 3; ++x)
	{
		for(int y = -3; y <= 3; ++y)
		{
			float depth = texture(u_shadow_texture, shadow_uv.xy + vec2(x, y) * texel_size).r; 
			shadow += (-pos_in_light.z - bias) > depth ? c_gaussian_7[x + 3] * c_gaussian_7[y + 3] : 0.0;        
		}    
	}

//	if (-pos_in_light.z - bias > texture(u_shadow_texture, shadow_uv.xy).r) {
//		shadow = 1;
//	}

	return 1 - shadow;
}

vec3 compute_point_light(PointLight light) {
	vec3 light_vec = normalize(light.pos - pos);
    vec3 half_vec = normalize(light_vec + view_vec);
    vec3 diffuse = max(dot(light_vec, normal), 0) * albedo;
	vec3 specular = pow(max(dot(half_vec, normal), 0), shininess) * vec3(.5);

    float dist = length(light.pos - pos);
    float attenuation = 1 / (1 + light.linear_attenuation * dist + light.quadratic_attenuation * dist * dist);

    return (diffuse + specular) * attenuation * light.color;
}

vec3 compute_directional_light() {
	vec3 light_vec = -normalize(u_dir_light.direction);
	vec3 half_vec = normalize(light_vec + view_vec);
    vec3 diffuse = max(dot(light_vec, normal), 0) * albedo;
	vec3 specular = pow(max(dot(half_vec, normal), 0), shininess) * vec3(.5);

    return (diffuse + specular) * u_dir_light.color * u_dir_light.strength * inv_shadow();
}

void main() {
	albedo = texture2D(u_albedo_spec_texture, v_uv).rgb;
	pos = texture2D(u_position_texture, v_uv).rgb;
	normal = texture2D(u_normal_texture, v_uv).rgb;
	shininess = texture2D(u_albedo_spec_texture, v_uv).a;
	float ssao = texture2D(u_ssao_texture, v_uv).r;

	vec3 ambient = albedo * ssao * .3;
	view_vec = normalize(u_camera_pos - pos);

	final_color.rgb = ambient;
	for (int i = 0; i < u_num_point_lights; ++i) {
		final_color.rgb += compute_point_light(u_point_lights[i]);
	}

	final_color.rgb += compute_directional_light();
	final_color.rgb = pow(final_color.rgb, vec3(1 / u_gamma));
	final_color.a = clamp(length(u_camera_pos - pos), 0, 1);

	// vec4 pos_in_light = u_shadow * vec4(pos, 1);
	// vec4 pos_in_map = u_map * pos_in_light;
	// vec2 shadow_uv = ((pos_in_map.xy / pos_in_map.w) + vec2(1)) / 2;
	// final_color = vec3(texture2D(u_shadow_texture, shadow_uv).r);
}