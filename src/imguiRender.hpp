#pragma once

#include <imgui.h>
#include "LlyodCentralisation.hpp"
#include "trackball/TrackBall.hpp"

class Render {
private:
    ImGuiIO& io;

    Graphe graphe; // Instance of the Graphe class to hold points and adjacencies

    TrackballCamera camera{20.f, 0.f, 0.f}; // Initialize the camera with a distance of 10, angleX of 45 degrees, and angleY of 0 degrees
    int             itrCentralisation = 0;  // Counter for the number of centralisations applied

public:
    bool drawPoints        = true;  // Flag to control whether to draw balls in the render
    bool drawTriangles     = true;  // Flag to control whether to draw triangles in the render
    bool drawVertex        = true;  // Flag to control whether to draw vertices in the render
    bool drawCelluleBorder = false; // Flag to control whether to draw the border of the cells
    bool mousePoint        = false;
    bool nbrPointsChanged  = false;

    Render()
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

    void render2D(std::vector<glm::vec3>& position, std::vector<glm::vec3>& positionCENTRE);

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
};