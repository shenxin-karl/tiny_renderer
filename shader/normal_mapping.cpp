#include "common.h"

void NormalMapping::initialize() noexcept {
	normal_texture_ptr = &get_uniform<Texture2d>("normal_texture");
	diffuse_texture_ptr = &get_uniform<Texture2d>("diffuse_texture");
	light_dir_ptr = &get_uniform<vec3>("light_dir");
	light_ambient_ptr = &get_uniform<vec3>("light_ambient");
	light_diffuse_ptr = &get_uniform<vec3>("light_diffuse");
	light_specular_ptr = &get_uniform<vec3>("light_specular");
	eye_pos_ptr = &get_uniform<vec3>("eye_pos");
	factor_ptr = &get_uniform<float>("specular_factor");
}

vec4 NormalMapping::vertex(const Vertex &vertex, int idx) noexcept {
	auto res = mvp * vertex.position;
	float inverse_z = 1.f / res.w();
	our_texcoords[idx] = vertex.texcoords * inverse_z;
	our_world_vertex[idx] = (model * vertex.position).head<3>() * inverse_z;
	return res;
}

bool NormalMapping::fragment(const vec3 &point, vec3 &color) noexcept {
	vec2 texcoords = interp(our_texcoords);
	vec3 normal = this->normal_texture_ptr->normal(texcoords);
	float gray = std::clamp(dot(*this->light_dir_ptr, normal), 0.f, 1.f);
	vec3 diffuse_texture_color = this->diffuse_texture_ptr->rgb(texcoords);

	// ambient 
	vec3 ambient = *this->light_ambient_ptr * diffuse_texture_color;

	// diffuse
	float diff = std::max(dot(normal, *this->light_dir_ptr), 0.f);
	vec3 diffuse = *light_diffuse_ptr * diff * diffuse_texture_color;

	// specular 
	vec3 world_vertex = interp(our_world_vertex);
	vec3 eye_dir = normalized(*this->eye_pos_ptr - world_vertex);
	vec3 half_vec = normalized(eye_dir + *light_dir_ptr);
	float spec = std::pow(std::max(dot(half_vec, normal), 0.f), *this->factor_ptr);
	vec3 specular = spec * *this->light_specular_ptr * diffuse_texture_color;

	color = ambient + diffuse + specular;
	return true;
}
