#include "common.h"
#include <thread>

constexpr int width = 400;
constexpr int height = 400;
constexpr float aspect = static_cast<float>(width) / static_cast<float>(height);

void init();
void test_cube();
void light_render();
void blinn_phong();
void normal_mapping();
void one_triangle();
void skybox();

int main(void) {
	init();
	//light_render();
	//test_cube();
	//blinn_phong();
	//normal_mapping();
	//one_triangle();
	skybox();
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

#if 0
void test_cube() {
	constexpr float near = 0.1f;
	constexpr float far = 100.f;
	constexpr float fov = 45.f;
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FpsCamera>(vec3(-5, 0, 0), vec3(0, 1, 0), 45.f, aspect, near, far, 5.f, 0.1f);
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<TextrueShader>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Loader::create_test_cube_obj());
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.test_cube(near, far);
}
#endif

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
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Loader::load_obj("resources/obj/african_head.obj"));
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.light_renderer();
}

void blinn_phong() {
	constexpr float near = -0.001f;
	constexpr float far = -100.f;
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FpsCamera>(vec3(-2, 0, 0), vec3(0, 1, 0), 45.f, aspect, near, far, 1.f, 0.1f);
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<BlinnPhong>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Loader::load_obj("resources/obj/african_head.obj"));
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.blinn_phong();
}

#if 0
void normal_mapping() {
	constexpr float near = 0.1f;
	constexpr float far = 100.f;
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FpsCamera>(vec3(-2, 0, 0), vec3(0, -1, 0), 45.f, aspect, near, far, 1.f, 0.1f);
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<NormalMapping>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Loader::load_obj("resources/obj/african_head.obj"));
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.normal_mapping();
}
#endif
void one_triangle() {
	constexpr float near = -0.0001f;
	constexpr float far = -100.f;
#if 0
	vec3 look_from = vec3(-5, 0, 0);
	vec3 look_at = vec3(0.976895f, 0.0505929f, -0.207645f);
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FixedCamera>(look_from, look_from + look_at, vec3(0, 1, 0), 45.f, aspect, near, far);
#else
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FpsCamera>(vec3(-5, 0, 0), vec3(0, 1, 0), 45.f, aspect, near, far, 1.f, 0.1f);
#endif
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<LightShader>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Loader::cearte_one_triangle());
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.light_renderer();
}

void skybox() {
	constexpr float near = -0.0001f;
	constexpr float far = -100.f;
	std::shared_ptr<CameraBase> camera_ptr
		= std::make_shared<FpsCamera>(vec3(-5, 0, 0), vec3(0, 1, 0), 45.f, aspect, near, far, 5.f, 0.1f);
	std::shared_ptr<ShaderBase> shader_ptr = std::make_shared<TextrueShader>();
	std::shared_ptr<Model> model_ptr = std::make_shared<Model>(Loader::load_obj("resources/obj/african_head.obj"));
	SoftRenderer renderer(width, height, camera_ptr, shader_ptr, model_ptr);
	renderer.skybox();
}
