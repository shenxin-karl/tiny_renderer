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

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include <Windows.h>
#undef max
#undef min

#include "vec.hpp"
#include "matrix.hpp"

#include "test.h"

#include "texture2d.h"
#include "mesh.h"
#include "shader.h"
#include "window.h"
#include "frame_buffer.h"
#include "draw.h"
#include "model.h"
#include "LightShader.h"