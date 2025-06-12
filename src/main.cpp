
#include "shader/Shader.hpp"
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
#include "Delaunay/include/vector2.h"
#include "LlyodCentralisation.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imguiRender.hpp"
#include "object/sphere.hpp"

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 900

#define M 7
#define N 3

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

using Point     = std::pair<float, float>; // Représente un point (x, y)
using Adjacency = std::pair<Point, Point>; // Représente une paire d'indices de points adjacents

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

    Sphere sphere(.1f, 4, 2);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint vbo = 0;
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

    Sphere sphereCENTRE(.05f, 4, 2);

    GLuint vaoCENTRE;
    glGenVertexArrays(1, &vaoCENTRE);
    glBindVertexArray(vaoCENTRE);
    GLuint vboCENTRE;
    glGenBuffers(1, &vboCENTRE);
    glBindBuffer(GL_ARRAY_BUFFER, vboCENTRE);
    glBufferData(GL_ARRAY_BUFFER, sphereCENTRE.getVertexCount() * sizeof(ShapeVertex), sphereCENTRE.getDataPointer(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)offsetof(ShapeVertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)offsetof(ShapeVertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ShapeVertex), (void*)offsetof(ShapeVertex, texCoords));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    int width  = WINDOW_WIDTH; // Initialize width and height for the viewport
    int height = WINDOW_HEIGHT;

    // std::vector<glm::vec3> randomPositions;

    // randomPositions = extract_point_from_obj(ASSETS_PATH + std::string{"cow.obj"});          // Extract random positions from the OBJ file
    // save_text_from_vectObj(randomPositions, ASSETS_PATH + std::string{"result/result.txt"}); // Save the positions to a text file

    std::vector<dt::Vector2<double>> points;

#if 1
    int numberPoints = 500;

    std::default_random_engine             eng(std::random_device{}());
    std::uniform_real_distribution<double> dist_w(-10, 10);
    std::uniform_real_distribution<double> dist_h(-10, 10);

    std::cout << "Generating " << numberPoints << " random points" << '\n';

    points.reserve(numberPoints);
    for (int i = 0; i < numberPoints; ++i)
    {
        points.emplace_back(dist_w(eng), dist_h(eng));
    }

#else
    points = {
        // Example points, replace with randomPositions if needed
        dt::Vector2<double>(0, 10),
        dt::Vector2<double>(-10, 8),
        dt::Vector2<double>(-5, 6),
        dt::Vector2<double>(1, 4),
        dt::Vector2<double>(4, 7),
        dt::Vector2<double>(10, 10),
        dt::Vector2<double>(-7, -10),
        dt::Vector2<double>(5, -10)
    };
#endif

    render.getGraph()->pointList.reserve(points.size()); // Reserve space for points in the graph
    for (const auto& point : points)
    {
        render.getGraph()->pointList.emplace_back(point.x, point.y); // Convert dt::Vector2 to Point
    }

    render.getGraph()->doDelaunayAndCalculateCenters(); // Perform Delaunay triangulation and calculate circumcenters

    // Initialize the position of the sphere at each point in the graph to display them

    std::vector<glm::vec3> position(render.getGraph()->pointList.size());                   // Initial position of the sphere
    std::vector<glm::vec3> positionCENTRE(render.getGraph()->nearCellulePointsList.size()); // Position of the center sphere
    // std::cout << "Number of points: " << render.getGraph()->pointList.size() / 2 << std::endl;

    for (int i = 0; i < render.getGraph()->pointList.size(); ++i)
    {
        position[i].x = render.getGraph()->pointList[i].first;
        position[i].y = render.getGraph()->pointList[i].second;
        position[i].z = 0.f; // Set z to 0 for 2D points
    }

    for (int i = 0; i < render.getGraph()->nearCellulePointsList.size(); ++i)
    {
        positionCENTRE[i].x = render.getGraph()->nearCellulePointsList[i].first;
        positionCENTRE[i].y = render.getGraph()->nearCellulePointsList[i].second;
        positionCENTRE[i].z = 0.f; // Set z to 0 for 2D points
    }

    glm::vec3 origin = glm::vec3{0., 0., 0.};

    // RENDERING SECTION ________________________________________________________________________________________________________________________________

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

        // for (int i = 0; i < randomPositions.size(); ++i)
        // {
        //     draw_ball(render.getCamera(), sphere, randomPositions[i], program, vao, window); // Draw the sphere at random positions
        // }

        // draw_ball(render.getCamera(), sphere, position, program, vao, window); // Draw the sphere

        if (render.drawTriangles)
        {
            glPushMatrix();
            glLoadIdentity(); // load identity matrix

            glColor3f(0.0f, 1.0f, 0.0f);
            glLineWidth(2.0f);

            glBegin(GL_LINES);

            for (const auto& triangle : render.getGraph()->trianglesPoints)
            {
                glVertex3f(triangle[0].first, triangle[0].second, 0.f);
                glVertex3f(triangle[1].first, triangle[1].second, 0.f);
                glVertex3f(triangle[1].first, triangle[1].second, 0.f);
                glVertex3f(triangle[2].first, triangle[2].second, 0.f);
                glVertex3f(triangle[2].first, triangle[2].second, 0.f);
                glVertex3f(triangle[0].first, triangle[0].second, 0.f);
            }

            glEnd();
            glPopMatrix();
        }

        if (render.drawPoints)
        {
            for (int i = 0; i < render.getGraph()->pointList.size(); ++i)
            {
                draw_ball(render.getCamera(), sphere, position[i], program, vao, window); // Draw the sphere at each point
            }
        }
        if (render.drawVertex)
        {
            for (int i = 0; i < render.getGraph()->nearCellulePointsList.size(); ++i)
            {
                draw_ball(render.getCamera(), sphereCENTRE, positionCENTRE[i], program, vaoCENTRE, window); // Draw the center sphere at each circumcenter
            }
        }

        draw_ball(render.getCamera(), sphereCENTRE, origin, program, vaoCENTRE, window); // Draw the center sphere at each circumcenter

        render.render2D(position, positionCENTRE); // Render the ImGui interface

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
