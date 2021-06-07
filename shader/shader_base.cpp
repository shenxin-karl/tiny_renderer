#include "common.h"

void ShaderBase::set_model(const mat4 &_model) {
	model = _model;
	update_mvp();
}

void ShaderBase::set_view(const mat4 &_view) {
	view = _view;
	update_mvp();
}

void ShaderBase::set_projection(const mat4 &_projection) {
	projection = _projection;
	update_mvp();
}

float ShaderBase::calc_depth(const vec3 &_coords, const std::array<VertexRes *, 3> &vertices) {
	coords = _coords;
	float d1 = coords[0] / vertices[0]->position.w();
	float d2 = coords[1] / vertices[1]->position.w();
	float d3 = coords[2] / vertices[2]->position.w();
	depth = 1.f / (d1 + d2 + d3);
	return depth;
}

float ShaderBase::get_depth() const noexcept {
	return depth;
}

void ShaderBase::set_face_culling_func(const std::function<bool(float)> &func) {
	face_culling_func = func;
}

bool ShaderBase::is_enable_face_culling() const noexcept {
	return face_culling_func != nullptr;
}

const std::function<bool(float)> &ShaderBase::get_face_culling_func() const noexcept {
	return face_culling_func;
}

void ShaderBase::update_mvp() {
	mvp = projection * view * model;
}
