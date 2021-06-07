#include "common.h"

SArgsPtr ShaderArgsBase::interp(const SArgsPtr &other, float t) const noexcept {
	return std::make_shared<ShaderArgsBase>();
}

SArgsPtr ShaderArgsBase::interp(const SArgsPtr &v2, const SArgsPtr &v3, const vec3 &coord, float depth) const noexcept {
	return std::make_shared<ShaderArgsBase>();
}

void ShaderArgsBase::perspective_divide(float inverse_z) noexcept {
}