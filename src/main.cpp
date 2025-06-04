
#include "shader/program.hpp"
#include "utils.hpp"

//--------------------

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <iostream>
#include <random>
#include <vector>
#include "glm/ext/vector_float3.hpp"
#include "imguiRender.hpp"
#include "object/sphere.hpp"
#include "trackball/TrackBall.hpp"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void mouse_button_callback(GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/) {}

static void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double /*yoffset*/) {};

static void cursor_position_callback(GLFWwindow* /*window*/, double /*xpos*/, double /*ypos*/) {};

static void size_callback(GLFWwindow* /*window*/, int width, int height)
{
    width  = WINDOW_WIDTH;
    height = WINDOW_HEIGHT;
}

int main()
{
    /* Initialize the library */
    if (!glfwInit())
    {
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
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Initialize GLAD after context creation
    if (!gladLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);

    ImGui::CreateContext();
    Render render;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH);

    // Setup

    Program program = loadProgram(SHADERS_PATH + std::string{"vertex.glsl"}, SHADERS_PATH + std::string{"fragment.glsl"});
    program.use();

    Sphere sphere(.1f, 10, 10); // Create a sphere with radius 1, 20 latitude and longitude divisions

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)offsetof(ShapeVertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)offsetof(ShapeVertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)offsetof(ShapeVertex, texCoords));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glm::mat4 ProjMatrix;
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;
    glm::mat4 MVP;

    int width;
    int height;

    glm::vec3 position(0.f, 0.f, 0.f); // Initial position of the sphere

    std::vector<glm::vec3> randomPositions;

    randomPositions = extract_point_from_obj(ASSETS_PATH + std::string{"cow.obj"});          // Extract random positions from the OBJ file
    save_text_from_vectObj(randomPositions, ASSETS_PATH + std::string{"result/result.txt"}); // Save the positions to a text file

    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(1.f, 0.5f, 0.5f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwGetWindowSize(window, &width, &height);

#ifdef __APPLE__
        // Nothing

#else
        glViewport(0, 0, width, height);
#endif

        button_action(window, render.getCamera()); // Handle render.getCamera() movement based on key presses

        for (int i = 0; i < randomPositions.size(); ++i)
        {
            draw_ball(render.getCamera(), sphere, randomPositions[i], program, vao, window); // Draw the sphere at random positions
        }

        draw_ball(render.getCamera(), sphere, position, program, vao, window); // Draw the sphere

        render.render2D(); // Render the ImGui interface

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
}
