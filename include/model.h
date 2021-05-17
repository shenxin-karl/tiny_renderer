#pragma once

class FrameBuffer;

class Model {
	std::vector<Mesh> meshs;
	std::string		  directory;
	std::unordered_map<std::string, Texture2d>	texture_cache;
public:
	Model(const std::string &path);
	Model(const Model &) = delete;
	Model(Model &&) noexcept = default;
	Model &operator=(Model &&) noexcept = default;
	~Model() = default;

	void draw(FrameBuffer &frame, ShaderBase &shader);

	static Model load_obj(const std::string &path);
private:
	Model() = default;
	Mesh process_mesh(aiMesh *mesh, const aiScene *scene);
	void process_node(aiNode *node, const aiScene *scene);
	std::vector<Texture> load_material_texture(aiMaterial *mat, aiTextureType type, const std::string &name);
	Texture2d load_texture(const std::string &path);
};


