#include "common.h"
#if 0
void SkyboxShader::initialize() noexcept {
	uniform_skybox_texture = &get_uniform<TextureCube>("skybox_texture");
	auto trans = view;
	//trans(0, 3) = 0.f;
	//trans(1, 3) = 0.f;
	//trans(2, 3) = 0.f;
	uniform_vp = projection * trans;
}

vec4 SkyboxShader::vertex(const Vertex &vertex, int idx) noexcept {
	auto res = mvp * vertex.position;
	float invers_z = 1.f / res.w();
	our_world_position[idx] = vertex.position *invers_z;
	return res;
}

bool SkyboxShader::fragment(const vec3 &point, vec3 &color) noexcept {
	vec3 texcoord = interp(our_world_position);
	color = uniform_skybox_texture->rgb(texcoord);
	return true;
}
#endif