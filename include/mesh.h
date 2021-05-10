#pragma once

class Shader;
using uint = unsigned int;


struct Vertex {
	vec3	position;
	vec3	normal;
	vec2	texcoords;
};

//struct Texture {
//	std::string var;
//	Image2D		data;
//};

class Mesh {
	std::vector<Vertex>	vertices;
	std::vector<uint>	indices;
public:
	Mesh(std::vector<Vertex> &&_vertices, std::vector<uint> &&_indices);
	Mesh(Mesh &&) = default;
	Mesh(const Mesh &) = delete;
	void draw(Shader &shader);
	~Mesh() = default;
};