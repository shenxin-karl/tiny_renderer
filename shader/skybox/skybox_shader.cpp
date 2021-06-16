#include "common.h"

SkyboxShaderArgs::SkyboxShaderArgs(const vec3 &texcoord) : our_texcoord(texcoord) {
}

SArgsPtr SkyboxShaderArgs::interp(const SArgsPtr &other, float t) const noexcept {
	const SkyboxShaderArgs *rhs = SArgsPtr_cast(other);
	return Make_ArgsPtr(
		common_interp(this, rhs, &SkyboxShaderArgs::our_texcoord, t)
	);
}

SArgsPtr SkyboxShaderArgs::interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept {
	const SkyboxShaderArgs *v1_ptr = this;
	const SkyboxShaderArgs *v2_ptr = SArgsPtr_cast(v2);
	const SkyboxShaderArgs *v3_ptr = SArgsPtr_cast(v3);
	return Make_ArgsPtr(
		common_interp(v1_ptr, v2_ptr, v3_ptr, &SkyboxShaderArgs::our_texcoord, coord, depth)
	);
}

void SkyboxShaderArgs::perspective_divide(float inverse_z) noexcept {
	our_texcoord *= inverse_z;
}


void SkyboxShader::initialize() noexcept {
	uniform_skybox_texture = get_uniform<TextureCube>("skybox_texture");
	auto trans = view;
	trans(0, 3) = 0.f;
	trans(1, 3) = 0.f;
	trans(2, 3) = 0.f;
	uniform_vp = projection * trans;
}

vec4 SkyboxShader::vertex(const Vertex &vertex, SArgsPtr &args) noexcept {
	args = std::make_shared<SkyboxShaderArgs>(SkyboxShaderArgs{
		vertex.position.head<3>()
	});
	auto res = uniform_vp * vertex.position;
	res.z() = -res.w();
	return res;
}

bool SkyboxShader::fragment(const vec3 &point, const SArgsPtr &args, vec3 &color) noexcept {
	const SkyboxShaderArgs *args_ptr = static_cast<const SkyboxShaderArgs *>(args.get());
	color = uniform_skybox_texture.rgb(args_ptr->our_texcoord);
	return true;
}
