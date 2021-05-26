#pragma once

class FrameBuffer;
struct ShaderBase;

struct Vertex {
	vec4	position;
	vec3	normal;
	vec2	texcoords;
public:
	Vertex operator*(float t) const noexcept;
	Vertex operator+(const Vertex &other) const noexcept;
	void perspective_divide();
};

struct Texture {
	std::string		var;		// 变量名
	Texture2d		texture;	// 纹理对象
public:
	Texture(const std::string &_var, const std::string &_path);
	Texture(const std::string &_var, const Texture2d &_texture);
	Texture(const Texture &) = default;
	Texture(Texture &&) noexcept = default;
	Texture &operator=(const Texture &) = default;
};

class Mesh {
	std::vector<Vertex>		vertices;
	std::vector<int>		indices;
	std::vector<Texture>	textures;
public:
	Mesh(std::vector<Vertex> &&_vertices, std::vector<int> &&_indices, std::vector<Texture> &&_textures);
	Mesh(Mesh &&) noexcept = default;
	Mesh(const Mesh &) = delete;
	void draw(FrameBuffer &frame, ShaderBase &shader) const;
	~Mesh() = default;
private:
	void process_triangle(FrameBuffer &frame, ShaderBase &shader, std::array<int, 3> our_indices) const;
	bool backface_culling(ShaderBase &shader, const Vertex &v1, const Vertex &v2, const Vertex &v3) const noexcept;
	int plane_cutting(std::vector<Vertex> &vertices, std::vector<int> &indices) const noexcept;
	int plane_cutting_triangle(std::vector<Vertex> &vertices,
								std::span<int, 3> indices,
								size_t idx,
								std::function<bool(float, float)> compare_func) const noexcept;

	bool outside(const vec4 &point, std::function<bool(float, float)> compare_func, size_t idx, float limit) const noexcept;
	Vertex interp_vertex(const Vertex &start, const Vertex &last, float t) const noexcept;
	float get_plane_ratio(const Vertex &start, const Vertex &last, size_t idx) const noexcept;
};