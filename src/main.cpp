#include "common.h"
#include <thread>

void init_path() {
	namespace fs = std::filesystem;
	constexpr std::string_view project_name("tiny_renderer");
	std::string cur_path = fs::current_path().string();
	std::string new_path = cur_path.substr(0, cur_path.find(project_name) + project_name.size());
	SetCurrentDirectory(new_path.c_str());
}

void init_path();
void init() {
	init_path();
#ifdef _DEBUG
	Test::check();
#endif // _DEBUG
}


void test_cube() {
	constexpr int width = 600;
	constexpr int height = 600;
	constexpr float aspect = static_cast<float>(width) / static_cast<float>(height);
	constexpr float near = 0.1f;
	constexpr float far = 50.f;
	constexpr float fov = 15.f;
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FixedCamera>(vec3(0, 0, 2), vec3(0, 0, -1), vec3(0, 1, 0), fov, aspect, near, far);
	//std::shared_ptr<CameraBase> camera_ptr
		//= std::make_shared<FpsCamera>(vec3(0, 0, 3), vec3(0, 1, 0), 45.f, aspect, near, far, 1.f, 0.1f);
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<LightShader>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Model::create_test_cube_obj());
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.test_cube();
}

void normal_renderer() {
	constexpr int width = 600;
	constexpr int height = 600;
	constexpr float aspect = static_cast<float>(width) / static_cast<float>(height);
	constexpr float near = 0.001f;
	constexpr float far = 100.f;
	std::shared_ptr<CameraBase> camera_ptr 
		= std::make_shared<FpsCamera>(vec3(-2, 0, 0), vec3(0, 1, 0), 45.f, aspect, near, far, 100.f, 0.1f);
	std::shared_ptr<LightShader> shader_ptr = std::make_shared<LightShader>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Model::load_obj("resources/obj/african_head.obj"));
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.normal_renderer();
}


int main(void) {
	init();
	//normal_renderer();
	test_cube();
	return 0;
}