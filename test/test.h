#pragma once

class Test {
	static std::pair<std::string_view, std::function<bool(void)>> test_func[];
public:
	static void check_depth();
public:
	static bool test_barycentric_coord();
	static bool test_view();
	static bool test_texture2d();
	static bool test_vec();
	static bool test_matrix();
	static bool test_orhto();
	static bool test_projection();
	static bool test_matrix_inverse();
};