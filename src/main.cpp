#include "common.h"
#include <thread>

void init();

namespace gvar {
	constexpr int width = 800;
	constexpr int height = 600;
	constexpr int frame_size = width * height;
}


int main(void) {
	init();
	Model our_model = Model::load_obj("resources/obj/african_head.obj");
	FrameBuffer frame(gvar::width, gvar::height);
	frame.clear_color(vec3(0, 0, 0));
	frame.clear(FrameBufferType::ColorBuffer);

	LightShader light_shader;
	light_shader.set_uniform("light_dir", normalized(vec3(2, 2, 2)));
	light_shader.view = Draw::view(vec3(0, 0, 2), vec3(0, 1, 0), vec3(0));
	light_shader.viewport = Draw::viewport(gvar::width, gvar::height);
	our_model.draw(frame, light_shader);
	Window window(gvar::width, gvar::height, "test");
	while (!window.window_should_be_close()) {
		window.poll_event();
		window.draw(frame);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
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