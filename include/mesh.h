#pragma once

struct Shader;
using uint = unsigned int;


struct Vertex {
	vec3	position;
	vec3	normal;
	vec2	texcoords;
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
	std::vector<uint>		indices;
	std::vector<Texture>	textures;
public:
	Mesh(std::vector<Vertex> &&_vertices, std::vector<uint> &&_indices, std::vector<Texture> &&_textures);
	Mesh(Mesh &&) noexcept = default;
	Mesh(const Mesh &) = delete;
	void draw(FrameBuffer &frame, Shader &shader) const;
	~Mesh() = default;
private:
	void process_triangle(FrameBuffer &frame, Shader &shader, std::array<int, 3> our_indices) const;
};