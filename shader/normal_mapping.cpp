#include "common.h"

NormalMappingShaderArgs::NormalMappingShaderArgs(const vec3 &position, const vec2 &texcoord, const vec3 &eye_pos, const vec3 &light_dir)
: our_position(position), our_texcoord(texcoord), our_eye_pos(eye_pos), our_light_dir(light_dir) {}

SArgsPtr NormalMappingShaderArgs::interp(const SArgsPtr &other, float t) const noexcept {
	const auto *rhs = SArgsPtr_cast(other);
	return Make_ArgsPtr(
		common_interp(this, rhs, &NormalMappingShaderArgs::our_position, t),
		common_interp(this, rhs, &NormalMappingShaderArgs::our_texcoord, t),
		common_interp(this, rhs, &NormalMappingShaderArgs::our_eye_pos, t),
		common_interp(this, rhs, &NormalMappingShaderArgs::our_light_dir, t)
	);
}

SArgsPtr NormalMappingShaderArgs::interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept {
	const auto *v1_ptr = this;
	const auto *v2_ptr = SArgsPtr_cast(v2);
	const auto *v3_ptr = SArgsPtr_cast(v3);
	return Make_ArgsPtr(
		common_interp(v1_ptr, v2_ptr, v3_ptr, &NormalMappingShaderArgs::our_position, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &NormalMappingShaderArgs::our_texcoord, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &NormalMappingShaderArgs::our_eye_pos, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &NormalMappingShaderArgs::our_light_dir, coord, depth)
	);
}

void NormalMappingShaderArgs::perspective_divide(float inverse_z) noexcept {
	our_position *= inverse_z;
	our_texcoord *= inverse_z;
	our_eye_pos *= inverse_z;
	our_light_dir *= inverse_z;
}

void NormalMappingShader::initialize() noexcept {
	uniform_diffuse_texture = get_uniform<Texture2d>("diffuse_texture");
	uniform_normal_texture = get_uniform<Texture2d>("normal_texture");
	uniform_eye_pos = get_uniform<vec3>("eye_pos");
	uniform_light_dir = get_uniform<vec3>("light_dir");
	uniform_light_ambient = get_uniform<vec3>("light_ambient");
	uniform_light_diffuse = get_uniform<vec3>("light_diffuse");
	uniform_light_specular = get_uniform<vec3>("light_specular");
	uniform_specular_factor = get_uniform<float>("specular_factor");
	uniform_normal_matrix = transpose(inverse(mat3(model)));
}

vec4 NormalMappingShader::vertex(const Vertex &vertex, SArgsPtr &args) noexcept {
	vec3 T = normalized(uniform_normal_matrix * vertex.tangent);
	vec3 B = normalized(uniform_normal_matrix * vertex.bitangent);
	vec3 N = normalized(uniform_normal_matrix * vertex.normal);
	mat3 reverse_tbn = transpose(tbn(T, B, N));
	vec3 model_position = vec3(model * vertex.position);
	args = std::make_shared<NormalMappingShaderArgs>(
		reverse_tbn * model_position,
		vertex.texcoords,
		reverse_tbn * uniform_eye_pos,
		reverse_tbn * uniform_light_dir
	);
	return mvp * vertex.position;
}

bool NormalMappingShader::fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept {
	const NormalMappingShaderArgs *args_ptr = static_cast<const NormalMappingShaderArgs *>(args.get());
	
	vec3 diffuse_color = uniform_diffuse_texture.rgb(args_ptr->our_texcoord);
	vec3 ambient = uniform_light_ambient * diffuse_color;

	// diffuse
	vec3 light_dir = normalized(args_ptr->our_light_dir);
	vec3 normal = uniform_normal_texture.normal(args_ptr->our_texcoord);
	float diff = std::max(dot(light_dir, normal), 0.f);
	vec3 diffuse = diff * uniform_light_diffuse * diffuse_color;
	// specular
	vec3 eye_dir = normalized(args_ptr->our_eye_pos - args_ptr->our_position);
	vec3 half_vec = normalized(eye_dir + light_dir);
	float spec = std::pow(std::max(dot(half_vec, normal), 0.f), uniform_specular_factor);
	vec3 specular = spec * uniform_light_specular * diffuse_color;

	color = ambient + diffuse + specular;
	return true;
}

