#include "common.h"

SoftRenderer::SoftRenderer(int _width, int _height, 
						  std::shared_ptr<CameraBase> _camera_ptr, 
						  std::shared_ptr<ShaderBase> _shader_ptr, 
						  std::shared_ptr<Model> _model_ptr) 
: window(_width, _height, "MySoftRenderer"), frame(_width, _height)
, camera_ptr(_camera_ptr), shader_ptr(_shader_ptr), model_ptr(_model_ptr) {

	assert(_width > 0);
	assert(_height > 0);
	assert(_camera_ptr != nullptr);
	assert(_shader_ptr != nullptr);
	assert(_model_ptr != nullptr);

	window.key_callback = [this](Window *, Window::WindowKey key) {
		this->key_callback(key);
	};
	window.mouse_callback = [this](Window *, int x, int y) {
		this->mouse_callback(x, y);
	};
	window.scroll_callback = [this](Window *, float offset) {
		this->scroll_callback(offset);
	};
	window.frame_callback = [this](Window *, int width, int height) {
		this->frame_callback(width, height);
	};
}

void SoftRenderer::key_callback(Window::WindowKey key) {
	auto curr_time = window.get_time();
	auto delta_time = curr_time - last_time;
	last_time = curr_time;
	camera_ptr->key_callback(key, delta_time);
}

void SoftRenderer::mouse_callback(int x, int y) {
	camera_ptr->mouse_callback(x, y);
}

void SoftRenderer::scroll_callback(float offset) {
	camera_ptr->scroll_callback(offset);
}

void SoftRenderer::frame_callback(int width, int height) {
	if (width <= 0 || height <= 0)
		return;
	frame.resize(width, height);
	camera_ptr->frame_callback(width, height);
}

void SoftRenderer::normal_renderer() {

}
