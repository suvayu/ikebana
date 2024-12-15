#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <duckdb.h>
#include <duckdb/main/connection.hpp>

void materialise_results(duckdb_result &res) {
  duckdb_data_chunk result = duckdb_fetch_chunk(res);
  if (!result) {
    // result is exhausted
    return;
  }
  // get the number of rows from the data chunk
  idx_t row_count = duckdb_data_chunk_get_size(result);
  // get the first column
  duckdb_vector col1 = duckdb_data_chunk_get_vector(result, 0);
  int32_t *col1_data = (int32_t *)duckdb_vector_get_data(col1);
  uint64_t *col1_validity = duckdb_vector_get_validity(col1);

  // get the second column
  duckdb_vector col2 = duckdb_data_chunk_get_vector(result, 1);
  char **col2_data = (char **)duckdb_vector_get_data(col2);
  uint64_t *col2_validity = duckdb_vector_get_validity(col2);

  // iterate over the rows
  for (idx_t row = 0; row < row_count; row++) {
    if (duckdb_validity_row_is_valid(col1_validity, row)) {
      printf("%d", col1_data[row]);
    } else {
      printf("NULL");
    }
    printf(",");
    if (duckdb_validity_row_is_valid(col2_validity, row)) {
      printf("%s", col2_data[row]);
    } else {
      printf("NULL");
    }
    printf("\n");
    ImGui::Text("%d\t%s", col1_data[row], col2_data[row]);
  }
  duckdb_destroy_data_chunk(&result);
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

    materialise_results(result);
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
