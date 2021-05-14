#include "common.h"
#include "stb_image.h"

Model::Model(const std::string &path) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		std::cerr << "ERROR:ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	process_node(scene->mRootNode, scene);
}

Model Model::load_obj(const std::string &path) {
	std::ifstream file(path, std::ios::in);
	if (!file.is_open())
		return {};

	std::vector<Vertex> vertices;
	std::vector<uint> indices;
	std::vector<vec3> all_vertices;
	std::vector<vec3> all_normal;
	std::vector<vec2> all_texcoords;

	std::string line;
	while (!file.eof()) {
		std::getline(file, line);
		std::stringstream isbuf(line);
		char c_trash;
		if (line.compare(0, 2, "v ") == 0) {				// 加载顶点
			vec3 vertex;
			isbuf >> c_trash >> vertex;
			all_vertices.emplace_back(std::move(vertex));

		} else if (line.compare(0, 2, "vn") == 0) {			// 加载法线
			vec3 normal;
			isbuf >> c_trash >> c_trash >> normal;
			all_normal.emplace_back(std::move(normal));

		} else if (line.compare(0, 2, "vt") == 0) {			// 加载纹理坐标
			vec2 texcoords;
			isbuf >> c_trash >> c_trash >> texcoords;
			all_texcoords.emplace_back(std::move(texcoords));

		} else if (line.compare(0, 2, "f ") == 0) {		   // 加载一个三角形
			int vertex_idx;
			int texture_idx;
			int normal_idx;
			isbuf >> c_trash;
			while (isbuf >> vertex_idx >> c_trash
					     >> texture_idx >> c_trash
				         >> normal_idx
				) {

				vertices.push_back(Vertex {
					all_vertices[vertex_idx-1],
					all_normal[normal_idx-1],
					all_texcoords[texture_idx-1],
				});
			}
		}
	}

	// 构建索引
	indices.reserve(vertices.size());
	std::generate_n(std::back_insert_iterator(indices), vertices.size(), [n = 0]() mutable {
		return n++;
	});

	Mesh mesh(std::move(vertices), std::move(indices), {});
	Model res;
	res.meshs.push_back(std::move(mesh));
	res.directory = path.substr(0, path.find_last_of('/') + 1);
	return res;
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<Vertex>	vertices;
	for (size_t i = 0; i < mesh->mNumVertices; ++i) {		// 加载顶点
		Vertex vertex;
		vertex.position.x() = mesh->mVertices[i].x;
		vertex.position.y() = mesh->mVertices[i].y;
		vertex.position.z() = mesh->mVertices[i].z;
		vertex.normal.x() = mesh->mNormals[i].x;
		vertex.normal.y() = mesh->mNormals[i].y;
		vertex.normal.z() = mesh->mNormals[i].z;
		if (mesh->mTextureCoords[0]) {
			vertex.texcoords.s() = mesh->mTextureCoords[0]->x;
			vertex.texcoords.t() = mesh->mTextureCoords[0]->y;
		}
		vertices.emplace_back(std::move(vertex));
	}

	std::vector<uint> indices;
	for (size_t i = 0; i < mesh->mNumFaces; ++i) {		// 处理索引
		const aiFace &face = mesh->mFaces[i];
		std::copy(face.mIndices, face.mIndices + face.mNumIndices, std::back_insert_iterator(indices));
	}

	std::vector<Texture> textures;
	int index = mesh->mNumAnimMeshes;
	if (index >= 0) {
		aiMaterial *material = scene->mMaterials[index];
		std::vector<Texture> diffuse_map = load_material_texture(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuse_map.begin(), diffuse_map.end());
		std::vector<Texture> specular_map = load_material_texture(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specular_map.begin(), specular_map.end());
	}

	return Mesh{ std::move(vertices), std::move(indices), std::move(textures) };
}

void Model::process_node(aiNode *node, const aiScene *scene) {
	for (size_t i = 0; i < node->mNumMeshes; ++i) {
		int index = node->mMeshes[i];
		aiMesh *mesh = scene->mMeshes[index];
		meshs.push_back(process_mesh(mesh, scene));
	}
	for (size_t i = 0; i < node->mNumChildren; ++i)
		process_node(node->mChildren[i], scene);
}

std::vector<Texture> Model::load_material_texture(aiMaterial *mat, aiTextureType type, const std::string &name) {
	std::vector<Texture> textures;
	for (size_t i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString str;
		mat->GetTexture(type, int(i), &str);
		std::string path = directory + '/' + str.C_Str();
		Texture texture(name, path);
		textures.emplace_back(std::move(texture));
	}
	return textures;
}

Texture2d Model::load_texture(const std::string &path) {
	std::string file_path = directory = directory + path;
	if (auto iter = texture_cache.find(file_path); iter != texture_cache.end())
		return iter->second;
	
	Texture2d new_texture(file_path);
	texture_cache.insert(std::make_pair(file_path, new_texture));
	return new_texture;
}
