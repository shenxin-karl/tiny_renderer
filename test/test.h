#pragma once

class Test {
	static std::pair<std::string_view, std::function<bool(void)>> test_func[];
public:
	static void check();
public:
	static bool test_barycentric_coord();
	static bool test_viewport();
};