cmake_minimum_required(VERSION 3.8)
project(tiny_renderer)
set(CMAKE_CXX_STANDARD 20)	


add_executable(tiny_renderer
	"include/common.h"			"src/main.cpp"
	"include/vec.hpp"			"include/matrix.hpp"
	"include/window.h"			"src/window.cpp"
	"include/draw.h"			"src/draw.cpp"
	"include/frame_buffer.h"	"src/frame_buffer.cpp"
	"include/mesh.h"			"src/mesh.cpp"
	"include/model.h"			"src/model.cpp"
	"include/texture2d.h"		"src/texture2d.cpp"
	"shader/shader_base.h"		"shader/shader_base.cpp"
	"test/test.h"				"test/test.cpp"
	"include/camera_base.h"		"src/camera_base.cpp"
	"include/fps_camera.h"		"src/fps_camera.cpp"
	"include/soft_renderer.h"	"src/soft_renderer.cpp"
	"include/fixed_camera.h"	"src/fixed_camera.cpp"
	"include/loader.h"			"src/loader.cpp" 
	"include/texture_cube.h"	"src/texture_cube.cpp"
	"shader/shader_args.h"		"shader/shader_args.cpp"

	"shader/depth_test/depth_shader.h"				"shader/depth_test/depth_shader.cpp"
	"shader/light/light_shader.h"					"shader/light/light_shader.cpp"
	"shader/texture_mapping/texture_mapping.h"		"shader/texture_mapping/texture_mapping.cpp"
	"shader/blinn_phong/blinn_phong.h"				"shader/blinn_phong/blinn_phong.cpp"
	"shader/normal_mapping/normal_mapping.h"		"shader/normal_mapping/normal_mapping.cpp"
	"shader/skybox/skybox_shader.h"					"shader/skybox/skybox_shader.cpp" 
	"shader/skybox/skybox_reflect.h"				"shader/skybox/skybox_reflect.cpp"
	"shader/parallax_mapping/parallax_mapping.h"	"shader/parallax_mapping/parallax_mapping.cpp")


target_include_directories(tiny_renderer PUBLIC
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/assimp/include
	${PROJECT_SOURCE_DIR}/stb/include
	${PROJECT_SOURCE_DIR}/shader
	${PROJECT_SOURCE_DIR}/test
)

target_link_directories(tiny_renderer PUBLIC
	${PROJECT_SOURCE_DIR}/assimp/lib
)

target_link_libraries(tiny_renderer PUBLIC
	assimp-vc142-mt
)