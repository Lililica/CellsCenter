
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
#include <numbers>
#include <random>
#include <vector>
#include "Delaunay/include/vector2.h"
#include "LlyodCentralisation.hpp"
#include "glm/ext/vector_float3.hpp"
#include "imguiRender.hpp"
#include "object/sphere.hpp"

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

void drawCircle(float r, Point& center)
{
    glPushMatrix();
    glLoadIdentity(); // load identity matrix

    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(2.0f);

    glBegin(GL_LINES);

    float x = center.first;
    float y = center.second;

    for (float i = 0.0f; i <= 360; i++)
        glVertex3f(r * cos(M_PI * i / 180.0) + x, r * sin(M_PI * i / 180.0) + y, 0.0f);

    glEnd();
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

    std::vector<Vertex> v = {
        {glm::vec3{-0.5f, -0.5f, 0.f}, glm::vec3{1.f, 0.f, 0.f}, {}},
        {glm::vec3{0.5f, -0.5f, 0.f}, glm::vec3{1.f, 0.f, 0.f}, {}},
        {glm::vec3{0.f, 0.5f, 0.f}, glm::vec3{1.f, 0.f, 0.f}, {}},
    };
    GLobject triangleObject(v, GL_TRIANGLES, false); // Object to hold the triangles

    v = {
        {glm::vec3{-0.25f, -0.25f, 0.f}, glm::vec3{0.f, 1.f, 0.f}, {}},
        {glm::vec3{0.25f, -0.25f, 0.f}, glm::vec3{0.f, 1.f, 0.f}, {}},
        {glm::vec3{0.f, 0.25f, 0.f}, glm::vec3{0.f, 1.f, 0.f}, {}},
    };
    GLobject triangleObjectVoro(v, GL_TRIANGLES, false); // Object to hold the triangles

    int width  = WINDOW_WIDTH; // Initialize width and height for the viewport
    int height = WINDOW_HEIGHT;

    // std::vector<glm::vec3> randomPositions;

    // randomPositions = extract_point_from_obj(ASSETS_PATH + std::string{"cow.obj"});          // Extract random positions from the OBJ file
    // save_text_from_vectObj(randomPositions, ASSETS_PATH + std::string{"result/result.txt"}); // Save the positions to a text file

    std::vector<dt::Vector2<double>> points;

#if 0
    int numberPoints = 500;

    std::default_random_engine             eng(std::random_device{}());
    std::uniform_real_distribution<double> rayon(0, render.getGraph()->radius - 1.); // Random radius from 0 to the graph's radius
    std::uniform_real_distribution<double> angle(0, std::numbers::pi * 2);           // 2 * pi

    std::cout << "Generating " << numberPoints << " random points" << '\n';

    points.reserve(numberPoints);
    for (int i = 0; i < numberPoints; ++i)
    {
        points.emplace_back(
            rayon(eng) * std::cos(angle(eng)), // Random x coordinate
            rayon(eng) * std::sin(angle(eng))  // Random y coordinate
        );                                     // Generate random points in a circle of radius 10
    }

#else
    points = {
        // Example points, replace with randomPositions if needed
        dt::Vector2<double>(0, 0),
        // dt::Vector2<double>(1, 1),
        // dt::Vector2<double>(1.5, 1),
        dt::Vector2<double>(3, 3),
        dt::Vector2<double>(3, -3),
        dt::Vector2<double>(-3, -3),
        dt::Vector2<double>(-3, 3),
        dt::Vector2<double>(2, 0),
        dt::Vector2<double>(0, 2),
        dt::Vector2<double>(-2, 0),
        dt::Vector2<double>(0, -2),
    };
#endif

    // add border points to the graph
    for (int i = 0; i < 50; ++i)
    {
        float angle = static_cast<float>(i) * (2.f * std::numbers::pi / 50.f); // Calculate the angle for each point
        render.getGraph()->pointList.emplace_back(
            render.getGraph()->radius * std::cos(angle),                                                        // x coordinate
            render.getGraph()->radius * std::sin(angle)                                                         // y coordinate
        );                                                                                                      // Generate points on the boundary of the circle
        render.getGraph()->idxPointBorder.push_back(static_cast<int>(render.getGraph()->pointList.size() - 1)); // Add the index of the border point to the list
    }

    render.getGraph()->pointList.reserve(points.size()); // Reserve space for points in the graph
    for (const auto& point : points)
    {
        render.getGraph()->pointList.emplace_back(point.x, point.y); // Convert dt::Vector2 to Point
    }

    // render.getGraph()->pointList = load_text_to_pointList(ASSETS_PATH + std::string{"pointExemple/pointListV0.txt"}); // Load points from a text file

    render.getGraph()->doDelaunayAndCalculateCenters(); // Perform Delaunay triangulation and calculate circumcenters

    // std::cout << "Number of points: " << render.getGraph()->pointList.size() / 2 << std::endl;

    float factorTriangle = 0.15f; // Factor to scale the triangles

    v.clear(); // Clear the vertex vector
    for (int i = 0; i < render.getGraph()->pointList.size(); ++i)
    {
        v.push_back({glm::vec3{render.getGraph()->pointList[i].first - factorTriangle, render.getGraph()->pointList[i].second - factorTriangle, 0.f}, RED, {}});
        v.push_back({glm::vec3{render.getGraph()->pointList[i].first + factorTriangle, render.getGraph()->pointList[i].second - factorTriangle, 0.f}, RED, {}});
        v.push_back({glm::vec3{render.getGraph()->pointList[i].first, render.getGraph()->pointList[i].second + factorTriangle, 0.f}, RED, {}});
    }
    triangleObject.set_vertex_data(v); // Set the vertex data for the triangle object

    float factorTriangle2 = 0.07f; // Factor to scale the circumcenters
    v.clear();                     // Clear the vertex vector for circumcenters
    for (int i = 0; i < render.getGraph()->nearCellulePointsList.size(); ++i)
    {
        v.push_back({glm::vec3{render.getGraph()->nearCellulePointsList[i].first - factorTriangle2, render.getGraph()->nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
        v.push_back({glm::vec3{render.getGraph()->nearCellulePointsList[i].first + factorTriangle2, render.getGraph()->nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}});
        v.push_back({glm::vec3{render.getGraph()->nearCellulePointsList[i].first, render.getGraph()->nearCellulePointsList[i].second + factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
    }
    triangleObjectVoro.set_vertex_data(v); // Set the vertex data for the circumcenter object

    glm::vec3 origin             = glm::vec3{0., 0., 0.};
    float     radiusEnergiePoint = .5f; // Radius of the energy point sphere

    Point TopLeft(-10.f, 10.f); // Top-left corner of the viewport

    std::vector<Point> examplePoints;

    int nbrFlips = 1; // Initialize the number of flips

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

        button_action(window, render.getCamera(), deltaTime); // Handle render.getCamera() movement based on key presses

        // get the mouse position
        if (render.mousePoint)
        {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            render.getGraph()->pointList[0].first  = (static_cast<float>(mouseX) / width) * 30 - 15.;           // Convert mouse position to OpenGL coordinates
            render.getGraph()->pointList[0].second = (static_cast<float>(height - mouseY) / height) * 30 - 15.; // Convert mouse position to OpenGL coordinates
            // position[0].x                          = render.getGraph()->pointList[0].first;
            // position[0].y                          = render.getGraph()->pointList[0].second;
            // position[0].z                          = 0.f; // Set z to 0 for 2D points
        }

        if (render.drawTriangles)
        {
            std::vector<Vertex> vertices;                                 // Create a vector to hold the vertices of the triangles
            glm::vec3           currentColor = BLUE;                      // Color for the triangles
            vertices.reserve(render.getGraph()->idxTriangles.size() * 3); // Reserve space
            for (const auto& triangle : render.getGraph()->idxTriangles)
            {
                Point p1 = render.getGraph()->pointList[triangle[0]];                        // Get the first point of the triangle
                Point p2 = render.getGraph()->pointList[triangle[1]];                        // Get the second point of the triangle
                Point p3 = render.getGraph()->pointList[triangle[2]];                        // Get the third point of the triangle
                vertices.push_back({glm::vec3{p1.first, p1.second, 0.f}, currentColor, {}}); // Add the first vertex
                vertices.push_back({glm::vec3{p2.first, p2.second, 0.f}, currentColor, {}}); // Add the second vertex
                vertices.push_back({glm::vec3{p2.first, p2.second, 0.f}, currentColor, {}}); // Add the second vertex
                vertices.push_back({glm::vec3{p3.first, p3.second, 0.f}, currentColor, {}}); // Add the third vertex
                vertices.push_back({glm::vec3{p3.first, p3.second, 0.f}, currentColor, {}}); // Add the third vertex
                vertices.push_back({glm::vec3{p1.first, p1.second, 0.f}, currentColor, {}}); // Add the first vertex
            }

            GLobject drawTriangles(vertices, GL_LINES, false); // Draw the triangles using the GLobject class
            drawTriangles.draw();                              // Draw the triangles
        }

        if (render.drawCelluleBorder)
        {
            for (const auto& list : render.getGraph()->nearCellulePointsTriees) // Get the list of segments formed by the near cell points
            {
                std::vector<Vertex> vertices;             // Create a vector to hold the vertices of the segments
                glm::vec3           currentColor = BLACK; // Color for the segments
                vertices.reserve(list.size() * 2);        // Reserve space for the segments
                for (const auto& segment : list)          // Iterate through each segment
                {
                    vertices.push_back({glm::vec3{segment[0].first, segment[0].second, 0.f}, currentColor, {}}); // Add the first point of the segment
                    vertices.push_back({glm::vec3{segment[1].first, segment[1].second, 0.f}, currentColor, {}}); // Add the second point of the segment
                }
                GLobject drawCelluleBorder(vertices, GL_LINES, false); // Draw the segments using
                drawCelluleBorder.draw();                              // Draw the segments
            }
        }

        if (render.drawCircles)
        {
            for (const auto& circle : render.getGraph()->triangleCircles) // Iterate through each circle
            {
                std::vector<Vertex> vertices;                                // Create a vector to hold the vertices of the circle
                glm::vec3           currentColor = BLACK;                    // Color for the circle
                vertices.reserve(render.getGraph()->triangleCircles.size()); // Reserve space for the circle vertices
                int nbrIterations = 50;                                      // Number of iterations for drawing the circle

                vertices.push_back({glm::vec3{circle.first.first + circle.second, circle.first.second, 0.f}, currentColor, {}}); // Add the vertex to the circle

                for (int j = 0; j < nbrIterations + 1; ++j)
                {
                    float angle = j * std::numbers::pi * 2. / nbrIterations; // Convert degrees to radians
                    float x     = circle.first.first + circle.second * std::cos(angle);
                    float y     = circle.first.second + circle.second * std::sin(angle);
                    vertices.push_back({glm::vec3{x, y, 0.f}, currentColor, {}}); // Add the vertex to the circle
                    vertices.push_back({glm::vec3{x, y, 0.f}, currentColor, {}}); // Add the vertex to the circle
                }
                GLobject drawCircles(vertices, GL_LINES, false); // Draw the circle using the GLobject class
                drawCircles.draw();                              // Draw the circle
            }
        }
        if (render.drawOrientedBox)
        {
            for (const auto& orientedBox : render.getGraph()->allOrientedBoxes) // Iterate through each oriented bounding box
            {
                std::vector<Vertex> vertices;             // Create a vector to hold the vertices of the oriented bounding box
                glm::vec3           currentColor = BLACK; // Color for the oriented bounding box
                vertices.reserve(4);                      // Reserve space for the vertices

                // Add the four corners of the oriented bounding box
                vertices.push_back({glm::vec3{orientedBox[0].first, orientedBox[0].second, 0.f}, currentColor, {}}); // Bottom-left corner
                vertices.push_back({glm::vec3{orientedBox[1].first, orientedBox[1].second, 0.f}, currentColor, {}}); // Bottom-right corner
                vertices.push_back({glm::vec3{orientedBox[1].first, orientedBox[1].second, 0.f}, currentColor, {}}); // Bottom-right corner
                vertices.push_back({glm::vec3{orientedBox[2].first, orientedBox[2].second, 0.f}, currentColor, {}}); // Top-right corner
                vertices.push_back({glm::vec3{orientedBox[2].first, orientedBox[2].second, 0.f}, currentColor, {}}); // Top-right corner
                vertices.push_back({glm::vec3{orientedBox[3].first, orientedBox[3].second, 0.f}, currentColor, {}}); // Top-left corner
                vertices.push_back({glm::vec3{orientedBox[3].first, orientedBox[3].second, 0.f}, currentColor, {}}); // Top-left corner
                vertices.push_back({glm::vec3{orientedBox[0].first, orientedBox[0].second, 0.f}, currentColor, {}}); // Bottom-left corner

                GLobject drawOrientedBox(vertices, GL_LINES, false); // Draw the oriented bounding box using the GLobject class
                drawOrientedBox.draw();                              // Draw the oriented bounding box
            }
        }

        if (render.drawPoints)
        {
            draw_ball(render.getCamera(), triangleObject, program, window); // Draw the sphere at each point
        }
        if (render.drawVertex)
        {
            draw_ball(render.getCamera(), triangleObjectVoro, program, window); // Draw the center sphere at each circumcenter
        }

        if (render.nbrPointsChanged)
        {
            render.getGraph()->pointList.clear();                               // Clear the previous points
            render.getGraph()->nearCellulePointsList.clear();                   // Clear the previous circumcenters
            render.getGraph()->idxPointBorder.clear();                          // Clear the previous border points
            render.getGraph()->pointList.reserve(render.getGraph()->nbrPoints); // Reserve space for new points
            render.getGraph()->allCircles.clear();                              // Clear the list of circles
            render.getGraph()->energies.clear();                                // Clear the list of energies
            std::default_random_engine             eng(std::random_device{}());
            std::uniform_real_distribution<double> rayon(0, render.getGraph()->radius - 5.); // Random radius from 0 to the graph's radius - 1
            std::uniform_real_distribution<double> angle(0, std::numbers::pi * 2);           // 2 * pi

            for (int i = 0; i < render.getGraph()->nbrPoints; ++i)
            {
                render.getGraph()->pointList.emplace_back(
                    rayon(eng) * std::cos(angle(eng)), // Random x coordinate
                    rayon(eng) * std::sin(angle(eng))  // Random y coordinate
                );                                     // Generate random points in a circle of radius 10
            }

            // add border points to the graph
            int nbrBorderPoints = 20; // Number of border points

            for (int i = 0; i < nbrBorderPoints; ++i)
            {
                float angle = static_cast<float>(i) * (2.f * std::numbers::pi / static_cast<float>(nbrBorderPoints)); // Calculate the angle for each point
                render.getGraph()->pointList.emplace_back(
                    render.getGraph()->radius * std::cos(angle), // x coordinate
                    render.getGraph()->radius * std::sin(angle)  // y coordinate
                );
                render.getGraph()->idxPointBorder.push_back(static_cast<int>(render.getGraph()->pointList.size() - 1)); // Add the index of the border point to the list
                                                                                                                        // Generate points on the boundary of the circle
            }

            render.getGraph()->doDelaunayAndCalculateCenters(); // Perform Delaunay triangulation and calculate centers

            v.clear(); // Clear the vertex vector
            for (int i = 0; i < render.getGraph()->pointList.size(); ++i)
            {
                v.push_back({glm::vec3{render.getGraph()->pointList[i].first - factorTriangle, render.getGraph()->pointList[i].second - factorTriangle, 0.f}, RED, {}});
                v.push_back({glm::vec3{render.getGraph()->pointList[i].first + factorTriangle, render.getGraph()->pointList[i].second - factorTriangle, 0.f}, RED, {}});
                v.push_back({glm::vec3{render.getGraph()->pointList[i].first, render.getGraph()->pointList[i].second + factorTriangle, 0.f}, RED, {}});
            }
            triangleObject.set_vertex_data(v); // Set the vertex data for the triangle object

            v.clear(); // Clear the vertex vector for circumcenters
            for (int i = 0; i < render.getGraph()->nearCellulePointsList.size(); ++i)
            {
                v.push_back({glm::vec3{render.getGraph()->nearCellulePointsList[i].first - factorTriangle2, render.getGraph()->nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
                v.push_back({glm::vec3{render.getGraph()->nearCellulePointsList[i].first + factorTriangle2, render.getGraph()->nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}});
                v.push_back({glm::vec3{render.getGraph()->nearCellulePointsList[i].first, render.getGraph()->nearCellulePointsList[i].second + factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
            }
            triangleObjectVoro.set_vertex_data(v); // Set the vertex data for the circumcenter object

            render.resetCentralisation();    // Reset the centralisation counter
            render.nbrPointsChanged = false; // Reset the flag after updating the points
        }

        render.render2D(); // Render the ImGui interface

        if (render.get_itrCentralisation() > 0)
        {
            if (render.trueDelaunay)
            {
                render.getGraph()->centralisation();                // Centralize the points in the graph
                render.getGraph()->doDelaunayAndCalculateCenters(); // Perform Delaunay triangulation and calculate centers
            }
            else if (render.flipDelaunay)
            {
                if (nbrFlips == 0)
                    render.getGraph()->centralisation();            // Centralize the points in the graph
                render.getGraph()->doDelaunayFlipVersion(nbrFlips); // Flip triangles
            }

            v.clear(); // Clear the vertex vector
            for (int i = 0; i < render.getGraph()->pointList.size(); ++i)
            {
                v.push_back({glm::vec3{render.getGraph()->pointList[i].first - factorTriangle, render.getGraph()->pointList[i].second - factorTriangle, 0.f}, RED, {}});
                v.push_back({glm::vec3{render.getGraph()->pointList[i].first + factorTriangle, render.getGraph()->pointList[i].second - factorTriangle, 0.f}, RED, {}});
                v.push_back({glm::vec3{render.getGraph()->pointList[i].first, render.getGraph()->pointList[i].second + factorTriangle, 0.f}, RED, {}});
            }
            triangleObject.set_vertex_data(v); // Set the vertex data for the triangle object

            v.clear(); // Clear the vertex vector for circumcenters
            for (int i = 0; i < render.getGraph()->nearCellulePointsList.size(); ++i)
            {
                v.push_back({glm::vec3{render.getGraph()->nearCellulePointsList[i].first - factorTriangle2, render.getGraph()->nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
                v.push_back({glm::vec3{render.getGraph()->nearCellulePointsList[i].first + factorTriangle2, render.getGraph()->nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}});
                v.push_back({glm::vec3{render.getGraph()->nearCellulePointsList[i].first, render.getGraph()->nearCellulePointsList[i].second + factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
            }
            triangleObjectVoro.set_vertex_data(v); // Set the vertex data for the circumcenter object

            render.decrease_itrCentralisation();
            render.getGraph()->nbrCentralisation++; // Increment the number of centralisations applied

            double energieTotal = 0.0; // Initialize the total energy
            for (int i = 0; i < render.getGraph()->pointList.size(); ++i)
            {
                Point p        = render.getGraph()->pointList[i];                    // Get the current point
                float distance = std::sqrt(p.first * p.first + p.second * p.second); // Calculate the distance from the origin
                if (distance < render.getGraph()->radius * 0.8f)
                {
                    energieTotal += render.getGraph()->calcul_CVT_energie(i); // Calculate the energy for the current point
                }
            }

            if (render.getGraph()->energies.size() > 0 && energieTotal > 2 * render.getGraph()->energies.back())
                render.getGraph()->energies.emplace_back(render.getGraph()->energies.back()); // Store the total energy in the energies vector
            else
                render.getGraph()->energies.emplace_back(energieTotal); // Store the total energy in the energies vector
        }

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
