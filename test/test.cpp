#include "common.h"

#define DECLARE_TEST_FUNC(func) { #func, &func }
std::pair<std::string_view, std::function<bool(void)>> Test::test_func[]{
	DECLARE_TEST_FUNC(test_barycentric_coord),
	DECLARE_TEST_FUNC(test_viewport),
	DECLARE_TEST_FUNC(test_view),
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

