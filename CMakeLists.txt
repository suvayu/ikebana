cmake_minimum_required(VERSION 3.10)
project(DuckDB_ImGui)

# Set the C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the vendored dependencies
add_subdirectory(third_party/DuckDB)
add_subdirectory(third_party/imgui)

# Add the source files
add_executable(${PROJECT_NAME} main.cpp)

# Link the dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC
  DuckDB::DuckDB
  imgui
  OpenGL::GL
  GLFW::GLFW
)

# Set the include directories
target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/DuckDB/include
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui
)

# Set the C++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# Set the build type
set(CMAKE_BUILD_TYPE Debug)
