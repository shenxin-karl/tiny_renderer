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
	for (int i = 0; i < limit; i += 3) {
		process_triangle(frame, shader, { i, i+1, i+2 });
	}
}

struct VertexResult {
	vec4	position;
	int		index;
};
void Mesh::process_triangle(FrameBuffer &frame, ShaderBase &shader, std::array<int, 3> our_indices) const {
	std::array<VertexResult, 3> vertex_res;
	for (int i = 0; i < 3; ++i) {
		int index = our_indices[i];
		const Vertex &vertex = vertices[index];
		vec4 point = shader.vertex(vertex, i);
		vertex_res[i] = { point, index };
	}

	std::vector<Vertex> out_vertices;
	std::vector<int>    out_indices;
	out_vertices.reserve(3);
	out_indices.reserve(3);
	if (plane_cutting(out_vertices, out_indices) < 0)		// cutting
		return;

	for (auto &v : out_vertices)		
		v.perspective_divide();
	
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

int Mesh::plane_cutting(std::vector<Vertex> &vertices, std::vector<int> &indices) const {
	static std::pair<size_t, std::function<bool(float, float)>> planes[] = {
		{ 0, std::less<float>()		},
		{ 0, std::greater<float>(), },
		{ 1, std::less<float>(),	},
		{ 1, std::greater<float>(), },
		{ 2, std::less<float>(),	},
		{ 2, std::greater<float>(), },
	};
	int size = indices.size();
	for (auto iter = std::begin(planes); iter != std::end(planes) && size > 0; ++iter) {
		for (int i = 0; i < indices.size()-2 && size > 0; ++i) {
			if (indices[i] < 0)
				continue;

			std::span<int, 3> view(&indices[i], 3);
			size += plane_cutting_triangle(vertices, view, iter->first, iter->second);
		}
	}
	return size;
}

int Mesh::plane_cutting_triangle(std::vector<Vertex> &vertices,
								  std::span<int, 3> indices,
								  size_t idx,
								  std::function<bool(float, float)> compare_func) const noexcept
{
	std::array<bool, 3> outside_flag = { false };
	std::array<int, 3> outside_indices = { -1 };
	int outside_size = 0;
	for (int i = 0; auto vertex_idx : indices) {
		const Vertex &v = vertices[vertex_idx];
		bool is_outside = outside(v.position, compare_func, idx, 1.f);
		if (is_outside) {
			outside_flag[i] = true;
			outside_indices[outside_size] = i;
			++outside_size;
		}
		++i;
	}

	switch (outside_size) {
	case 3:
	{
		std::fill(indices.begin(), indices.end(), -1);
		return -3;
	}
	case 1:
	{
		// todo case1
		return 3;
	}
	case 2:
	{
		const Vertex &outside_vertex1 = vertices[outside_indices[0]];
		const Vertex &outside_vertex2 = vertices[outside_indices[1]];
		auto iter = std::find(outside_flag.begin(), outside_flag.end(), true);
		assert(iter != outside_flag.end());
		int inside_idx = indices[static_cast<int>(iter - outside_flag.begin())];
		const Vertex &inside_vertex = vertices[inside_idx];
		// 插值第一个顶点
		float t1 = get_plane_ratio(inside_vertex, outside_vertex1, idx);
		vertices[outside_indices[0]] = interp_vertex(inside_vertex, outside_vertex1, t1);
		// 插值第二个顶点
		float t2 = get_plane_ratio(inside_vertex, outside_vertex2, idx);
		vertices[outside_indices[1]] = interp_vertex(inside_vertex, outside_vertex1, t2);
		return 0;
	}
	}
	assert(false);
	return 0;
}

bool Mesh::outside(const vec4 &point, std::function<bool(float, float)> compare_func, size_t idx, float limit) const noexcept {
	return compare_func(point[idx], limit);
}

Vertex Mesh::interp_vertex(const Vertex &start, const Vertex &last, float t) const noexcept {
	return (start * t) +  (last * (1.f-t));
}

float Mesh::get_plane_ratio(const Vertex &start, const Vertex &last, size_t idx) const noexcept {
	float f1 = start.position[idx];
	float f2 = last.position[idx];
	float w1 = start.position.w();
	float w2 = last.position.w();
	return (f1 - w1) / ((w2 - w1) - (f2 - f1));
}
