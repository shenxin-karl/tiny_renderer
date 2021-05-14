#include "common.h"

Texture::Texture(const std::string &_var, const std::string &_path)
: var(_var), texture(_path) {

}

Mesh::Mesh(std::vector<Vertex> &&_vertices, std::vector<uint> &&_indices, std::vector<Texture> &&_textures)
: vertices(std::move(_vertices)), indices(std::move(_indices))
, textures(std::move(_textures)) {

}

void Mesh::draw(FrameBuffer &frame, Shader &shader) const {
	int limit = static_cast<int>(indices.size());
	for (int i = 0; i < limit-2; i += 3) 
		process_triangle(frame, shader, { i, i+1, i+2 });
}

void Mesh::process_triangle(FrameBuffer &frame, Shader &shader, std::array<int, 3> our_indices) const {
	std::array<vec4, 3> vertex_res;
	for (int i = 0; i < 3; ++i) {
		int index = our_indices[i];
		const Vertex &vertex = vertices[index];
		vertex_res[i] = shader.vertex(vertex);
	}

	std::vector<Vertex> out_vertices;
	std::vector<uint>	out_indices;
	out_vertices.reserve(5);
	// TODO Æë´Î²Ã¼ô

	for (int i = 0; i < 3; ++i) {
		vec4 &point = vertex_res[i];
		const Vertex &vertex = vertices[i];
		out_vertices.push_back(Vertex {
			vec3(point.x(), point.y(), point.z()),
			vertex.normal,
			vertex.texcoords,
		});
		out_indices.push_back(i);
	}

	for (int i = 0; i < indices.size(); i += 3) {
		Draw::triangle(frame, shader, {
			out_vertices[i],
			out_vertices[i+1],
			out_vertices[i+2],
		});
	}
}

