#include "common.h"

Mesh::Mesh(std::vector<Vertex> &&_vertices, std::vector<uint> &&_indices)
: vertices(std::move(_vertices)), indices(std::move(_indices)) {

}

void Mesh::draw(Shader &shader) {
	//TODO
}

