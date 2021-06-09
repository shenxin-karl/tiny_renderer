#include "common.h"

SkyboxReflectArgs::SkyboxReflectArgs(const vec3 &position, const vec3 &normal)
: our_position(position), our_normal(normal) {
}

SArgsPtr SkyboxReflectArgs::interp(const SArgsPtr &other, float t) const noexcept {
	const auto *rhs = SArgsPtr_cast(other);
	return Make_ArgsPtr(
		common_interp(this, rhs, &SkyboxReflectArgs::our_position, t),
		common_interp(this, rhs, &SkyboxReflectArgs::our_normal, t)
	);
}

SArgsPtr SkyboxReflectArgs::interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept {
	const auto *v1_ptr = this;
	const auto *v2_ptr = SArgsPtr_cast(v2);
	const auto *v3_ptr = SArgsPtr_cast(v3);
	return Make_ArgsPtr(
		common_interp(v1_ptr, v2_ptr, v3_ptr, &SkyboxReflectArgs::our_position, coord, depth),
		common_interp(v1_ptr, v2_ptr, v3_ptr, &SkyboxReflectArgs::our_normal, coord, depth)
	);
}

void SkyboxReflectArgs::perspective_divide(float inverse_z) noexcept {
	our_position *= inverse_z;
	our_normal *= inverse_z;
}

void SkyboxReflect::initialize() noexcept {
	uniform_skybox_texture = get_uniform<TextureCube>("skybox_texture");
	uniform_eye_pos = get_uniform<vec3>("eye_pos");
	uniform_normal_matrix = transpose(inverse(mat3(model)));
}

vec4 SkyboxReflect::vertex(const Vertex &vertex, SArgsPtr &args) noexcept {
	args = std::make_shared<SkyboxReflectArgs>(SkyboxReflectArgs{
		model * vertex.position,
		uniform_normal_matrix * vertex.normal
	});
	return mvp * vertex.position;
}

bool SkyboxReflect::fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept {
	const auto *args_ptr = static_cast<const SkyboxReflectArgs *>(args.get());
	vec3 our_position = args_ptr->our_position;
	vec3 our_normal = args_ptr->our_position;

	vec3 eye_dir = normalized(uniform_eye_pos - our_position);
	vec3 reflect = Draw::reflect(eye_dir, our_normal);
	color = uniform_skybox_texture.rgb(reflect);
	return true;
}
