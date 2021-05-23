#include "common.h"

void BlinnPhong::initialize() noexcept {
	
}

vec4 BlinnPhong::vertex(const Vertex &vertex, int idx) noexcept {
	our_world_pos[idx] = model * vertex.position;
	our_normal[idx] = vertex.normal;
	return mvp * vertex.position;
}

bool BlinnPhong::fragment(const vec3 &point, const std::array<Vertex *, 3> &vertices, vec3 &color) noexcept {
	vec3 world_pos = interp(our_world_pos);
	vec3 world_normal = normalized(interp(our_normal));
	const vec3 &light_dir = get_uniform<vec3>("light_dir");

	const Texture2d diffuse_texture = get_uniform<Texture2d>("diffuse_texture");
	vec2 texcoords = interp(vertices, &Vertex::texcoords);
	vec3 diffuse_texture_color = diffuse_texture.texture(texcoords);

	// ambient color
	const vec3 &light_ambient = get_uniform<vec3>("light_ambient");
	vec3 ambient = light_ambient * diffuse_texture_color;

	 // diffuse color
	const vec3 &light_diffuse = get_uniform<vec3>("light_diffuse");
	float diff = std::max(dot(world_normal, light_dir), 0.f);
	vec3 diffuse = diff * diffuse_texture_color * light_diffuse;

	// specular color
	const vec3 &light_specular = get_uniform<vec3>("light_specular");
	const vec3 &eye_pos = get_uniform<vec3>("eye_pos");
	vec3 eye_dir = normalized(eye_pos - world_pos);
	vec3 half_vec = normalized(eye_dir + light_dir);
	float factor = get_uniform<float>("specular_factor");
	float spec = std::pow(std::max(dot(world_normal, half_vec), 0.f), factor);
	vec3 specular = spec * diffuse_texture_color * light_specular;

	color = ambient + diffuse + specular;
	return true;
}
