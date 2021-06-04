#pragma once

class Loader {
public:
	static Model load_obj(const std::string &path);
	static Model create_test_cube_obj();
	static Model cearte_one_triangle();
};