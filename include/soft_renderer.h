#pragma once

class SoftRenderer {
	Window						window;
	FrameBuffer					frame;
	std::shared_ptr<CameraBase> camera_ptr;
	std::shared_ptr<ShaderBase> shader_ptr;
	std::shared_ptr<Model>		model_ptr;
public:
	SoftRenderer(int _width, int _height, std::shared_ptr<CameraBase> _camera_ptr, 
				 std::shared_ptr<ShaderBase> _shader_ptr, std::shared_ptr<Model> _model_ptr);
	~SoftRenderer() = default;
public:
	void normal_renderer();	
};