#include <fmt/core.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <duckdb.h>

void materialise_results(duckdb_result &res) {
  duckdb_data_chunk chunk = duckdb_fetch_chunk(res);
  if (!chunk) {
    // result is exhausted
    return;
  }
  // get the number of rows from the data chunk
  idx_t row_count = duckdb_data_chunk_get_size(chunk);
  // get the first column
  duckdb_vector col1 = duckdb_data_chunk_get_vector(chunk, 0);
  int32_t *col1_data = (int32_t *)duckdb_vector_get_data(col1);
  uint64_t *col1_validity = duckdb_vector_get_validity(col1);

  // get the second column
  duckdb_vector col2 = duckdb_data_chunk_get_vector(chunk, 1);
  duckdb_string_t *col2_data = (duckdb_string_t *)duckdb_vector_get_data(col2);
  uint64_t *col2_validity = duckdb_vector_get_validity(col2);

  // iterate over the rows
  for (idx_t row = 0; row < row_count; row++) {
    if (duckdb_validity_row_is_valid(col1_validity, row)) {
      fmt::print("{:d}, {}", col1_data[row]);
    } else {
      fmt::print("NULL");
    }
    if (duckdb_validity_row_is_valid(col2_validity, row)) {
      fmt::print("{:d}", col2_data[row].value.pointer.length);
    } else {
      fmt::print("NULL");
    }
    fmt::print("\n");
  }
  // ImGui::Text("%d\t%s", col1_data[row], col2_data[row]);
  duckdb_destroy_data_chunk(&chunk);
}

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

  duckdb_database db;
  duckdb_connection con;

  // Connect to an in-memory database
  if (duckdb_open(NULL, &db) == DuckDBError) {
    // handle error
  }
  if (duckdb_connect(db, &con) == DuckDBError) {
    // handle error
  }

  duckdb_state state;
  duckdb_result result;

  state =
      duckdb_query(con, "CREATE TABLE test (id INTEGER, name VARCHAR);", NULL);
  if (state == DuckDBError) {
    // handle error
  }

  // Insert some data
  state = duckdb_query(con, "INSERT INTO test VALUES (1, 'John'), (2, 'Jane');",
                       NULL);
  if (state == DuckDBError) {
    // handle error
  }

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create a window with a table
    ImGui::Begin("DuckDB Table");
    ImGui::Text("ID\tName");

    state = duckdb_query(con, "SELECT * FROM test;", &result);
    if (state == DuckDBError) {
      // handle error
    }

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

  // Cleanup
  duckdb_destroy_result(&result);
  duckdb_disconnect(&con);
  duckdb_close(&db);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
