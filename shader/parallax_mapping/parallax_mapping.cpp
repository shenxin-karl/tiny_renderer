#include "common.h"

ParallaxMappingShaderArgs::ParallaxMappingShaderArgs(const vec2 &texcoord, const vec3 &tangent_position, 
	const vec3 &tangent_eye_pos, const vec3 &tangent_light_dir) 
: our_texcoord(texcoord), our_tangent_position(tangent_position)
, our_tangent_eye_pos(tangent_eye_pos), our_tangent_light_dir(tangent_light_dir) {

}

SArgsPtr ParallaxMappingShaderArgs::interp(const SArgsPtr &other, float t) const noexcept {
	const auto *rhs = SArgsPtr_cast(other);
	return Make_ArgsPtr(
		common_interp(this, rhs, &ParallaxMappingShaderArgs::our_texcoord, t),
		common_interp(this, rhs, &ParallaxMappingShaderArgs::our_tangent_position, t),
		common_interp(this, rhs, &ParallaxMappingShaderArgs::our_tangent_eye_pos, t),
		common_interp(this, rhs, &ParallaxMappingShaderArgs::our_tangent_light_dir, t)
	);
}

SArgsPtr ParallaxMappingShaderArgs::interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept {
	const auto *v1_ptr = this;
	const auto *v2_ptr = SArgsPtr_cast(v2);
	const auto *v3_ptr = SArgsPtr_cast(v3);
	return Make_ArgsPtr(
		common_interp(v1_ptr, v2_ptr, v3_ptr, &ParallaxMappingShaderArgs::our_texcoord, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &ParallaxMappingShaderArgs::our_tangent_position, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &ParallaxMappingShaderArgs::our_tangent_eye_pos, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &ParallaxMappingShaderArgs::our_tangent_light_dir, coord, depth)
	);
}

void ParallaxMappingShaderArgs::perspective_divide(float inverse_z) noexcept {
	our_texcoord *= inverse_z;
	our_tangent_position *= inverse_z;
	our_tangent_eye_pos *= inverse_z;
	our_tangent_light_dir *= inverse_z;
}

void ParallaxMappingShader::initialize() noexcept {
	uniform_eye_pos = get_uniform<vec3>("eye_pos");
	uniform_light_dir = get_uniform<vec3>("light_dir");
	uniform_normal_matrix = transpose(inverse(mat3(model)));
	uniform_diffuse_map = get_uniform<Texture2d>("diffuse_map");
	uniform_normal_map = get_uniform<Texture2d>("normal_map");
	uniform_depth_map = get_uniform<Texture2d>("depth_map");
	uniform_height_scale = get_uniform<float>("height_scale");
}

vec4 ParallaxMappingShader::vertex(const Vertex &vertex, SArgsPtr &args) noexcept {
	vec3 T = normalized(uniform_normal_matrix * vertex.tangent);
	vec3 B = normalized(uniform_normal_matrix * vertex.bitangent);
	vec3 N = normalized(uniform_normal_matrix * vertex.normal);
	mat3 reverse_tbn = inverse_tbn(T, B, N);
	vec3 model_position = vec3(model * vertex.position);
	args = std::make_shared<ParallaxMappingShaderArgs>(
		vertex.texcoords,
		reverse_tbn * model_position,
		reverse_tbn * uniform_eye_pos,
		reverse_tbn * uniform_light_dir
	);
	return mvp * vertex.position;
}

bool ParallaxMappingShader::fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept {
	const auto *args_ptr = static_cast<const ParallaxMappingShaderArgs *>(args.get());

	vec3 normal = uniform_normal_map.normal(args_ptr->our_texcoord);
	vec3 view_dir = normalized(args_ptr->our_tangent_eye_pos - args_ptr->our_tangent_position);
	vec2 depth_texcoord = parallax_mapping_texcoord(view_dir, args_ptr->our_texcoord);
	vec3 diffuse_color = uniform_diffuse_map.rgb(depth_texcoord);

	// todo compute blinn-phong shading
}

vec2 ParallaxMappingShader::parallax_mapping_texcoord(const vec3 &view_dir, const vec2 &texcoord) const {
	float depth = uniform_depth_map.rgb(texcoord).r();
	vec2 p = (vec2(view_dir) / view_dir.z()) * (depth * uniform_height_scale);
	return texcoord - p;
}
