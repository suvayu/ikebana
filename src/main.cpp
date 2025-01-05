#include <climits>
#include <fmt/core.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include "quack.hpp"

static void glfw_error_callback(int error, const char *description) {
  fmt::println(stderr, "GLFW Error {:d} {:s}", error, description);
}

int main() {
  // Initialize GLFW
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  GLFWwindow *window = glfwCreateWindow(1280, 720, "Ikegana", nullptr, nullptr);
  if (window == nullptr)
    return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Initialize Dear ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  // FIXME: do version properly
  ImGui_ImplOpenGL3_Init("#version 130");

  // TODO: fonts

  // FIXME: segfaults
  // const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
  const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

  auto dbm = DBManager();
  setup_db(dbm.con);
  auto table_contents = query_db(dbm.con);

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
      // Create a window with a table
      ImGui::Begin("DuckDB Table");

      static ImGuiTableFlags flags =
          ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg |
          ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
          ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
          ImGuiTableFlags_Hideable;

      static size_t nrows = table_contents.size();
      static size_t ncols = table_contents[0].size();

      ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 200);
      if (ImGui::BeginTable("select_test_table", ncols, flags, outer_size)) {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible

        for (size_t col = 0; col < ncols; ++col) {
          ImGui::TableSetupColumn(table_contents[0][col].data(),
                                  ImGuiTableColumnFlags_None);
        }
        ImGui::TableHeadersRow();

        // Demonstrate using clipper for large vertical lists
        ImGuiListClipper clipper;
        clipper.Begin(INT_MAX); // when we don't know, row count otherwise
        while (clipper.Step()) {
          for (size_t row = clipper.DisplayStart;
               row < size_t(clipper.DisplayEnd); row++) {
            if (row >= nrows)
              break;
            ImGui::TableNextRow();
            for (size_t col = 0; col < ncols; col++) {
              ImGui::TableSetColumnIndex(col);
              ImGui::Text("%s", table_contents[row][col].data());
            }
          }
        }
        ImGui::EndTable();
      }

      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
  }

  // cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
