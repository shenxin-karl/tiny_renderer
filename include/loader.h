#pragma once

struct ImageInfo {
	unsigned char *data;
	int width;
	int height;
	int channel;
};

class Loader {
public:
	static Model load_obj(const std::string &path);
	static Model create_test_cube_obj();
	static Model cearte_one_triangle();
	static Model create_skybox_obj();
	static Model create_test_plane();
	static std::shared_ptr<ImageInfo> load_image(const std::string &path);
	static void generate_tangent(std::vector<Vertex> &vertices, const std::vector<int> &incides);
private:
	struct ImageCacheRecycle {
		~ImageCacheRecycle();
	};
	inline static std::unordered_map<std::string, std::shared_ptr<ImageInfo>> image_cache;
	inline static ImageCacheRecycle image_cache_recycle;
};