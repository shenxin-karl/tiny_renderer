#include "common.h"

#define DECLARE_TEST_FUNC(func) { #func, &func }
std::pair<std::string_view, std::function<bool(void)>> Test::test_func[]{
	DECLARE_TEST_FUNC(test_barycentric_coord),
	DECLARE_TEST_FUNC(test_viewport),
	DECLARE_TEST_FUNC(test_view),
	DECLARE_TEST_FUNC(test_texture2d),
	DECLARE_TEST_FUNC(test_vec),
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
			vec3 color = texture.texture(texcoord);
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

