#include "common.h"

Texture::Texture(const std::string &_var, const std::string &_path)
: var(_var), texture(_path) {

}


Texture::Texture(const std::string &_var, const Texture2d &_texture) 
: var(_var), texture(_texture) {

}

Mesh::Mesh(std::vector<Vertex> &&_vertices, std::vector<uint> &&_indices, std::vector<Texture> &&_textures)
: vertices(std::move(_vertices)), indices(std::move(_indices))
, textures(std::move(_textures)) {

}

void Mesh::draw(FrameBuffer &frame, ShaderBase &shader) const {
	int limit = static_cast<int>(indices.size());
	for (int i = 0; i < limit-2; i += 3) {
		process_triangle(frame, shader, { i, i+1, i+2 });
	}
}

void Mesh::process_triangle(FrameBuffer &frame, ShaderBase &shader, std::array<int, 3> our_indices) const {
	std::vector<Vertex> out_vertices;
	out_vertices.reserve(5);
	std::vector<uint>	out_indices;
	for (int i = 0; i < 3; ++i) {
		int index = our_indices[i];
		const Vertex &vertex = vertices[index];
		vec4 point = shader.vertex(vertex, i);
		if (point.w() >= 0.f)
			return;

		for (size_t idx = 0; idx < point.size()-1; ++idx)
			point[idx] /= point.w();

		float z = point.z();
		out_vertices.push_back(Vertex{
			point,
			vertex.normal	 / z,
			vertex.texcoords / z,
		});
		out_indices.push_back(i);
	}


	int limit = static_cast<int>(out_indices.size()) - 2;
	for (int i = 0; i < limit; i += 3) {
		//std::cout << "\t" << out_vertices[i].position 
		//		  << "\t" << out_vertices[i+1].position
		//		  << "\t" << out_vertices[i+2].position 
		//		  << std::endl;
		Draw::triangle(frame, shader, {
			&out_vertices[i],
			&out_vertices[i+1],
			&out_vertices[i+2],
		});
	}
}

