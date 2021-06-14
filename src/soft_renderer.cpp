#include "common.h"

SoftRenderer::SoftRenderer(int _width, int _height, float _near, float _far,
						  std::shared_ptr<CameraBase> _camera_ptr, 
						  std::shared_ptr<ShaderBase> _shader_ptr, 
						  std::shared_ptr<Model> _model_ptr) 
: window(_width, _height, "MySoftRenderer"), frame(_width, _height)
, camera_ptr(_camera_ptr), shader_ptr(_shader_ptr), model_ptr(_model_ptr)
, width(_width), height(_height) 
{
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
	if (on_key_input != nullptr)
		on_key_input(key);
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

void SoftRenderer::test_cube(float near, float far) {
	shader_ptr->set_uniform("near", near);
	shader_ptr->set_uniform("far", far);
	Texture2d texture("resources/test_cube/container2.png");
	shader_ptr->set_uniform("texture", texture);
	
	while (!window.window_should_be_close()) {
		//frame.clear_color(vec3(0.1f, 0.3f, 0.1f));
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		shader_ptr->set_model(Draw::rotate_y(window.get_time() * 30.f));
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
	//shader_ptr->set_uniform("light_dir", normalized(vec3(0, 0, -2)));
	shader_ptr->set_face_culling_func([](float cosine) { return cosine > 0.0f; });

	vec3 light_dir = normalized(vec3(0, 0, -2));
	frame.clear_color(vec3(0.1f, 0.2f, 0.3f));
	while (!window.window_should_be_close()) {
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		mat4 light_rotate_matrix = Draw::rotate_y(window.get_time() * 10.f);
		vec3 new_light_dir = vec3(light_rotate_matrix * vec4(light_dir, 1.f));

		shader_ptr->set_uniform("light_dir", new_light_dir);
		shader_ptr->set_view(camera_ptr->get_view());
		shader_ptr->set_projection(camera_ptr->get_projection());
		model_ptr->draw(frame, *shader_ptr);
		window.draw(frame);
		poll_event();
	}
	return;
}

void SoftRenderer::blinn_phong() {
	vec3 light_dir = normalized(vec3(0, 1.5, -2));
	Texture2d diffuse_texture("resources/obj/african_head_diffuse.tga");
	shader_ptr->set_uniform("diffuse_texture", diffuse_texture);
	shader_ptr->set_uniform("specular_factor", 32.f);
	shader_ptr->set_uniform("light_ambient", vec3(0.4f, 0.4f, 0.4f));
	shader_ptr->set_uniform("light_diffuse", vec3(1.f, 1.f, 1.f));
	shader_ptr->set_uniform("light_specular", vec3(1.f, 1.f, 1.f));
	shader_ptr->set_face_culling_func([](float cosine) { return cosine > 0.0f; });

	frame.clear_color(vec3(0.1f, 0.1f, 0.15f));
	while (!window.window_should_be_close()) {
		mat4 light_rotate_mat = Draw::rotate_y(window.get_time() * 77.f);
		vec3 new_light_dir = normalized(vec3(light_rotate_mat * vec4(light_dir, 1.f)));
		shader_ptr->set_uniform("light_dir", new_light_dir);

		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		shader_ptr->set_model(Draw::rotate_y(window.get_time() * 30.f));
		shader_ptr->set_view(camera_ptr->get_view());
		shader_ptr->set_projection(camera_ptr->get_projection());
		shader_ptr->set_uniform("eye_pos", camera_ptr->get_look_from());
		model_ptr->draw(frame, *shader_ptr);
		window.draw(frame);
		poll_event();
	}
}

void SoftRenderer::normal_mapping() {
	Texture2d normal_texture("resources/obj/african_head_nm_tangent.png");
	Texture2d diffuse_texture("resources/obj/african_head_diffuse.tga");
	shader_ptr->set_uniform("normal_texture", normal_texture);
	shader_ptr->set_uniform("diffuse_texture", diffuse_texture);
	shader_ptr->set_uniform("specular_factor", 64.f);
	shader_ptr->set_uniform("light_ambient", vec3(0.3f, 0.3f, 0.3f));
	shader_ptr->set_uniform("light_diffuse", vec3(0.9f, 0.9f, 0.9f));
	shader_ptr->set_uniform("light_specular", vec3(0.3f, 0.3f, 0.3f));
	shader_ptr->set_uniform("light_dir", normalized(vec3(-2, 2, 2)));
	shader_ptr->set_face_culling_func([](float cosine) { return cosine >= 0.f; });
	
	while (!window.window_should_be_close()) {
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		shader_ptr->set_model(Draw::rotate_y(window.get_time() * 10.f));
		shader_ptr->set_view(camera_ptr->get_view());
		shader_ptr->set_projection(camera_ptr->get_projection());
		shader_ptr->set_uniform("eye_pos", camera_ptr->get_look_from());
		model_ptr->draw(frame, *shader_ptr);
		window.draw(frame);
		poll_event();
	}
}

void SoftRenderer::skybox() {
	std::shared_ptr<ShaderBase> skybox_shader_ptr = std::make_shared<SkyboxShader>();
	std::shared_ptr<Model> skybox_model = std::make_shared<Model>(Loader::create_skybox_obj());

	auto backface_func = [](float conse) { return conse > 0.f; };
	shader_ptr->set_face_culling_func(backface_func);
	Texture2d diffuse_texture("resources/obj/african_head_diffuse.tga");
	shader_ptr->set_uniform("texture", diffuse_texture);
	
	std::array<std::string, 6> faces = {
		"resources/skybox/right.jpg",
		"resources/skybox/left.jpg",
		"resources/skybox/top.jpg",
		"resources/skybox/bottom.jpg",
		"resources/skybox/front.jpg",
		"resources/skybox/back.jpg",
	};
	TextureCube skybox_texture(faces);
	
	int type = 1;
	on_key_input = [&](Window::WindowKey key) {
		if (key == Window::WindowKey::Space)
			type = (type + 1) % 2;
	};

	std::shared_ptr<ShaderBase> reflect_shader = std::make_shared<SkyboxReflect>();
	reflect_shader->set_face_culling_func(backface_func);
	std::shared_ptr<ShaderBase> curr_shader = nullptr;
	reflect_shader->set_uniform("skybox_texture", skybox_texture);

	while (!window.window_should_be_close()) {
		frame.clear_color(vec3(0));
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);

		switch (type) {
		case 0:
			curr_shader = reflect_shader;
			break;
		default:
			curr_shader = shader_ptr;
			break;
		}

		// draw model
		curr_shader->set_uniform("eye_pos", camera_ptr->get_look_from());
		curr_shader->set_view(camera_ptr->get_view());
		curr_shader->set_projection(camera_ptr->get_projection());
		model_ptr->draw(frame, *curr_shader);
		// draw skybox
		skybox_shader_ptr->set_model(Draw::rotate_y(window.get_time() * 10.f));
		skybox_shader_ptr->set_view(camera_ptr->get_view());
		skybox_shader_ptr->set_projection(camera_ptr->get_projection());
		skybox_shader_ptr->set_uniform("skybox_texture", skybox_texture);
		skybox_model->draw(frame, *skybox_shader_ptr);
		window.draw(frame);
		poll_event();
	}
}

void SoftRenderer::test_plane() {
	vec3 light_dir = normalized(vec3(0, 2, 2));
	Texture2d normal_texture("resources/test_plane/brickwall_normal.jpg");
	Texture2d diffuse_texture("resources/test_plane/brickwall.jpg");
	shader_ptr->set_uniform("normal_texture", normal_texture);
	shader_ptr->set_uniform("diffuse_texture", diffuse_texture);
	shader_ptr->set_uniform("specular_factor", 32.f);
	shader_ptr->set_uniform("light_ambient", vec3(0.2f, 0.2f, 0.2f));
	shader_ptr->set_uniform("light_diffuse", vec3(0.5f, 0.5f, 0.5f));
	shader_ptr->set_uniform("light_specular", vec3(1.f, 1.f, 1.f));
	shader_ptr->set_uniform("light_dir", light_dir);
	shader_ptr->set_face_culling_func([](float cosine) { return cosine >= 0.f; });

	while (!window.window_should_be_close()) {
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		//shader_ptr->set_model(Draw::rotate_x(window.get_time() * 10.f));
		shader_ptr->set_view(camera_ptr->get_view());
		shader_ptr->set_projection(camera_ptr->get_projection());
		shader_ptr->set_uniform("eye_pos", camera_ptr->get_look_from());
		model_ptr->draw(frame, *shader_ptr);
		window.draw(frame);
		poll_event();
	}
}

void SoftRenderer::car() {
	vec3 light_dir = normalized(vec3(0, 2, 2));
	Texture2d normal_texture("resources/car/normal.jpg");
	Texture2d diffuse_texture("resources/car/green.jpg");
	shader_ptr->set_uniform("normal_texture", normal_texture);
	shader_ptr->set_uniform("diffuse_texture", diffuse_texture);
	shader_ptr->set_uniform("specular_factor", 32.f);
	shader_ptr->set_uniform("light_ambient", vec3(0.1f, 0.1f, 0.1f));
	shader_ptr->set_uniform("light_diffuse", vec3(0.9f, 0.9f, 0.9f));
	shader_ptr->set_uniform("light_specular", vec3(0.3f, 0.3f, 0.3f));
	shader_ptr->set_uniform("light_dir", light_dir);
	shader_ptr->set_face_culling_func([](float cosine) { return cosine >= 0.f; });

	frame.clear_color(vec3(0.1f, 0.1f, 0.1f));
	while (!window.window_should_be_close()) {
		frame.clear(FrameBufferType::ColorBuffer | FrameBufferType::DepthBuffer);
		shader_ptr->set_model(Draw::rotate_y(window.get_time() * 10.f));
		shader_ptr->set_view(camera_ptr->get_view());
		shader_ptr->set_projection(camera_ptr->get_projection());
		shader_ptr->set_uniform("eye_pos", camera_ptr->get_look_from());
		model_ptr->draw(frame, *shader_ptr);
		window.draw(frame);
		poll_event();
	}
}
