#include "common.h"

SoftRenderer::SoftRenderer(int _width, int _height, 
						  std::shared_ptr<CameraBase> _camera_ptr, 
						  std::shared_ptr<ShaderBase> _shader_ptr, 
						  std::shared_ptr<Model> _model_ptr) 
: window(_width, _height, "MySoftRenderer"), frame(_width, _height)
, camera_ptr(_camera_ptr), shader_ptr(_shader_ptr), model_ptr(_model_ptr)
, width(_width), height(_height) {
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
	camera_ptr->key_callback(key, delta_time);
}

void SoftRenderer::mouse_callback(int x, int y) {
	camera_ptr->mouse_callback(x, y);
}

void SoftRenderer::scroll_callback(float offset) {
	camera_ptr->scroll_callback(offset);
}

void SoftRenderer::frame_callback(int _width, int _height) {
	if (_width <= 0 || _height <= 0)
		return;

	width = _width;
	height = _height;
	frame.resize(_width, _height);
	camera_ptr->frame_callback(_width, _height);
}

void SoftRenderer::poll_event() {
	window.poll_event();
	last_time = window.get_time();
	process_input();

	fps++;
	if (last_time > now_time) {
		std::cout << "fps: " << fps << std::endl;
		now_time = static_cast<int>(last_time) + 1;
		fps = 0;
	}
}

void SoftRenderer::process_input() {
	while (is_input()) {
		std::string line;
		std::getline(std::cin, line);
		std::stringstream sbuf(line);
		if (line.compare("save") == 0) {
			std::string file_name;
			long type;
			sbuf >> file_name >> type;
			frame.save(file_name, static_cast<FrameBufferType>(type));
		}
	}
}

bool SoftRenderer::is_input() {
	return _kbhit();
}

void SoftRenderer::test_cube() {
	Texture2d diffuse_texture("resources/test_cube/container2.png");
	shader_ptr->set_uniform("diffuse_texture", diffuse_texture);
	while (!window.window_should_be_close()) {
		frame.clear_color(vec3(0.1f, 0.3f, 0.1f));
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		shader_ptr->set_viewport(Draw::viewport(width, height));
		shader_ptr->set_view(camera_ptr->get_view());
		shader_ptr->set_projection(camera_ptr->get_projection());
		model_ptr->draw(frame, *shader_ptr);
		window.draw(frame);
		poll_event();
	}
	return;
}

void SoftRenderer::light_renderer() {
	Texture2d diffuse_texture("resources/obj/african_head_diffuse.tga");
	shader_ptr->set_uniform("diffuse_texture", diffuse_texture);
	shader_ptr->set_uniform("light_dir", normalized(vec3(0, 0, 2)));
	shader_ptr->set_face_culling_func([](float cosine) { return cosine > 0.0f; });

	while (!window.window_should_be_close()) {
		frame.clear_color(vec3(0.1f, 0.3f, 0.1f));
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		shader_ptr->set_viewport(Draw::viewport(width, height));
		shader_ptr->set_view(camera_ptr->get_view());
		shader_ptr->set_projection(camera_ptr->get_projection());
		model_ptr->draw(frame, *shader_ptr);
		window.draw(frame);
		poll_event();
	}
	return;
}

void SoftRenderer::blinn_phong() {
	vec3 light_dir = normalized(vec3(0, 2, -2));
	Texture2d diffuse_texture("resources/obj/african_head_diffuse.tga");
	shader_ptr->set_uniform("diffuse_texture", diffuse_texture);
	shader_ptr->set_uniform("specular_factor", 32.f);
	shader_ptr->set_uniform("light_ambient", vec3(0.4f, 0.4f, 0.4f));
	shader_ptr->set_uniform("light_diffuse", vec3(0.8f, 0.8f, 0.8f));
	shader_ptr->set_uniform("light_specular", vec3(2.f, 2.f, 2.f));
	shader_ptr->set_face_culling_func([](float cosine) { return cosine > 0.0f; });

	while (!window.window_should_be_close()) {
		mat4 light_rotate_mat = Draw::rotate_y(window.get_time() * 30);
		vec3 new_light_dir = normalized(light_rotate_mat * vec4(light_dir, 1.f));
		shader_ptr->set_uniform("light_dir", new_light_dir);

		frame.clear_color(vec3(0.f));
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		shader_ptr->set_viewport(Draw::viewport(width, height));
		shader_ptr->set_view(camera_ptr->get_view());
		shader_ptr->set_projection(camera_ptr->get_projection());
		shader_ptr->set_uniform("eye_pos", camera_ptr->get_look_from());
		model_ptr->draw(frame, *shader_ptr);
		window.draw(frame);
		poll_event();
	}
}
