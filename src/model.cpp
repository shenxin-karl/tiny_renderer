#include "common.h"
#include "stb_image.h"

Model::Model(const std::string &path) {
	Assimp::Importer importer;
	auto flag = aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace;
	const aiScene *scene = importer.ReadFile(path, flag);
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		std::cerr << "ERROR:ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	process_node(scene->mRootNode, scene);
}

void Model::draw(FrameBuffer &frame, ShaderBase &shader) {
	for (Mesh &mesh : meshs)
		mesh.draw(frame, shader);
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene) {
	std::vector<Vertex>	vertices;
	for (size_t i = 0; i < mesh->mNumVertices; ++i) {		// 加载顶点
		Vertex vertex;
		vertex.position.x() = mesh->mVertices[i].x;
		vertex.position.y() = mesh->mVertices[i].y;
		vertex.position.z() = mesh->mVertices[i].z;
		vertex.position.w() = 1.f;
		vertex.normal.x() = mesh->mNormals[i].x;
		vertex.normal.y() = mesh->mNormals[i].y;
		vertex.normal.z() = mesh->mNormals[i].z;
		vertex.tangent.x() = mesh->mTangents[i].x;
		vertex.tangent.y() = mesh->mTangents[i].y;
		vertex.tangent.z() = mesh->mTangents[i].z;
		vertex.bitangent.x() = mesh->mBitangents[i].x;
		vertex.bitangent.y() = mesh->mBitangents[i].y;
		vertex.bitangent.z() = mesh->mBitangents[i].z;
		if (mesh->mTextureCoords[0]) {
			vertex.texcoords.s() = mesh->mTextureCoords[0][i].x;
			vertex.texcoords.t() = mesh->mTextureCoords[0][i].y;
		}
		vertices.emplace_back(std::move(vertex));
	}

	std::vector<int> indices;
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
		Texture texture = { name, load_texture(path) };
		textures.emplace_back(std::move(texture));
	}
	return textures;
}

Texture2d Model::load_texture(const std::string &path) {
	if (auto iter = texture_cache.find(path); iter != texture_cache.end())
		return iter->second;
	
	Texture2d new_texture(path);
	texture_cache.insert(std::make_pair(path, new_texture));
	return new_texture;
}
