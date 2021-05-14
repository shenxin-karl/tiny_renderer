#pragma once

struct Shader;
using uint = unsigned int;


struct Vertex {
	vec3	position;
	vec3	normal;
	vec2	texcoords;
};

struct Texture {
	std::string		var;		// ������
	Texture2d		texture;	// �������
public:
	Texture(const std::string &_var, const std::string &_path);
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
	void draw(Shader &shader);
	~Mesh() = default;
};