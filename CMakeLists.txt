cmake_minimum_required(VERSION 3.12)

if(DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
else()
    message(FATAL_ERROR "Please set the VCPKG_ROOT environment variable to the path of your vcpkg installation.")
endif()
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(ikebana)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg dependencies
find_package(fmt REQUIRED)
find_package(imgui REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# vendored dependencies
set(VENDOR ${CMAKE_SOURCE_DIR}/third_party)
add_subdirectory(${VENDOR}/duckdb)

# implementation
add_subdirectory(src)
