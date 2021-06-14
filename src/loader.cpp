#include "common.h"

Model Loader::load_obj(const std::string &path) {
	std::ifstream file(path, std::ios::in);
	if (!file.is_open())
		return {};

	std::vector<Vertex> vertices;
	std::vector<int> indices;
	std::vector<vec4> all_vertices;
	std::vector<vec3> all_normal;
	std::vector<vec2> all_texcoords;

	std::string line;
	while (!file.eof()) {
		std::getline(file, line);
		std::stringstream isbuf(line);
		char c_trash;
		if (line.compare(0, 2, "v ") == 0) {				// 加载顶点
			vec4 vertex;
			isbuf >> c_trash >> vertex;
			vertex.w() = 1.0f;
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

				vertices.push_back(Vertex{
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
	generate_tangent(vertices, indices);
	Mesh mesh(std::move(vertices), std::move(indices), {});
	Model res;
	res.meshs.push_back(std::move(mesh));
	res.directory = path.substr(0, path.find_last_of('/') + 1);
	return res;
}

Model Loader::create_test_cube_obj() {
	std::vector<Vertex> vertices = {
		Vertex{ vec4{-0.5f, -0.5f, -0.5f, 1.0f}, vec3{0.0f,  0.0f, -1.0f},  vec2{ 0.0f, 0.0f},},
		Vertex{ vec4{ 0.5f, -0.5f, -0.5f, 1.0f}, vec3{0.0f,  0.0f, -1.0f},  vec2{ 1.0f, 0.0f},},
		Vertex{ vec4{ 0.5f,  0.5f, -0.5f, 1.0f}, vec3{0.0f,  0.0f, -1.0f},  vec2{ 1.0f, 1.0f},},
		Vertex{ vec4{ 0.5f,  0.5f, -0.5f, 1.0f}, vec3{0.0f,  0.0f, -1.0f},  vec2{ 1.0f, 1.0f},},
		Vertex{ vec4{-0.5f,  0.5f, -0.5f, 1.0f}, vec3{0.0f,  0.0f, -1.0f},  vec2{ 0.0f, 1.0f},},
		Vertex{ vec4{-0.5f, -0.5f, -0.5f, 1.0f}, vec3{0.0f,  0.0f, -1.0f},  vec2{ 0.0f, 0.0f},},
		Vertex{ vec4{-0.5f, -0.5f,  0.5f, 1.0f}, vec3{0.0f,  0.0f, 1.0f},   vec2{0.0f, 0.0f},},
		Vertex{ vec4{ 0.5f, -0.5f,  0.5f, 1.0f}, vec3{0.0f,  0.0f, 1.0f},   vec2{1.0f, 0.0f},},
		Vertex{ vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, vec3{0.0f,  0.0f, 1.0f},   vec2{1.0f, 1.0f},},
		Vertex{ vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, vec3{0.0f,  0.0f, 1.0f},   vec2{1.0f, 1.0f},},
		Vertex{ vec4{-0.5f,  0.5f,  0.5f, 1.0f}, vec3{0.0f,  0.0f, 1.0f},   vec2{0.0f, 1.0f},},
		Vertex{ vec4{-0.5f, -0.5f,  0.5f, 1.0f}, vec3{0.0f,  0.0f, 1.0f},   vec2{0.0f, 0.0f},},
		Vertex{ vec4{-0.5f,  0.5f,  0.5f, 1.0f}, vec3{-1.0f,  0.0f,  0.0f}, vec2{1.0f, 0.0f},},
		Vertex{ vec4{-0.5f,  0.5f, -0.5f, 1.0f}, vec3{-1.0f,  0.0f,  0.0f}, vec2{1.0f, 1.0f},},
		Vertex{ vec4{-0.5f, -0.5f, -0.5f, 1.0f}, vec3{-1.0f,  0.0f,  0.0f}, vec2{0.0f, 1.0f},},
		Vertex{ vec4{-0.5f, -0.5f, -0.5f, 1.0f}, vec3{-1.0f,  0.0f,  0.0f}, vec2{0.0f, 1.0f},},
		Vertex{ vec4{-0.5f, -0.5f,  0.5f, 1.0f}, vec3{-1.0f,  0.0f,  0.0f}, vec2{0.0f, 0.0f},},
		Vertex{ vec4{-0.5f,  0.5f,  0.5f, 1.0f}, vec3{-1.0f,  0.0f,  0.0f}, vec2{1.0f, 0.0f},},
		Vertex{ vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, vec3{ 1.0f,  0.0f,  0.0f}, vec2{1.0f, 0.0f},},
		Vertex{ vec4{ 0.5f,  0.5f, -0.5f, 1.0f}, vec3{ 1.0f,  0.0f,  0.0f}, vec2{1.0f, 1.0f},},
		Vertex{ vec4{ 0.5f, -0.5f, -0.5f, 1.0f}, vec3{ 1.0f,  0.0f,  0.0f}, vec2{0.0f, 1.0f},},
		Vertex{ vec4{ 0.5f, -0.5f, -0.5f, 1.0f}, vec3{ 1.0f,  0.0f,  0.0f}, vec2{0.0f, 1.0f},},
		Vertex{ vec4{ 0.5f, -0.5f,  0.5f, 1.0f}, vec3{ 1.0f,  0.0f,  0.0f}, vec2{0.0f, 0.0f},},
		Vertex{ vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, vec3{ 1.0f,  0.0f,  0.0f}, vec2{1.0f, 0.0f},},
		Vertex{ vec4{-0.5f, -0.5f, -0.5f, 1.0f}, vec3{ 0.0f, -1.0f,  0.0f}, vec2{0.0f, 1.0f},},
		Vertex{ vec4{ 0.5f, -0.5f, -0.5f, 1.0f}, vec3{ 0.0f, -1.0f,  0.0f}, vec2{1.0f, 1.0f},},
		Vertex{ vec4{ 0.5f, -0.5f,  0.5f, 1.0f}, vec3{ 0.0f, -1.0f,  0.0f}, vec2{1.0f, 0.0f},},
		Vertex{ vec4{ 0.5f, -0.5f,  0.5f, 1.0f}, vec3{ 0.0f, -1.0f,  0.0f}, vec2{1.0f, 0.0f},},
		Vertex{ vec4{-0.5f, -0.5f,  0.5f, 1.0f}, vec3{ 0.0f, -1.0f,  0.0f}, vec2{0.0f, 0.0f},},
		Vertex{ vec4{-0.5f, -0.5f, -0.5f, 1.0f}, vec3{ 0.0f, -1.0f,  0.0f}, vec2{0.0f, 1.0f},},
		Vertex{ vec4{-0.5f,  0.5f, -0.5f, 1.0f}, vec3{ 0.0f,  1.0f,  0.0f}, vec2{0.0f, 1.0f},},
		Vertex{ vec4{ 0.5f,  0.5f, -0.5f, 1.0f}, vec3{ 0.0f,  1.0f,  0.0f}, vec2{1.0f, 1.0f},},
		Vertex{ vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, vec3{ 0.0f,  1.0f,  0.0f}, vec2{1.0f, 0.0f},},
		Vertex{ vec4{ 0.5f,  0.5f,  0.5f, 1.0f}, vec3{ 0.0f,  1.0f,  0.0f}, vec2{1.0f, 0.0f},},
		Vertex{ vec4{-0.5f,  0.5f,  0.5f, 1.0f}, vec3{ 0.0f,  1.0f,  0.0f}, vec2{0.0f, 0.0f},},
		Vertex{ vec4{-0.5f,  0.5f, -0.5f, 1.0f}, vec3{ 0.0f,  1.0f,  0.0f}, vec2{0.0f, 1.0f},},
	};

	std::vector<int> indices;
	indices.reserve(vertices.size());
	std::generate_n(std::back_inserter(indices), vertices.size(), [n = 0]() mutable {
		return n++;
	});



	Mesh mesh(std::move(vertices), std::move(indices), {});
	Model res;
	res.directory = "test_cube";
	res.meshs.push_back(std::move(mesh));
	return res;
}

Model Loader::cearte_one_triangle() {
	std::vector<Vertex> vertices = {
		Vertex{ vec4{ 0, 0,  2, 1 }, vec3{ -1, 0, 0 }, vec2{ 0, 0    } },
		Vertex{ vec4{ 0, 0, -2, 1 }, vec3{ -1, 0, 0 }, vec2{ 1, 0    } },
		Vertex{ vec4{ 0, 2,  0, 1 }, vec3{ -1, 0, 0 }, vec2{ 0.5f, 1 } },
	};
	std::vector<int> indices;
	indices.reserve(vertices.size());
	std::generate_n(std::back_inserter(indices), vertices.size(), [n = 0]() mutable {
		return n++;
	});
	Mesh mesh(std::move(vertices), std::move(indices), {});
	Model res;
	res.directory = "one_triangle";
	res.meshs.push_back(std::move(mesh));
	return res;
}


Model Loader::create_skybox_obj() {
	std::vector<Vertex> vertices = {
		Vertex{ vec4{-1.0f,  1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f, -1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f, -1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f, -1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f,  1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f,  1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f, -1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f, -1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f,  1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f,  1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f,  1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f, -1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f, -1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f, -1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f,  1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f,  1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f,  1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f, -1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f, -1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f,  1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f,  1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f,  1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f, -1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f, -1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f,  1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f,  1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f,  1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f,  1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f,  1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f,  1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f, -1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f, -1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f, -1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f, -1.0f, -1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{-1.0f, -1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
		Vertex{ vec4{ 1.0f, -1.0f,  1.0f, 1.f}, vec3{0.f}, vec2{0.f}, },
	};
	std::vector<int> indices;
	indices.reserve(vertices.size());
	std::generate_n(std::back_inserter(indices), vertices.size(), [n = 0]() mutable {
		return n++;
	});
	Mesh mesh(std::move(vertices), std::move(indices), {});
	Model res;
	res.directory = "create_skybox_obj";
	res.meshs.push_back(std::move(mesh));
	return res;
}

Model Loader::create_test_plane() {
	vec3 pos1(-1.0, 1.0, 0.0);		// 左上
	vec3 pos2(-1.0, -1.0, 0.0);		// 左下
	vec3 pos3(1.0, -1.0, 0.0);		// 右下
	vec3 pos4(1.0, 1.0, 0.0);		// 右上

	vec2 uv1(0.0, 1.0);
	vec2 uv2(0.0, 0.0);
	vec2 uv3(1.0, 0.0);
	vec2 uv4(1.0, 1.0);
	vec3 nm(0.0, 0.0, 1.0);

	std::vector<Vertex> vertices = {
		Vertex { vec4(pos1, 1.f), nm, uv1 },
		Vertex { vec4(pos2, 1.f), nm, uv2 },
		Vertex { vec4(pos3, 1.f), nm, uv3 },
		Vertex { vec4(pos4, 1.f), nm, uv4 },
	};

	auto calc_tangent = [](const Vertex &v1, const Vertex &v2, const Vertex &v3) -> std::tuple<vec3, vec3> {
		vec3 edge1 = v2.position.head<3>() - v1.position.head<3>();
		vec3 edge2 = v3.position.head<3>() - v1.position.head<3>();
		vec2 deltaUV1 = v2.texcoords - v1.texcoords;
		vec2 deltaUV2 = v3.texcoords - v1.texcoords;
		float f = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV2.x() * deltaUV1.y());
		vec3 tangent = {
			f * (deltaUV2.y() * edge1.x() - deltaUV1.y() * edge2.x()),
			f * (deltaUV2.y() * edge1.y() - deltaUV1.y() * edge2.y()),
			f * (deltaUV2.y() * edge1.z() - deltaUV1.y() * edge2.z()),
		};
		tangent = normalized(tangent);
		vec3 bitangent = {
			f * (-deltaUV2.x() * edge1.x() + deltaUV1.x() * edge2.x()),
			f * (-deltaUV2.x() * edge1.y() + deltaUV1.x() * edge2.y()),
			f * (-deltaUV2.x() * edge1.z() + deltaUV1.x() * edge2.z()),
		};
		bitangent = normalized(bitangent);
		return { tangent, bitangent };
	};

	std::array<int, 3> indices1 = { 0, 1, 2 };
	auto &&[tangent1, bitangent1] = calc_tangent(vertices[0], vertices[1], vertices[2]);
	for (int i = 0; i < 3; ++i) {
		vertices[i].tangent = tangent1;
		vertices[i].bitangent = bitangent1;
	}

	std::array<int, 3> indices2 = { 0, 2, 3 };
	auto &&[tangent2, bitangent2] = calc_tangent(vertices[0], vertices[2], vertices[3]);
	for (int i : indices2) {
		vertices[i].tangent = tangent2;
		vertices[i].bitangent = bitangent2;
	}
	std::vector<int> indices;
	indices.insert(indices.end(), indices1.begin(), indices1.end());
	indices.insert(indices.end(), indices2.begin(), indices2.end());
	generate_tangent(vertices, indices);
#ifdef _DEBUG
	for (const auto &v : vertices) 
		std::cout << "tangent: " << v.tangent << "\t bitangent " << v.bitangent << std::endl;
#endif // _DEBUG

	Mesh mesh(std::move(vertices), std::move(indices), {});
	Model res;
	res.directory = "create_test_plane";
	res.meshs.push_back(std::move(mesh));
	return res;
}

std::shared_ptr<ImageInfo> Loader::load_image(const std::string &path) {
	if (auto iter = image_cache.find(path); iter != image_cache.end())
		return iter->second;

	ImageInfo image_info;
	image_info.data = stbi_load(path.c_str(), &image_info.width, &image_info.height, &image_info.channel, 0);
	if (image_info.data == nullptr) {
		std::cerr << "Loader::load_image::load " << path << " error" << std::endl;
		return nullptr;
	}
	std::shared_ptr<ImageInfo> res = std::make_shared<ImageInfo>(image_info);
	image_cache.insert(std::make_pair(path, res));
	return res;
}


void Loader::generate_tangent(std::vector<Vertex> &vertices, const std::vector<int> &indices) {
	if (indices.size() < 3ul)
		return;

	std::vector<vec3> temp_tagent(vertices.size(), vec3());
	size_t limit = static_cast<int>(indices.size()) - 2;
	for (size_t i = 0; i < limit; i += 3) {
		const Vertex &v1 = vertices[indices[i + 0]];
		const Vertex &v2 = vertices[indices[i + 1]];
		const Vertex &v3 = vertices[indices[i + 2]];
		vec3 E1 = v2.position.head<3>() - v1.position.head<3>();
		vec3 E2 = v3.position.head<3>() - v1.position.head<3>();
		float t1 = v2.texcoords.t() - v1.texcoords.t();
		float t2 = v3.texcoords.t() - v1.texcoords.t();
		vec3 tangent = t2 * E1 - t1 * E2;
		for (int j = 0; j < 3; ++j)
			temp_tagent[indices[i + j]] += tangent;
	}

	for (size_t i = 0; i < vertices.size(); ++i) {
		Vertex &v = vertices[i];
		vec3 t = temp_tagent[i];
		t -= v.normal * dot(t, v.normal);
		t.normalize();
		v.tangent = t;
		v.bitangent = cross(v.normal, t);
	}
}

Loader::ImageCacheRecycle::~ImageCacheRecycle() {
	for (auto &&[_, image_info_ptr] : image_cache) {
		if (image_info_ptr != nullptr && image_info_ptr->data != nullptr)
			stbi_image_free(image_info_ptr->data);
	}
}
