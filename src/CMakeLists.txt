# list variable as there are no quotes
set(cxxflags -fdiagnostics-color=always -Wall -Wextra) # -Werror

add_executable(${PROJECT_NAME} main.cpp)

# must quote list variables to retain the list
target_compile_options(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${cxxflags}>")

target_link_libraries(${PROJECT_NAME} PRIVATE
  fmt::fmt
  imgui::imgui
  glfw
  OpenGL::GL
  duckdb
)
