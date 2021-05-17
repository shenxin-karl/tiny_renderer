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
	"shader/shader.h"			"shader/shader.cpp"
	"shader/light_shader.h"		"shader/light_shader.cpp"
	"test/test.h"				"test/test.cpp"
	"include/camera.h"			"src/camera.cpp"
)

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