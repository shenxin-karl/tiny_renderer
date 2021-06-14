#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <functional>
#include <chrono>
#include <limits>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <string_view>
#include <tuple>
#include <variant>
#include <random>
#include <span>
#include <optional>
#include <utility>
#include <cstring>
#include <type_traits>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include <Windows.h>
#include <conio.h>
#undef max
#undef min
#undef far
#undef near

#include "vec.hpp"
#include "matrix.hpp"

#include "test.h"

#include "texture2d.h"
#include "mesh.h"
#include "texture_cube.h"
#include "shader_args.h"
#include "shader_base.h"
#include "window.h"
#include "frame_buffer.h"
#include "draw.h"
#include "model.h"
#include "camera_base.h"
#include "fps_camera.h"
#include "fixed_camera.h"
#include "soft_renderer.h"
#include "loader.h"

#include "depth_test/depth_shader.h"
#include "light/light_shader.h"
#include "texture_mapping/texture_mapping.h"
#include "blinn_phong/blinn_phong.h"
#include "skybox/skybox_shader.h"
#include "skybox/skybox_reflect.h"
#include "normal_mapping/normal_mapping.h"
#include "parallax_mapping/parallax_mapping.h"