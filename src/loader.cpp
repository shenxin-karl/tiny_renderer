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

std::shared_ptr<ImageInfo> Loader::load_image(const std::string &path) {
	if (auto iter = image_cache.find(path); iter != image_cache.end())
		return iter->second;

	ImageInfo image_info;
	image_info.data = stbi_load(path.c_str(), &image_info.width, &image_info.height, &image_info.chnnell, 0);
	if (image_info.data == nullptr) {
		std::cerr << "Loader::load_image::load " << path << " error" << std::endl;
		return nullptr;
	}
	std::shared_ptr<ImageInfo> res = std::make_shared<ImageInfo>(image_info);
	image_cache.insert(std::make_pair(path, res));
	return res;
}

Loader::ImageCacheRecycle::~ImageCacheRecycle() {
	for (auto &&[_, image_info_ptr] : image_cache) {
		if (image_info_ptr != nullptr && image_info_ptr->data != nullptr)
			stbi_image_free(image_info_ptr->data);
	}
}
