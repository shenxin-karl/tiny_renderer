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
	shader.initialize();
	int limit = static_cast<int>(indices.size());
	for (int i = 0; i < limit-2; i += 3) {
		process_triangle(frame, shader, { i, i+1, i+2 });
	}
}

struct VertexResult {
	vec4	position;
	uint	index;
};
void Mesh::process_triangle(FrameBuffer &frame, ShaderBase &shader, std::array<int, 3> our_indices) const {
	std::array<VertexResult, 3> vertex_res;
	for (int i = 0; i < 3; ++i) {
		uint index = our_indices[i];
		const Vertex &vertex = vertices[index];
		vec4 point = shader.vertex(vertex, i);
		vertex_res[i] = { point, index };
	}

	std::vector<Vertex> out_vertices;
	std::vector<uint>   out_indices;
	out_vertices.reserve(6);
	out_indices.reserve(6);
	for (int i = 0; VertexResult & ver_res : vertex_res) {
		if (ver_res.position.w() >= 0.f)
			return;

		float inverse_world_z = 1.f / ver_res.position.w();
		for (size_t i = 0; i < ver_res.position.size()-1; ++i)
			ver_res.position[i] *= inverse_world_z;

		const Vertex &ver = vertices[ver_res.index];
		out_indices.push_back(i++);
		out_vertices.push_back(Vertex {
			ver_res.position,
			ver.normal    * inverse_world_z,
			ver.texcoords * inverse_world_z,
		});
	}

	int limit = static_cast<int>(out_indices.size()) - 2;
	for (size_t i = 0; i < limit; i += 3u) {
		if (backface_culling(shader, out_vertices[i], out_vertices[i+1], out_vertices[i+2]))
			continue;

		Draw::triangle(frame, shader, {
			&out_vertices[i],
			&out_vertices[i+1],
			&out_vertices[i+2],
		});
	}
}

bool Mesh::backface_culling(ShaderBase &shader, const Vertex &v1, const Vertex &v2, const Vertex &v3) const noexcept {
	if (!shader.is_enable_face_culling())
		return false;

	constexpr vec3 look_at = { 0, 0, -1 };
	vec3 vec1 = v2.position - v1.position;
	vec3 vec2 = v3.position - v1.position;
	vec3 normal = cross(vec1, vec2);
	float cosine = dot(normal, look_at);
	bool res = shader.get_face_culling_func()(cosine);
	return res;
}

