#include "common.h"

BlinnPhongShaderArgs::BlinnPhongShaderArgs(const vec3 &normal, const vec3 &position, const vec2 texcoord)
: our_normal(normal), our_world_pos(position), our_texcoord(texcoord) {
}

SArgsPtr BlinnPhongShaderArgs::interp(const SArgsPtr &other, float t) const noexcept {
	const auto *rhs = SArgsPtr_cast(other);
	return Make_ArgsPtr(
		common_interp(this, rhs, &BlinnPhongShaderArgs::our_normal, t),
		common_interp(this, rhs, &BlinnPhongShaderArgs::our_world_pos, t),
		common_interp(this, rhs, &BlinnPhongShaderArgs::our_texcoord, t)
	);
}

SArgsPtr BlinnPhongShaderArgs::interp(const SArgsPtr &v2, const SArgsPtr &v3, 
	const vec3 &coord, float depth) const noexcept 
{
	const auto *v1_ptr = this;
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
	uniform_light_dir = get_uniform<vec3>("light_dir");
	uniform_eye_pos = get_uniform<vec3>("eye_pos");
	uniform_diffuse_texture = get_uniform<Texture2d>("diffuse_texture");
	uniform_specular_factor = get_uniform<float>("specular_factor");
	uniform_light_ambient = get_uniform<vec3>("light_ambient");
	uniform_light_diffuse = get_uniform<vec3>("light_diffuse");
	uniform_light_specular = get_uniform<vec3>("light_specular");
}

vec4 BlinnPhong::vertex(const Vertex &vertex, SArgsPtr &args) noexcept {
	args = std::make_shared<BlinnPhongShaderArgs>(BlinnPhongShaderArgs {
		vertex.normal,
		model * vertex.position,
		vertex.texcoords,
	});
	return mvp * vertex.position;
}

bool BlinnPhong::fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept {
	const BlinnPhongShaderArgs *args_ptr = static_cast<const BlinnPhongShaderArgs *>(args.get());
	vec3 our_normal = args_ptr->our_normal;
	vec3 our_position = args_ptr->our_world_pos;
	vec2 our_texcoord = args_ptr->our_texcoord;

	// ambient
	vec3 ambient = uniform_light_ambient * uniform_diffuse_texture.rgb(our_texcoord);
	
	// diffuse
	float diff = std::max(dot(uniform_light_dir, our_normal), 0.f);
	vec3 diffuse = diff * uniform_light_diffuse * uniform_diffuse_texture.rgb(our_texcoord);

	// specular
	vec3 eye_dir = normalized(uniform_eye_pos - our_position);
	vec3 half_vec = normalized(eye_dir + uniform_light_diffuse);
	float spec = std::pow(std::max(dot(half_vec, our_normal), 0.f), uniform_specular_factor);
	vec3 specular = spec * uniform_light_specular * uniform_diffuse_texture.rgb(our_texcoord);

	color = ambient + diffuse + specular;
	return true;
}
