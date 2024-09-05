#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <duckdb.hpp>

int main() {
    // Initialize GLFW
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "DuckDB ImGui App", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Initialize Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Initialize DuckDB
    duckdb::DuckDB db;
    db.Connect(":memory:"); // Connect to an in-memory database

    // Create a table
    db.Query("CREATE TABLE test (id INTEGER, name VARCHAR);");

    // Insert some data
    db.Query("INSERT INTO test VALUES (1, 'John');");
    db.Query("INSERT INTO test VALUES (2, 'Jane');");

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create a window with a table
        ImGui::Begin("DuckDB Table");
        ImGui::Text("ID\tName");
        auto result = db.Query("SELECT * FROM test;");
        while (result->Next()) {
            ImGui::Text("%d\t%s", result->GetValue(0).GetValue<int>(), result->GetValue(1).GetValue<std::string>().c_str());
        }
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

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}
