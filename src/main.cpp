#include "common.h"
#include <thread>

namespace gvar {
	constexpr int width = 800;
	constexpr int height = 600;
	constexpr int frame_size = width * height;
}

void init_path();

int main(void) {
	init_path();
	Model our_model = Model::load_obj("resources/obj/african_head.obj");
	FrameBuffer frame(gvar::width, gvar::height);
	frame.clear_color(vec3(0, 0, 0));
	frame.clear(FrameBufferType::ColorBuffer);

	Window window(gvar::width, gvar::height, "test");
	while (!window.window_should_be_close()) {
		window.poll_event();
		window.draw(frame);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	std::system("pause");
	return 0;
}

void init_path() {
	namespace fs = std::filesystem;
	constexpr std::string_view project_name("tiny_renderer");
	std::string cur_path = fs::current_path().string();
	std::string new_path = cur_path.substr(0, cur_path.find(project_name) + project_name.size());
	SetCurrentDirectory(new_path.c_str());
}