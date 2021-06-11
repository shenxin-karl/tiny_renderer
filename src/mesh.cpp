#include "common.h"

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
	std::vector<VertexRes> out_vertices;
	out_vertices.reserve(3);
	for (int i = 0; i < 3; ++i) {
		int index = our_indices[i];
		const Vertex &vertex = vertices[index];
		SArgsPtr args = std::make_shared<ShaderArgsBase>();
		vec4 point = shader.vertex(vertex, args);
		out_vertices.push_back({ point, std::move(args) });
	}
	
	if (!Draw::plane_cutting(out_vertices))
		return;

	int half_width = frame.get_width() / 2;
	int half_height = frame.get_height() / 2;
	for (auto &v : out_vertices) {
		float inverse_z = 1.f / v.position.w();
		v.position.head<3>() *= inverse_z;
		v.position.x() = v.position.x() * half_width + half_width;
		v.position.y() = v.position.y() * half_height + half_height;
		v.args->perspective_divide(inverse_z);
	}
	
	size_t limit = out_vertices.size() - 2;
	for (size_t i = 0; i < limit; ++i) {
		VertexRes &v1 = out_vertices[0];
		VertexRes &v2 = out_vertices[i+1];
		VertexRes &v3 = out_vertices[i+2];
		if (backface_culling(shader, v1, v2, v3))
			continue;
		Draw::triangle(frame, shader, { &v1, &v2, &v3 });
	}
}

#define USE_BACKFACE_CULLING_OPTIMIZE
bool Mesh::backface_culling(ShaderBase &shader, const VertexRes &v1, const VertexRes &v2, const VertexRes &v3) const noexcept {
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
