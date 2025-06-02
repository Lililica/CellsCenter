#include <imgui.h>
#include <iostream>

#include "GLFW/glfw3.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static void key_callback(GLFWwindow *window, int key, int /*scancode*/,
                         int action, int /*mods*/) {
  if (key == GLFW_KEY_A && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void mouse_button_callback(GLFWwindow * /*window*/, int /*button*/,
                                  int /*action*/, int /*mods*/) {}

static void scroll_callback(GLFWwindow * /*window*/, double /*xoffset*/,
                            double /*yoffset*/) {};

static void cursor_position_callback(GLFWwindow * /*window*/, double /*xpos*/,
                                     double /*ypos*/) {};

static void size_callback(GLFWwindow * /*window*/, int width, int height) {
  width = WINDOW_WIDTH;
  height = WINDOW_HEIGHT;
}

int main()
{


  /* Initialize the library */
  if (!glfwInit()) {
    return -1;
  }

  GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)


/* Create a window and its OpenGL context */
#ifdef __APPLE__
  /* We need to explicitly ask for a 3.3 context on Mac */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
  window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TP1", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  
  /* Make the window's context current */
  glfwMakeContextCurrent(window);

 

  /* Hook input callbacks */
  glfwSetKeyCallback(window, &key_callback);
  glfwSetMouseButtonCallback(window, &mouse_button_callback);
  glfwSetScrollCallback(window, &scroll_callback);
  glfwSetCursorPosCallback(window, &cursor_position_callback);
  glfwSetWindowSizeCallback(window, &size_callback);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH);


    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();


  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");





    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClearColor(1.f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ImGui_ImplGlfw_NewFrame();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
                               ImGuiDockNodeFlags_PassthruCentralNode);

                        ImGui::Begin("Example");


                    if (ImGui::Button("1", ImVec2{50.f, 50.f}))
                        std::cout << "Clicked button 1\n";
                    ImGui::SameLine(); // Draw the next ImGui widget on the same line as the previous one. Otherwise it would be below it

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{1.f, 0.f, 0.f, 1.f}); // Changes the color of all buttons until we call ImGui::PopStyleColor(). There is also ImGuiCol_ButtonActive and ImGuiCol_ButtonHovered

                    ImGui::PushID(2); // When some ImGui items have the same label (for exemple the next two buttons are labeled "Yo") ImGui needs you to specify an ID so that it can distinguish them. It can be an int, a pointer, a string, etc.
                                      // You will definitely run into this when you create a button for each of your chess pieces, so remember to give them an ID!
                    if (ImGui::Button("Yo", ImVec2{50.f, 50.f}))
                        std::cout << "Clicked button 2\n";
                    ImGui::PopID(); // Then pop the id you pushed after you created the widget

                    ImGui::SameLine();
                    ImGui::PushID(3);
                    if (ImGui::Button("Yo", ImVec2{50.f, 50.f}))
                        std::cout << "Clicked button 3\n";
                    ImGui::PopID();

                    ImGui::PopStyleColor();

                    ImGui::End();

                      ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();


  glfwTerminate();
  return 0;



    // float value{0.f};

    // quick_imgui::loop(
    //     "Chess",
    //     {
    //         .init = [&]() {},
    //         .loop =
    //             [&]() {

    //                 ImGui::Begin("Example");

    //                 ImGui::SliderFloat("My Value", &value, 0.f, 3.f);

    //                 if (ImGui::Button("1", ImVec2{50.f, 50.f}))
    //                     std::cout << "Clicked button 1\n";
    //                 ImGui::SameLine(); // Draw the next ImGui widget on the same line as the previous one. Otherwise it would be below it

    //                 ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{1.f, 0.f, 0.f, 1.f}); // Changes the color of all buttons until we call ImGui::PopStyleColor(). There is also ImGuiCol_ButtonActive and ImGuiCol_ButtonHovered

    //                 ImGui::PushID(2); // When some ImGui items have the same label (for exemple the next two buttons are labeled "Yo") ImGui needs you to specify an ID so that it can distinguish them. It can be an int, a pointer, a string, etc.
    //                                   // You will definitely run into this when you create a button for each of your chess pieces, so remember to give them an ID!
    //                 if (ImGui::Button("Yo", ImVec2{50.f, 50.f}))
    //                     std::cout << "Clicked button 2\n";
    //                 ImGui::PopID(); // Then pop the id you pushed after you created the widget

    //                 ImGui::SameLine();
    //                 ImGui::PushID(3);
    //                 if (ImGui::Button("Yo", ImVec2{50.f, 50.f}))
    //                     std::cout << "Clicked button 3\n";
    //                 ImGui::PopID();

    //                 ImGui::PopStyleColor();

    //                 ImGui::End();
    //             },
    //     }
    // );
}