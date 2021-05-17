#include "common.h"

SoftRenderer::SoftRenderer(int _width, int _height, 
						  std::shared_ptr<CameraBase> _camera_ptr, 
						  std::shared_ptr<ShaderBase> _shader_ptr, 
						  std::shared_ptr<Model> _model_ptr) 
: window(_width, _height, "MySoftRenderer"), frame(_width, _height)
, camera_ptr(_camera_ptr), shader_ptr(_shader_ptr), model_ptr(_model_ptr) {

	// TODO 增加窗口事件回调函数
}
