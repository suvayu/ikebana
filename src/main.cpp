#include <fmt/core.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include "quack.hpp"

int main() {
  // Initialize GLFW
  glfwInit();
  GLFWwindow *window =
      glfwCreateWindow(1280, 720, "DuckDB ImGui App", NULL, NULL);
  glfwMakeContextCurrent(window);

  // Initialize Dear ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 130");

  auto dbm = DBManager();

  setup_db(dbm.con);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create a window with a table
    ImGui::Begin("DuckDB Table");
    ImGui::Text("ID\tName");

    // query_db(dbm.con);

    ImGui::Text("%d\t%s", 1, "foo");
    ImGui::Text("%d\t%s", 2, "bar");
    ImGui::Text("%d\t%s", 314, "baz");
    ImGui::End();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
