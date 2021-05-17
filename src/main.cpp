#include "common.h"
#include <thread>

void init();

namespace gvar {

constexpr int width = 600;
constexpr int height = 600;
constexpr int frame_size = width * height;
constexpr float near = 0.01f;
constexpr float far = 1000.f;

}

int main(void) {
	init();
	Window window(gvar::width, gvar::height, "test");
	Model our_model = Model::load_obj("resources/obj/african_head.obj");
	FrameBuffer frame(gvar::width, gvar::height);


	LightShader light_shader;
	light_shader.set_uniform("light_dir", normalized(vec3(0, 0, 2)));
	light_shader.set_view(Draw::view(vec3(0, 0, 2), vec3(0, 1, 0), vec3(0)));
	light_shader.set_viewport(Draw::viewport(gvar::width, gvar::height));
	Texture2d diffuse_texture("resources/obj/african_head_diffuse.tga");
	light_shader.set_uniform("diffuse_texture", diffuse_texture);

	while (!window.window_should_be_close()) {
		window.poll_event();	
		frame.clear_color(vec3(0.2f, 0.5f, 0.2f));
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		our_model.draw(frame, light_shader);
		window.draw(frame);
	}
	std::system("pause");
	return 0;
}


void init_path();
void init() {
	init_path();
#ifdef _DEBUG
	Test::check();
#endif // _DEBUG
}

void init_path() {
	namespace fs = std::filesystem;
	constexpr std::string_view project_name("tiny_renderer");
	std::string cur_path = fs::current_path().string();
	std::string new_path = cur_path.substr(0, cur_path.find(project_name) + project_name.size());
	SetCurrentDirectory(new_path.c_str());
}