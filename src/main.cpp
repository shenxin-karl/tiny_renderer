#include "common.h"
#include <thread>

constexpr int width = 600;
constexpr int height = 600;
constexpr float aspect = static_cast<float>(width) / static_cast<float>(height);

void init();
void test_cube();
void light_render();
void blinn_phong();
void normal_mapping();

int main(void) {
	init();
	//light_render();
	//test_cube();
	blinn_phong();
	//normal_mapping();
	return 0;
}

void init_path() {
	namespace fs = std::filesystem;
	constexpr std::string_view project_name("tiny_renderer");
	std::string cur_path = fs::current_path().string();
	auto npos = cur_path.find(project_name);
	if (npos == std::string::npos)
		return;

	std::string new_path = cur_path.substr(0, npos + project_name.size());
	SetCurrentDirectory(new_path.c_str());
}

void init() {
	init_path();
#ifdef _DEBUG
	Test::check_depth();
#endif // _DEBUG
}

void test_cube() {
	constexpr float near = 0.1f;
	constexpr float far = 10.f;
	constexpr float fov = 15.f;
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FpsCamera>(vec3(2, 0, 0), vec3(0, 1, 0), 45.f, aspect, near, far, 1.f, 0.1f);
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<DepthShader>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Model::create_test_cube_obj());
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.test_cube(near, far);
}

void light_render() {
	constexpr float near = -0.1f;
	constexpr float far = -100.f;
#if 1
	std::shared_ptr<CameraBase> camera_ptr 
		= std::make_shared<FpsCamera>(vec3(-5, 0, 0), vec3(0, 1, 0), 45.f, aspect, near, far, 1.f, 0.1f);
#else
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FixedCamera>(vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0), 45.f, aspect, near, far);
#endif
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<LightShader>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Model::load_obj("resources/obj/african_head.obj"));
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.light_renderer();
}

void blinn_phong() {
	constexpr float near = -0.1f;
	constexpr float far = -100.f;
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FpsCamera>(vec3(-2, 0, 0), vec3(0, 1, 0), 45.f, aspect, near, far, 1.f, 0.1f);
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<BlinnPhong>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Model::load_obj("resources/obj/african_head.obj"));
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.blinn_phong();
}

void normal_mapping() {
	constexpr float near = 0.1f;
	constexpr float far = 100.f;
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FpsCamera>(vec3(-2, 0, 0), vec3(0, -1, 0), 45.f, aspect, near, far, 1.f, 0.1f);
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<NormalMapping>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Model::load_obj("resources/obj/african_head.obj"));
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.normal_mapping();
}