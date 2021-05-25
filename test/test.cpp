#include "common.h"

#define DECLARE_TEST_FUNC(func) { #func, &func }
std::pair<std::string_view, std::function<bool(void)>> Test::test_func[]{
	DECLARE_TEST_FUNC(test_barycentric_coord),
	DECLARE_TEST_FUNC(test_viewport),
	DECLARE_TEST_FUNC(test_view),
	DECLARE_TEST_FUNC(test_texture2d),
	DECLARE_TEST_FUNC(test_vec),
	DECLARE_TEST_FUNC(test_matrix),
	DECLARE_TEST_FUNC(test_orhto),
	DECLARE_TEST_FUNC(test_projection),
};
#undef DECLARE_TEST_FUNC


void Test::check() {
	using std::pair;
	using std::function;
	using std::string_view;
	for (const pair<string_view, function<bool(void)>> par : test_func) {
		if (!par.second()) {
			std::cerr << "call " << par.first << " error!";
			std::system("pause");
			exit(EXIT_FAILURE);
		}
	}
}

bool Test::test_barycentric_coord() {
	auto check_coords = [](auto &coords) {
		return coords[0] >= 0.f && coords[1] >= 0.f && coords[2] >= 0.f;
	};

	vec3 v1(-100, 0, 0);
	vec3 v2(100, 0, 0);
	vec3 v3(0, 300, 0);
	vec3 point(-101, 0, 0);
	vec3 coords = Draw::barycentric_coord(point, v1, v2, v3);
	if (check_coords(coords))
		return false;

	point = { 100, 0, 0 };
	coords = Draw::barycentric_coord(point, v1, v2, v3);
	if (coords[0] != 0 || !check_coords(coords))
		return false;

	vec2 p = { 591, 352 };
	v1 = { 590.952515f, 352.662018f, 1.29826248f };
	v2 = { 591.149719f, 355.008820f, 1.29185522f };
	v3 = { 590.959473f, 352.809326f, 1.28281009f };
	coords = Draw::barycentric_coord(p, v1, v2, v3);
	//if (!(coords[0] > 0.f) || !(coords[1] > 0.f) || !(coords[2] > 0.f))
		//return false;
	return true;
}

bool Test::test_viewport() {
	int width = 800;
	int height = 600;
	vec4 point(0, 0, 0, 1);
	mat4 viewport = Draw::viewport(width, height);
	auto res = viewport * point;			// 应该被映射到 屏幕中心
	if (res.x() != (width / 2.f) || res.y() != (height / 2.f))
		return false;

	point = { 1, 1, 0, 1 };
	res = viewport * point;		// 被映射到 [width, height]
	if (res.x() != width || res.y() != height)
		return false;

	point = { -1, 1, 0, 1 };	// 映射到 [0, height]
	res = viewport * point;
	if (res.x() != 0 || res.y() != height)
		return false;
	return true;
}

bool Test::test_view() {
	{
		vec3 look_from = { 2, 2, 0 };
		vec3 look_up = { 0, 1, 0 };
		vec3 look_at = { 0, 0, 0 };
		auto view = Draw::view(look_from, look_up, look_at);
		auto point = vec4(look_from, 1.f);
		auto res = view * point;
		if (res.x() != 0 && res.y() != 0 && res.z() != 0 && res.w() != 1)
			return false;

		point = { 1, 1, 0, 1 };
		res = view * point;
		if (res.x() != 1 && res.y() != 1 && res.z() != 0 && res.w() != 1)
			return false;
	}
	{
		vec3 look_from = { 1, 2, 2 };
		vec3 look_up = { 0, 1, 0 };
		vec3 look_at = { 1, 2, 0 };
		mat4 view = Draw::view(look_from, look_up, look_at);
		vec4 point = { 0, 0, 0, 1 };
		auto point_res = view * point;
		if (point_res.x() != -1.f || point_res.y() != -2.f || point_res.z() != -2.f)
			return false;
	}
	return true;
}

bool Test::test_texture2d() {
	return true;

	int width = 1024;
	int height = 1024;
	Texture2d texture("test/test_texture_resource.png");
	FrameBuffer frame(width, height);
	frame.clear_color(vec3(0, 1, 0));
	frame.clear(FrameBufferType::ColorBuffer);
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			vec2 texcoord = { float(x) / width, float(y) / height };
			vec3 color = texture.rgb(texcoord);
			frame.set_color(vec3(float(x), float(y), 0), color);
		}
	}
	frame.save("test_texture2d", FrameBufferType::ColorBuffer);
	return true;
}

