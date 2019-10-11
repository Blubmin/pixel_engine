#version 330

// Input
in vec3 v_normal;
in vec3 v_pos;
in vec2 v_uv;

// Uniforms
struct PointLight {
    vec3 pos;
    vec3 color;
    float linear_attenuation;
    float quadratic_attenuation;
};
uniform vec3 u_camera_pos;
uniform int u_num_point_lights;
uniform PointLight[8] u_point_lights;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D diffuse_texture;
};
uniform Material u_material;

// Output
out vec3 f_color;

// Globals
vec3 normal_vec;
vec3 view_vec;

vec3 compute_base_diffuse() {
	return u_material.diffuse + texture2D(u_material.diffuse_texture, v_uv).xyz;
}

vec3 compute_point_light(PointLight light) {
    vec3 light_vec = normalize(light.pos - v_pos);
    vec3 half_vec = normalize(light_vec + view_vec);
    vec3 diffuse = max(dot(light_vec, normal_vec), 0) * compute_base_diffuse();
	vec3 specular = pow(max(dot(half_vec, normal_vec), 0), u_material.shininess) * u_material.specular;

    float dist = length(light.pos - v_pos);
    float attenuation = 1 / (1 + light.linear_attenuation * dist + light.quadratic_attenuation * dist * dist);

    return (u_material.ambient * .2 + (diffuse + specular) * .8) * attenuation * light.color;
}

vec3 compute_directional_light() {
    vec3 light_vec = -normalize(vec3(1, -1, 1));
	vec3 half_vec = normalize(light_vec + view_vec);
    vec3 diffuse = max(dot(light_vec, normal_vec), 0) * compute_base_diffuse();
	vec3 specular = pow(max(dot(half_vec, normal_vec), 0), u_material.shininess) * u_material.specular;

    return (u_material.ambient * .2 + (diffuse + specular) * .8) * vec3(.003, .003, .003);
}

void main() {
    normal_vec = normalize(v_normal);
    view_vec = normalize(u_camera_pos - v_pos);
    f_color = vec3(.0, .0, .0);
    for (int i = 0; i < u_num_point_lights; ++i) {
        f_color += compute_point_light(u_point_lights[i]);
    }
    f_color += compute_directional_light();
}