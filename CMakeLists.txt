#
# MIT License
# Copyright (c) 2018-2019 Jongmin Yun
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
# CMakeList.txt : CMake project for VulkanSandbox, include source and define
# project specific logic here.
#
# Reference : https://pmateusz.github.io/c++/cmake/2018/03/11/cmake-project-setup.html
# Reference : https://stackoverflow.com/questions/45995784/how-to-set-compiler-options-with-cmake-in-visual-studio-2017
# Reference : https://www.codeproject.com/Articles/1181455/A-CMake-tutorial-for-Visual-Cplusplus-developers
# Reference : https://cmake.org/pipermail/cmake/2008-July/023021.html
#
cmake_minimum_required (VERSION 3.8 FATAL_ERROR)
project ("VulkanSandbox")
enable_testing()

# The version number
set (VulkanSandbox_VERSION_MAJOR 1)
set (VulkanSandbox_VERSION_MINOR 0)

set(CUDA_VERBOSE_BUILD ON)
set(CUDA_NVCC_FLAGS_DEBUG "-g -G")

# Set cutomized flag.
# https://stackoverflow.com/questions/41205725/how-to-disable-specific-warning-inherited-from-parent-in-visual-studio?rq=1
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201 /W4 /WX /MP /EHc /std:c++17")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

# Include library directories.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}\\Include)
include_directories(C:\\TPLibraries\\glfw-3.2.1\\include)
include_directories(C:\\VulkanSDK\\1.1.85.0\\Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}\\Library\\glm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}\\Library\\fmt\\Include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}\\Library\\stb)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}\\Library\\tinyobjloader)

# Include sub-projects.
add_subdirectory (Source)
# Add source to this project's executable.
add_executable(VulkanSandbox main.cpp)

set(GLFW_LIB_PATH		C:\\TPLibraries\\glfw-3.2.1\\build\\src\\${CMAKE_BUILD_TYPE})
set(VULKAN_LIB_PATH		C:\\VulkanSDK\\1.1.85.0\\Lib)
set(FMT_LIB_PATH		C:\\TPLibraries\\fmt-5.3.0\\build\\${CMAKE_BUILD_TYPE})
set(TINYOBJ_LIB_PATH	${CMAKE_CURRENT_SOURCE_DIR}\\Library\\tinyobjloader\\${CMAKE_BUILD_TYPE})

link_directories(${GLFW_LIB_PATH})
link_directories(${VULKAN_LIB_PATH})
link_directories(${FMT_LIB_PATH})
link_directories(${TINYOBJ_LIB_PATH})

find_library(GLFW3 NAMES glfw3dll.lib HINTS ${GLFW_LIB_PATH} REQUIRED)
find_library(TINYOBJ NAMES tinyobjloader.lib HINTS ${TINYOBJ_LIB_PATH} REQUIRED)
find_library(VULKAN NAMES vulkan-1.lib HINTS ${VULKAN_LIB_PATH} REQUIRED)
   
if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
  find_library(FMT NAMES fmtd.lib HINTS ${FMT_LIB_PATH} REQUIRED)
else()
  find_library(FMT NAMES fmt.lib HINTS ${FMT_LIB_PATH} REQUIRED)
endif()

target_link_libraries(VulkanSandbox ${FMT})
target_link_libraries(VulkanSandbox ${TINYOBJ})
target_link_libraries(VulkanSandbox ${GLFW3})
target_link_libraries(VulkanSandbox ${VULKAN})
target_link_libraries(VulkanSandbox Source)