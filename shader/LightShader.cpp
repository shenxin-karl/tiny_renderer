#include "common.h"

vec4 LightShader::vertex(const Vertex &vertex, int idx) {
	return mvp * vertex.position;
}

bool LightShader::fragment(std::array<Vertex *, 3> &vertices, vec3 &color) {
	vec2 texcoords = interp(vertices[0]->texcoords, vertices[1]->texcoords, vertices[2]->texcoords);
	const Texture2d &diffuse_texture = get_uniform<Texture2d>("diffuse_texture");
	color = diffuse_texture.texture(texcoords);
	return true;
}
