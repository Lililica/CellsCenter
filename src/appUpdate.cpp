#include <math.h>
#include <numbers>
#include <random>
#include "imguiRender.hpp"
#include "utils.hpp"

void App::update(GLFWwindow* window, Program& program)
{
    int width  = 0;
    int height = 0;
    glfwGetWindowSize(window, &width, &height);

    int nbrFlips = 1; // Initialize the number of flips

    // get the mouse position
    if (mousePoint)
    {
        double mouseX = NAN;
        double mouseY = NAN;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        graphe.pointList[0].first  = (static_cast<float>(mouseX) / static_cast<float>(width)) * 30 - 15.; // Convert mouse position to OpenGL coordinates
        graphe.pointList[0].second = (static_cast<float>(height - mouseY) / height) * 30 - 15.;           // Convert mouse position to OpenGL coordinates
        // position[0].x                          = graphe.pointList[0].first;
        // position[0].y                          = graphe.pointList[0].second;
        // position[0].z                          = 0.f; // Set z to 0 for 2D points
    }

    if (drawTriangles)
    {
        std::vector<Vertex> vertices;                     // Create a vector to hold the vertices of the triangles
        glm::vec3           currentColor = BLUE;          // Color for the triangles
        vertices.reserve(graphe.idxTriangles.size() * 3); // Reserve space
        for (const auto& triangle : graphe.idxTriangles)
        {
            Point p1 = graphe.pointList[triangle[0]];                                    // Get the first point of the triangle
            Point p2 = graphe.pointList[triangle[1]];                                    // Get the second point of the triangle
            Point p3 = graphe.pointList[triangle[2]];                                    // Get the third point of the triangle
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

    if (drawCelluleBorder)
    {
        for (const auto& list : graphe.nearCellulePointsTriees) // Get the list of segments formed by the near cell points
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

    if (drawCircles)
    {
        for (const auto& circle : graphe.triangleCircles) // Iterate through each circle
        {
            std::vector<Vertex> vertices;                    // Create a vector to hold the vertices of the circle
            glm::vec3           currentColor = BLACK;        // Color for the circle
            vertices.reserve(graphe.triangleCircles.size()); // Reserve space for the circle vertices
            int nbrIterations = 50;                          // Number of iterations for drawing the circle

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
    if (drawOrientedBox)
    {
        for (const auto& orientedBox : graphe.allOrientedBoxes) // Iterate through each oriented bounding box
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

    if (drawPoints)
    {
        draw_ball(&camera, *triangleObject, program, window); // Draw the sphere at each point
    }
    if (drawVertex)
    {
        draw_ball(&camera, *triangleObjectVoro, program, window); // Draw the center sphere at each circumcenter
    }

    if (nbrPointsChanged)
    {
        graphe.pointList.clear();                   // Clear the previous points
        graphe.nearCellulePointsList.clear();       // Clear the previous circumcenters
        graphe.idxPointBorder.clear();              // Clear the previous border points
        graphe.pointList.reserve(graphe.nbrPoints); // Reserve space for new points
        graphe.allCircles.clear();                  // Clear the list of circles
        graphe.energies.clear();                    // Clear the list of energies
        std::default_random_engine             eng(std::random_device{}());
        std::uniform_real_distribution<double> rayon(0, graphe.radius - 5.);   // Random radius from 0 to the graph's radius - 1
        std::uniform_real_distribution<double> angle(0, std::numbers::pi * 2); // 2 * pi

        for (int i = 0; i < graphe.nbrPoints; ++i)
        {
            graphe.pointList.emplace_back(
                rayon(eng) * std::cos(angle(eng)), // Random x coordinate
                rayon(eng) * std::sin(angle(eng))  // Random y coordinate
            );                                     // Generate random points in a circle of radius 10
        }

        // add border points to the graph
        int nbrBorderPoints = 20; // Number of border points

        for (int i = 0; i < nbrBorderPoints; ++i)
        {
            float angle = static_cast<float>(i) * (2.f * std::numbers::pi / static_cast<float>(nbrBorderPoints)); // Calculate the angle for each point
            graphe.pointList.emplace_back(
                graphe.radius * std::cos(angle), // x coordinate
                graphe.radius * std::sin(angle)  // y coordinate
            );
            graphe.idxPointBorder.push_back(static_cast<int>(graphe.pointList.size() - 1)); // Add the index of the border point to the list
                                                                                            // Generate points on the boundary of the circle
        }

        graphe.doDelaunayAndCalculateCenters(); // Perform Delaunay triangulation and calculate centers

        v.clear(); // Clear the vertex vector
        for (int i = 0; i < graphe.pointList.size(); ++i)
        {
            v.push_back({glm::vec3{graphe.pointList[i].first - factorTriangle, graphe.pointList[i].second - factorTriangle, 0.f}, RED, {}});
            v.push_back({glm::vec3{graphe.pointList[i].first + factorTriangle, graphe.pointList[i].second - factorTriangle, 0.f}, RED, {}});
            v.push_back({glm::vec3{graphe.pointList[i].first, graphe.pointList[i].second + factorTriangle, 0.f}, RED, {}});
        }
        triangleObject->set_vertex_data(v); // Set the vertex data for the triangle object

        v.clear(); // Clear the vertex vector for circumcenters
        for (int i = 0; i < graphe.nearCellulePointsList.size(); ++i)
        {
            v.push_back({glm::vec3{graphe.nearCellulePointsList[i].first - factorTriangle2, graphe.nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
            v.push_back({glm::vec3{graphe.nearCellulePointsList[i].first + factorTriangle2, graphe.nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}});
            v.push_back({glm::vec3{graphe.nearCellulePointsList[i].first, graphe.nearCellulePointsList[i].second + factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
        }
        triangleObjectVoro->set_vertex_data(v); // Set the vertex data for the circumcenter object

        resetCentralisation();    // Reset the centralisation counter
        nbrPointsChanged = false; // Reset the flag after updating the points
    }

    render2D(); // Render the ImGui interface

    if (get_itrCentralisation() > 0)
    {
        if (trueDelaunay)
        {
            graphe.centralisation();                // Centralize the points in the graph
            graphe.doDelaunayAndCalculateCenters(); // Perform Delaunay triangulation and calculate centers
        }
        else if (flipDelaunay)
        {
            if (nbrFlips == 0)
                graphe.centralisation();            // Centralize the points in the graph
            graphe.doDelaunayFlipVersion(nbrFlips); // Flip triangles
        }

        v.clear(); // Clear the vertex vector
        for (int i = 0; i < graphe.pointList.size(); ++i)
        {
            v.push_back({glm::vec3{graphe.pointList[i].first - factorTriangle, graphe.pointList[i].second - factorTriangle, 0.f}, RED, {}});
            v.push_back({glm::vec3{graphe.pointList[i].first + factorTriangle, graphe.pointList[i].second - factorTriangle, 0.f}, RED, {}});
            v.push_back({glm::vec3{graphe.pointList[i].first, graphe.pointList[i].second + factorTriangle, 0.f}, RED, {}});
        }
        triangleObject->set_vertex_data(v); // Set the vertex data for the triangle object

        v.clear(); // Clear the vertex vector for circumcenters
        for (int i = 0; i < graphe.nearCellulePointsList.size(); ++i)
        {
            v.push_back({glm::vec3{graphe.nearCellulePointsList[i].first - factorTriangle2, graphe.nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
            v.push_back({glm::vec3{graphe.nearCellulePointsList[i].first + factorTriangle2, graphe.nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}});
            v.push_back({glm::vec3{graphe.nearCellulePointsList[i].first, graphe.nearCellulePointsList[i].second + factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
        }
        triangleObjectVoro->set_vertex_data(v); // Set the vertex data for the circumcenter object

        decrease_itrCentralisation();
        graphe.nbrCentralisation++; // Increment the number of centralisations applied

        double energieTotal = 0.0; // Initialize the total energy
        for (int i = 0; i < graphe.pointList.size(); ++i)
        {
            Point p        = graphe.pointList[i];                                // Get the current point
            float distance = std::sqrt(p.first * p.first + p.second * p.second); // Calculate the distance from the origin
            if (distance < graphe.radius * 0.8f)
            {
                energieTotal += graphe.calcul_CVT_energie(i); // Calculate the energy for the current point
            }
        }

        if (graphe.energies.size() > 0 && energieTotal > 2 * graphe.energies.back())
            graphe.energies.emplace_back(graphe.energies.back()); // Store the total energy in the energies vector
        else
            graphe.energies.emplace_back(energieTotal); // Store the total energy in the energies vector
    }
}

void App::init()
{
    // randomPositions = extract_point_from_obj(ASSETS_PATH + std::string{"cow.obj"});          // Extract random positions from the OBJ file
    // save_text_from_vectObj(randomPositions, ASSETS_PATH + std::string{"result/result.txt"}); // Save the positions to a text file

    std::vector<dt::Vector2<double>> points;

#if 0
    int numberPoints = 500;

    std::default_random_engine             eng(std::random_device{}());
    std::uniform_real_distribution<double> rayon(0, render.graphe.radius - 1.); // Random radius from 0 to the graph's radius
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
        graphe.pointList.emplace_back(
            graphe.radius * std::cos(angle),                                            // x coordinate
            graphe.radius * std::sin(angle)                                             // y coordinate
        );                                                                              // Generate points on the boundary of the circle
        graphe.idxPointBorder.push_back(static_cast<int>(graphe.pointList.size() - 1)); // Add the index of the border point to the list
    }

    graphe.pointList.reserve(points.size()); // Reserve space for points in the graph
    for (const auto& point : points)
    {
        graphe.pointList.emplace_back(point.x, point.y); // Convert dt::Vector2 to Point
    }

    // graphe.pointList = load_text_to_pointList(ASSETS_PATH + std::string{"pointExemple/pointListV0.txt"}); // Load points from a text file

    graphe.doDelaunayAndCalculateCenters(); // Perform Delaunay triangulation and calculate circumcenters

    // std::cout << "Number of points: " << graphe.pointList.size() / 2 << std::endl;

    v.clear(); // Clear the vertex vector
    for (int i = 0; i < graphe.pointList.size(); ++i)
    {
        v.push_back({glm::vec3{graphe.pointList[i].first - factorTriangle, graphe.pointList[i].second - factorTriangle, 0.f}, RED, {}});
        v.push_back({glm::vec3{graphe.pointList[i].first + factorTriangle, graphe.pointList[i].second - factorTriangle, 0.f}, RED, {}});
        v.push_back({glm::vec3{graphe.pointList[i].first, graphe.pointList[i].second + factorTriangle, 0.f}, RED, {}});
    }
    triangleObject->set_vertex_data(v); // Set the vertex data for the triangle object

    v.clear(); // Clear the vertex vector for circumcenters
    for (int i = 0; i < graphe.nearCellulePointsList.size(); ++i)
    {
        v.push_back({glm::vec3{graphe.nearCellulePointsList[i].first - factorTriangle2, graphe.nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
        v.push_back({glm::vec3{graphe.nearCellulePointsList[i].first + factorTriangle2, graphe.nearCellulePointsList[i].second - factorTriangle2, 0.f}, BLACK, {}});
        v.push_back({glm::vec3{graphe.nearCellulePointsList[i].first, graphe.nearCellulePointsList[i].second + factorTriangle2, 0.f}, BLACK, {}}); // Add circumcenter vertices
    }
    triangleObjectVoro->set_vertex_data(v); // Set the vertex data for the circumcenter object
}