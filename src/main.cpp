#include "common.h"
#include <thread>

namespace gvar {
	constexpr int width = 800;
	constexpr int height = 600;
	constexpr int frame_size = width * height;
}


int main(void) {
	FrameBuffer frame(gvar::width, gvar::height);
	Window window(gvar::width, gvar::height, "test");

	frame.clear_color(vec3(1.0, 1.0, 0.0));
	frame.clear(FrameBufferType::ColorBuffer);
	while (!window.window_should_be_close()) {
		window.poll_event();
		window.draw(frame);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
	std::cout << "hello world" << std::endl;
	std::system("pause");
	return 0;
}