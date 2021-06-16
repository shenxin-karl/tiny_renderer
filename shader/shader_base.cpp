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
