#version 330

// Input
in vec3 v_normal;
in vec3 v_pos;

// Output
out vec3 f_color;

void main() {
    vec3 v_norm = normalize(v_normal);
    vec3 light = vec3(0, 1, 0);
    vec3 view = normalize(-v_pos);
    vec3 half_vec = normalize(light + view);
    float diffuse = max(dot(half_vec, v_norm), 0);

    f_color = vec3(.6, .6, .6) * diffuse;
}