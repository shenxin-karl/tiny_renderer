#include "common.h"

Vertex Vertex::operator*(float t) const noexcept {
	return {
		position	* t,
		normal		* t,
		texcoords	* t,
	};
}

Vertex Vertex::operator+(const Vertex &other) const noexcept {
	return {
		position  + other.position,
		normal	  + other.normal,
		texcoords + other.texcoords,
	};
}

Vertex Vertex::operator-(const Vertex &other) const noexcept {
	return {
		position  - other.position,
		normal    - other.normal,
		texcoords - other.texcoords,
	};
}

Texture::Texture(const std::string &_var, const std::string &_path)
: var(_var), texture(_path) {

}

Texture::Texture(const std::string &_var, const Texture2d &_texture)
: var(_var), texture(_texture) {

}

Mesh::Mesh(std::vector<Vertex> &&_vertices, std::vector<int> &&_indices, std::vector<Texture> &&_textures)
: vertices(std::move(_vertices)), indices(std::move(_indices))
, textures(std::move(_textures)) {

}

void Mesh::draw(FrameBuffer &frame, ShaderBase &shader) const {
	shader.initialize();
	int limit = static_cast<int>(indices.size()) - 2;
	for (int i = 0; i < limit; i += 3)
		process_triangle(frame, shader, { i, i+1, i+2 });
	
}

void Mesh::process_triangle(FrameBuffer &frame, ShaderBase &shader, std::array<int, 3> our_indices) const {
	std::vector<Vertex> out_vertices;
	std::vector<int>    out_indices;
	out_vertices.reserve(12);
	out_indices.reserve(12); 
	for (int i = 0; i < 3; ++i) {
		int index = our_indices[i];
		const Vertex &vertex = vertices[index];
		vec4 point = shader.vertex(vertex, i);
		out_vertices.push_back(Vertex {
			point,
			vertex.normal,
			vertex.texcoords,
		});
		out_indices.push_back(i);
	}

	if (Draw::plane_cutting(out_vertices, out_indices) == 0)		// cutting
		return;
		
	int half_width = frame.get_width() / 2;
	int half_height = frame.get_height() / 2;
	for (auto &v : out_vertices) {
		v.position.head<3>() /= v.position.w();
		v.position.x() = v.position.x() * half_width + half_width;
		v.position.y() = v.position.y() * half_height + half_height;
	}
	
	int limit = static_cast<int>(out_indices.size()) - 2;
	for (size_t i = 0; i < limit; i += 3) {
		int idx1 = out_indices[i];
		int idx2 = out_indices[i+1];
		int idx3 = out_indices[i+2];
		if (idx1 < 0)
			continue;
		if (backface_culling(shader, out_vertices[idx1], out_vertices[idx2], out_vertices[idx3]))
			continue;
		Draw::triangle(frame, shader, {
			&out_vertices[idx1],
			&out_vertices[idx2],
			&out_vertices[idx3],
		});
	}
}

#define USE_BACKFACE_CULLING_OPTIMIZE
bool Mesh::backface_culling(ShaderBase &shader, const Vertex &v1, const Vertex &v2, const Vertex &v3) const noexcept {
	if (!shader.is_enable_face_culling())
		return false;

#ifdef USE_BACKFACE_CULLING_OPTIMIZE
	vec2 normal_vec1 = v2.position.head<2>() - v1.position.head<2>();
	vec2 normal_vec2 = v3.position.head<2>() - v1.position.head<2>();
	float cosine = -((normal_vec1.x() * normal_vec2.y()) - (normal_vec1.y() * normal_vec2.x()));
#else
	constexpr vec3 look_at = { 0, 0, -1 };
	vec3 vec1 = v2.position - v1.position;
	vec3 vec2 = v3.position - v1.position;
	vec3 normal = cross(vec1, vec2);
	float cosine = dot(normal, look_at);
#endif // USE_BACKFACE_CULLING_OPTIMIZE
	bool res = shader.get_face_culling_func()(cosine);
	return res;
}
