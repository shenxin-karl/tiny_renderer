#include "common.h"

CameraBase::CameraBase(const vec3 &_look_from, const vec3 &_look_at, const vec3 &_look_up, 
	float _fov, float _aspect, float _near, float _far)
: look_from(_look_from), look_at(_look_at), look_up(_look_up)
, fov(_fov), aspect(_aspect), near(_near), far(_far) {

}
