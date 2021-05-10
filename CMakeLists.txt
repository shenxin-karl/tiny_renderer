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
 )

target_include_directories(tiny_renderer PUBLIC
	${PROJECT_SOURCE_DIR}/include
)