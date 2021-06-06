#include "common.h"

SArgsPtr BlinnPhongShaderArgs::interp(const SArgsPtr &other, float t) const noexcept {
	const auto *rhs = SArgsPtr_cast(other);
	return Make_ArgsPtr(
		common_interp(this, rhs, &BlinnPhongShaderArgs::our_normal, t),
		common_interp(this, rhs, &BlinnPhongShaderArgs::our_world_pos, t),
		common_interp(this, rhs, &BlinnPhongShaderArgs::our_texcoord, t)
	);
}

SArgsPtr BlinnPhongShaderArgs::interp(const SArgsPtr &v1, const SArgsPtr &v2, const SArgsPtr &v3,
	const vec3 &coord, float depth) const noexcept 
{
	const auto *v1_ptr = SArgsPtr_cast(v1);
	const auto *v2_ptr = SArgsPtr_cast(v2);
	const auto *v3_ptr = SArgsPtr_cast(v3);
	return Make_ArgsPtr(
		common_interp(v1_ptr, v2_ptr, v3_ptr, &BlinnPhongShaderArgs::our_normal, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &BlinnPhongShaderArgs::our_world_pos, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &BlinnPhongShaderArgs::our_texcoord, coord, depth)
	);
};
	
void BlinnPhongShaderArgs::perspective_divide(float inverse_z) noexcept {
	our_normal *= inverse_z;
	our_world_pos *= inverse_z;
	our_texcoord *= inverse_z;
}


void BlinnPhong::initialize() noexcept {
	light_dir_ptr = &get_uniform<vec3>("light_dir");
	diffuse_texture_ptr = &get_uniform<Texture2d>("diffuse_texture");
	light_ambient_ptr = &get_uniform<vec3>("light_ambient");
	light_diffuse_ptr = &get_uniform<vec3>("light_diffuse");
	light_specular_ptr = &get_uniform<vec3>("light_specular");
	eye_pos_ptr = &get_uniform<vec3>("eye_pos");
	factor_ptr = &get_uniform<float>("specular_factor");
}

vec4 BlinnPhong::vertex(const Vertex &vertex, int idx) noexcept {
	auto res = mvp * vertex.position;
	float inverse_z = 1.f / res.w();
	our_world_pos[idx] = model * vertex.position * inverse_z;
	our_normal[idx] = vertex.normal * inverse_z;
	our_texcoords[idx] = vertex.texcoords * inverse_z;
	return res;
}

bool BlinnPhong::fragment(const vec3 &point, vec3 &color) noexcept {
	vec3 world_pos = interp(our_world_pos);
	vec3 world_normal = interp(our_normal);
	const vec3 &light_dir = *this->light_dir_ptr;

	const Texture2d &diffuse_texture = *this->diffuse_texture_ptr;
	vec2 texcoords = interp(our_texcoords);
	vec3 diffuse_texture_color = diffuse_texture.rgb(texcoords);

	// ambient color
	const vec3 &light_ambient = *this->light_ambient_ptr;
	vec3 ambient = light_ambient * diffuse_texture_color;

	 // diffuse color
	const vec3 &light_diffuse = *this->light_diffuse_ptr;
	float diff = std::max(dot(world_normal, light_dir), 0.f);
	vec3 diffuse = diff * diffuse_texture_color * light_diffuse;

	// specular color
	const vec3 &light_specular = *this->light_specular_ptr;
	const vec3 &eye_pos = *this->eye_pos_ptr;
	vec3 eye_dir = normalized(eye_pos - world_pos);
	vec3 half_vec = normalized(eye_dir + light_dir);
	float factor = *this->factor_ptr;
	float spec = std::pow(std::max(dot(world_normal, half_vec), 0.f), factor);
	vec3 specular = spec * diffuse_texture_color * light_specular;

	color = ambient + diffuse + specular;
	return true;
}
