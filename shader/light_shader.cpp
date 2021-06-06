#include "common.h"

LightShaderArgs::LightShaderArgs(const vec3 &normal) : our_normal(normal) {
}

SArgsPtr LightShaderArgs::interp(const SArgsPtr &other, float t) const noexcept {
	const auto *rhs = SArgsPtr_cast(other);
	return Make_ArgsPtr(
		common_interp(this, rhs, &LightShaderArgs::our_normal, t)
	);
}

SArgsPtr LightShaderArgs::interp(const SArgsPtr &v1, const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coords, float depth) const noexcept {
	const auto *v1_ptr = SArgsPtr_cast(v1);
	const auto *v2_ptr = SArgsPtr_cast(v2);
	const auto *v3_ptr = SArgsPtr_cast(v3);
	return Make_ArgsPtr(
		common_interp(v1_ptr, v2_ptr, v3_ptr, &LightShaderArgs::our_normal, coords, depth)
	);
}

void LightShaderArgs::perspective_divide(float inverse_z) noexcept {
	our_normal *= inverse_z;
}

vec4 LightShader::vertex(const Vertex &vertex, SArgsPtr &args) noexcept {
	args = std::make_unique<LightShaderArgs>(vertex.normal);
	return mvp * vertex.position;
}

bool LightShader::fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept {
	const LightShaderArgs *args_ptr = static_cast<const LightShaderArgs *>(args.get());
	const vec3 &light_dir = get_uniform<vec3>("light_dir");
	float gray = std::max(dot(light_dir, args_ptr->our_normal), 0.f);
	color = { gray, gray, gray };
	return true;
}

