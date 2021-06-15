#pragma once

class SoftRenderer {
	Window						window;
	FrameBuffer					frame;
	std::shared_ptr<CameraBase> camera_ptr;
	std::shared_ptr<ShaderBase> shader_ptr;
	std::shared_ptr<Model>		model_ptr;
	int		width;
	int		height;
	float	last_time = 0.f;
	int		now_time = 0;
	int		fps = 0;
	float	far;
	float	near;
public:
	SoftRenderer(int _width, int _height, float _near, float _far, std::shared_ptr<CameraBase> _camera_ptr, 
				 std::shared_ptr<ShaderBase> _shader_ptr, std::shared_ptr<Model> _model_ptr);
	SoftRenderer(const SoftRenderer &) = delete;
	~SoftRenderer() = default;
public:
	std::function<void(Window::WindowKey)>	on_key_input;
public:
	void key_callback(Window::WindowKey key);
	void mouse_callback(int x, int y);
	void scroll_callback(float offset);
	void frame_callback(int width, int height);
	void poll_event();
	void process_input();
	bool is_input();
public:
	void test_cube(float near, float far);
	void light_renderer();
	void blinn_phong();
	void normal_mapping();
	void skybox();
	void test_plane();
	void car();
	void parallax_mapping();
};