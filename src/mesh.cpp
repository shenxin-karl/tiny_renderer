#include "common.h"

Texture::Texture(const std::string &_var, const std::string &_path)
: var(_var), texture(_path) {

}

Mesh::Mesh(std::vector<Vertex> &&_vertices, std::vector<uint> &&_indices, std::vector<Texture> &&_textures)
: vertices(std::move(_vertices)), indices(std::move(_indices))
, textures(std::move(_textures)) {

}

void Mesh::draw(Shader &shader) {
	//TODO
}

