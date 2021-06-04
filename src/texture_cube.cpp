#include "common.h"

TextureCube::TextureCube(const std::array<std::string, 6> &faces)
: textures{ faces[0], faces[1], faces[2], faces[3], faces[4], faces[5] }  {
	assert(static_cast<bool>(textures[0]));
	assert(static_cast<bool>(textures[1]));
	assert(static_cast<bool>(textures[2]));
	assert(static_cast<bool>(textures[3]));
	assert(static_cast<bool>(textures[4]));
	assert(static_cast<bool>(textures[5]));
}

vec3 TextureCube::rgb(const vec3 &direction) const noexcept {
	auto &&[index, texcoord] = calc_texture_index(direction);
	return textures[index].rgb(texcoord);
}

vec4 TextureCube::rgba(const vec3 &direction) const noexcept {
	auto &&[index, texcoord] = calc_texture_index(direction);
	return textures[index].rgba(texcoord);
}

std::tuple<int, vec2> TextureCube::calc_texture_index(const vec3 &direction) const noexcept {
	float abs_x = std::abs(direction.x());
	float abs_y = std::abs(direction.y());
	float abs_z = std::abs(direction.z());
	float sc;
	float tc;
	float ma;
	int index;
	vec2 texcoord;
	if (abs_x > abs_y && abs_x > abs_z) {	// use left or right 
		if (direction.x() >= 0.f) {
			sc = -direction.z();
			index = 0;
		} else {
			sc = direction.z();
			index = 1;
		}
		tc = -direction.y();
		ma = abs_x;
	} else if (abs_y > abs_z) {				// use top or bottom
		if (direction.y() >= 0.f) {
			tc = direction.z();
			index = 2;
		} else {
			tc = -direction.z();
			index = 3;
		}
		sc = direction.x();
		ma = abs_y;
	} else {								// use front or back
		if (direction.z() >= 0.f) {
			index = 4;
			sc = direction.x();
		} else {
			index = 5;
			sc = -direction.x();
		}
		tc = -direction.y();
		ma = abs_z;
	}
	constexpr float inverse_two = 1.f / 2.f;
	texcoord.s() = (sc / ma + 1) * inverse_two;
	texcoord.t() = (tc / ma + 1) * inverse_two;
	return { index, texcoord };
}