bool Test::test_vec() {
	vec3 v1 = { 1, 1, 1 };
	v1 *= 3;
	if (v1[0] != 3 || v1[1] != 3 || v1[2] != 3)
		return false;

	vec3 v2 = v1 * 3;
	if (v2[0] != 9 || v2[1] != 9 || v2[2] != 9)
		return false;

	vec3 v3 = v1 / 3;
	if (v3[0] != 1 || v3[1] != 1 || v3[2] != 1)
		return false;

	vec3 v4 = v3 + v2;
	if (v4[0] != 10.f || v4[1] != 10.f || v4[2] != 10.f)
		return false;

	vec3 v5 = vec3{ 5 };
	v4 -= v5;
	if (v4[0] != 5.f || v4[1] != 5.f || v4[2] != 5.f)
		return false;
	return true;
}

bool Test::test_matrix() {
	vec4 p(1);
	mat4 model = {
		0.5f, 0, 0, 0,
		0, 0.5f, 0, 0,
		0, 0, 0.5f, 0,
		0, 0, 0, 1,
	};
	auto res = model * p;
	if (res.x() != 0.5f || res.y() != 0.5f || res.z() != 0.5f)
		return false;

	model = model * model;
	res = model * p;
	if (res.x() != 0.25f || res.y() != 0.25f || res.z() != 0.25f)
		return false;
	return true;
}

bool Test::test_orhto() {
	std::vector<vec3> vertices = { {2, 0, -2}, {0, 2, -2}, {-2, 0, -2} };
	auto viewport = Draw::viewport(600, 600);
	auto view = Draw::view({ 0, 0, -1 }, { 0, 1, 0 }, { 0, 0, -2 });
#if 0
	for (int fov = 15; fov < 80; ++fov) {
		auto ortho = Draw::ortho(fov, 1, 15.f, 100.f);
		auto mvp = viewport * ortho * view;
		std::cout << "fov " << fov << std::endl;
		for (const auto &v :vertices)
			std::cout << (mvp *	vec4(v, 1.f)) << std::endl;
	}
#endif
	return true;
}

bool Test::test_projection() {
	std::vector<Vertex> vertices = {
		Vertex{ vec4{-0.5f, -0.5f, -0.5f, 1.0f}, vec3{0.0f,  0.0f, -1.0f},  vec2{ 0.0f, 0.0f}, },
		Vertex{ vec4{ 0.5f, -0.5f, -0.5f, 1.0f}, vec3{0.0f,  0.0f, -1.0f},  vec2{ 1.0f, 0.0f}, },
		Vertex{ vec4{ 0.5f,  0.5f, -0.5f, 1.0f}, vec3{0.0f,  0.0f, -1.0f},  vec2{ 1.0f, 1.0f}, },
	};
	constexpr int widht = 600;
	constexpr int height = 600;
	constexpr float aspect = static_cast<float>(widht) / static_cast<float>(height);
	constexpr float near = 0.001f;
	constexpr float far = 100.f;
	vec3 look_from(0, 0, 0);
	vec3 look_at(0, 0, -1);
	vec3 look_up(0, 1, 0);

	FrameBuffer frame(widht, height);
	LightShader shader;
	auto viewport(Draw::viewport(widht, height));
	shader.set_viewport(Draw::viewport(widht, height));
	shader.set_view(Draw::view(look_from, look_up, look_at));

	for (int fov = 1; fov < 90; ++fov) {
		shader.set_projection(Draw::projection(static_cast<float>(fov), aspect, near, far));
		//shader.set_projection(Draw::ortho(fov, aspect, near, far));

		std::vector<Vertex> out_vertices;
		out_vertices.reserve(vertices.size());
		for (int i = 0; i < vertices.size(); ++i) {
			vec4 position = shader.vertex(vertices[i], i);
			for (size_t idx = 0; idx < position.size()-1; ++idx)
				position[idx] /= position.w();

			out_vertices.push_back({
				position,
				vertices[i].normal,
				vertices[i].texcoords,
			});
		}
		//Draw::triangle(frame, shader, {
		//	&out_vertices[0],
		//	&out_vertices[1],
		//	&out_vertices[2],
		//});
		std::cout << "fov: " << fov << "\tp1: " << out_vertices[0].position 
				  << "\tp2: " << out_vertices[1].position
				  << "\tp3: " << out_vertices[2].position
				  << std::endl;
	}
	return true;
}

