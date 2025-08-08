#pragma once

#include <imgui.h>
#include "LlyodCentralisation.hpp"
#include "trackball/TrackBall.hpp"

class App {
private:
    ImGuiIO& io;

    Graphe graphe; // Instance of the Graphe class to hold points and adjacencies

    TrackballCamera camera{10.f, 0.f, 0.f}; // Initialize the camera with a distance of 10, angleX of 45 degrees, and angleY of 0 degrees
    int             itrCentralisation = 0;  // Counter for the number of centralisations applied

    GLobject* triangleObject{};     // Object to hold the triangles
    GLobject* triangleObjectVoro{}; // Object to hold the Voronoi triangles

    bool drawPoints        = true;  // Flag to control whether to draw balls in the render
    bool drawTriangles     = true;  // Flag to control whether to draw triangles in the render
    bool drawVertex        = true;  // Flag to control whether to draw vertices in the render
    bool drawCelluleBorder = false; // Flag to control whether to draw the border of the cells
    bool mousePoint        = false;
    bool nbrPointsChanged  = false;
    bool drawCircles       = false; // Flag to control whether to draw circles in the render
    bool drawOrientedBox   = false; // Flag to control whether to draw the oriented bounding box

    bool trueDelaunay = true;
    bool flipDelaunay = false;

    float factorTriangle  = 0.15f; // Factor to scale the triangles
    float factorTriangle2 = 0.07f; // Factor to scale the circumcenters

    std::vector<Vertex> v;

    // std::vector<glm::vec3> randomPositions;

public:
    App()
        : io(ImGui::GetIO()) {};

    int get_itrCentralisation() const
    {
        return this->itrCentralisation; // Provide a way to access the current centralisation iteration
    }

    void decrease_itrCentralisation()
    {
        if (itrCentralisation > 0)
        {
            itrCentralisation--; // Decrease the centralisation counter
        }
    }

    void update(GLFWwindow* window, Program& program);
    void init();

    void render2D();

    Graphe* getGraph()
    {
        return &this->graphe; // Provide a way to access the graph
    }

    TrackballCamera* getCamera()
    {
        return &this->camera; // Provide a way to access the camera
    }

    void resetCentralisation()
    {
        itrCentralisation        = 0; // Reset the centralisation counter
        graphe.nbrCentralisation = 0; // Reset the number of centralisations in the graph
    }

    int getItrCentralisation() const
    {
        return itrCentralisation; // Provide a way to access the current centralisation iteration
    }

    std::vector<Vertex> getVertices() const
    {
        return v; // Provide a way to access the vertices
    }

    void setTriangleObject(GLobject* obj)
    {
        triangleObject = obj;
    }

    void setTriangleObjectVoro(GLobject* obj)
    {
        triangleObjectVoro = obj;
    }
};