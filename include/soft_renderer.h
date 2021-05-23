#pragma once

class SoftRenderer {
	Window						window;
	FrameBuffer					frame;
	std::shared_ptr<CameraBase> camera_ptr;
	std::shared_ptr<ShaderBase> shader_ptr;
	std::shared_ptr<Model>		model_ptr;
	int width;
	int height;
	float last_time = 0.f;
public:
	SoftRenderer(int _width, int _height, std::shared_ptr<CameraBase> _camera_ptr, 
				 std::shared_ptr<ShaderBase> _shader_ptr, std::shared_ptr<Model> _model_ptr);
	SoftRenderer(const SoftRenderer &) = delete;
	~SoftRenderer() = default;
public:
	void key_callback(Window::WindowKey key);
	void mouse_callback(int x, int y);
	void scroll_callback(float offset);
	void frame_callback(int width, int height);
	void poll_event();
public:
	void test_cube();
	void light_renderer();
	void blinn_phong();
};