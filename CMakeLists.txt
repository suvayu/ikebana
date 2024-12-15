cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
else()
    message(FATAL_ERROR "Please set the VCPKG_ROOT environment variable to the path of your vcpkg installation.")
endif()
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(ikebana)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Add the vendored dependencies
add_subdirectory(third_party/duckdb)

# Add the source files
add_executable(${PROJECT_NAME} main.cpp)

# Link the dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
  fmt::fmt
  imgui::imgui
  glfw
  OpenGL::GL
  duckdb
)

# Set the include directories
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/duckdb/include
)

# Set the C++ flags
set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always ${CMAKE_CXX_FLAGS} -Wall -Wextra") # -Werror

# Set the build type
set(CMAKE_BUILD_TYPE Debug)
