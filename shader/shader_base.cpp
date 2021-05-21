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

void ShaderBase::set_viewport(const mat4 &_viewport) {
	viewport = _viewport;
	update_mvp();
}

float ShaderBase::calc_depth(const vec3 &_coords, const std::array<Vertex *, 3> &vertices) {
	coords = _coords;
	float d1 = coords[0] / vertices[0]->position.w();
	float d2 = coords[1] / vertices[1]->position.w();
	float d3 = coords[2] / vertices[2]->position.w();
	float depth = 1.f / (d1 + d2 + d3);
	inverse_z = 1.f / depth;
	return depth;
}

void ShaderBase::update_mvp() {
	mvp = viewport * projection * view * model;
}
