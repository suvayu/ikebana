# list variable as there are no quotes
set(cxxflags -fdiagnostics-color=always -Wall -Wextra) # -Werror

add_executable(${PROJECT_NAME} main.cpp quack.cpp)
# must quote list variables to retain the list
target_compile_options(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${cxxflags}>")
target_link_libraries(${PROJECT_NAME} PRIVATE
  fmt::fmt
  imgui::imgui
  glfw
  OpenGL::GL
  duckdb
)

# dummy for testing
add_executable(ddtest ddtest.cpp quack.cpp)
# must quote list variables to retain the list
target_compile_options(ddtest PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:${cxxflags}>")
target_link_libraries(ddtest PRIVATE fmt::fmt duckdb)
