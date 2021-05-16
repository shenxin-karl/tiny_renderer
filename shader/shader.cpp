#include "common.h"

void Shader::set_model(const mat4 &_model) {
	model = _model;
	update_mvp();
}

void Shader::set_view(const mat4 &_view) {
	view = _view;
	update_mvp();
}

void Shader::set_projection(const mat4 &_projection) {
	projection = _projection;
	update_mvp();
}

void Shader::set_viewport(const mat4 &_viewport) {
	viewport = _viewport;
	update_mvp();
}

void Shader::set_coords(const vec3 &_coords) {
	coords = _coords;
}

void Shader::set_inverse_z(float _inverse_z) {
	inverse_z = _inverse_z;
}

void Shader::update_mvp() {
	mvp = viewport * projection * view * model;
}
