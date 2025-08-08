
#include "shader/program.hpp"
#include "shader/shader.hpp"
#include "utils.hpp"

//--------------------

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include "glm/ext/vector_float3.hpp"
#include "imguiRender.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 900

#define M 7
#define N 3

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
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

using Point     = std::pair<float, float>; // Représente un point (x, y)
using Adjacency = std::pair<Point, Point>; // Représente une paire d'indices de points adjacents

int main()
{
    /* Initialize the library */
    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window = nullptr; // (In the accompanying source code, this variable is global for simplicity)

/* Create a window and its OpenGL context */
#ifdef __APPLE__
    /* We need to explicitly ask for a 3.3 context on Mac */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    window =
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CellsCenter", nullptr, nullptr);
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
        std::cerr << "Failed to initialize GLAD" << '\n';
        return -1;
    }

    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);

    ImGui::CreateContext();
    App app;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH);

    // Setup

    Program program = loadProgram(SHADERS_PATH + std::string{"vertex.glsl"}, SHADERS_PATH + std::string{"fragment.glsl"});
    program.use();

    int width  = WINDOW_WIDTH; // Initialize width and height for the viewport
    int height = WINDOW_HEIGHT;

    std::vector<Vertex> v = {
        {glm::vec3{-0.5f, -0.5f, 0.f}, RED, {}},
        {glm::vec3{0.5f, -0.5f, 0.f}, RED, {}},
        {glm::vec3{0.f, 0.5f, 0.f}, RED, {}},
    };
    GLobject triangleObject(v, GL_TRIANGLES, false); // Object to hold the triangles

    v = {
        {glm::vec3{-0.25f, -0.25f, 0.f}, GREEN, {}},
        {glm::vec3{0.25f, -0.25f, 0.f}, GREEN, {}},
        {glm::vec3{0.f, 0.25f, 0.f}, GREEN, {}},
    };
    GLobject triangleObjectVoro(v, GL_TRIANGLES, false); // Object to hold the triangles

    app.setTriangleObjectVoro(&triangleObjectVoro);
    app.setTriangleObject(&triangleObject);

    app.init(); // Initialize the application

    double previousTime = 0.; // Initialize the previous time for frame rate control

    // RENDERING SECTION ________________________________________________________________________________________________________________________________

    while (!glfwWindowShouldClose(window))
    {
        // Get the delta time for frame rate control
        double        newTime   = glfwGetTime();
        static double deltaTime = newTime - previousTime;
        previousTime            = newTime;

        /* Render here */
        glClearColor(0.9f, .9f, .9f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwGetWindowSize(window, &width, &height);

#ifdef __APPLE__
        // Nothing

#else
        glViewport(0, 0, width, height);
#endif

        button_action(window, app.getCamera(), deltaTime); // Handle render.getCamera() movement based on key presses

        app.update(window, program); // Update the ImGui state

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
