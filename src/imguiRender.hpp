#pragma once

#include <imgui.h>
#include "LlyodCentralisation.hpp"
#include "trackball/TrackBall.hpp"

class Render {
private:
    ImGuiIO& io;

    Graphe graphe; // Instance of the Graphe class to hold points and adjacencies

    TrackballCamera camera{20.f, 0.f, 0.f}; // Initialize the camera with a distance of 10, angleX of 45 degrees, and angleY of 0 degrees

public:
    Render()
        : io(ImGui::GetIO()) {};

    void render2D(std::vector<glm::vec3>& position, std::vector<glm::vec3>& positionCENTRE);

    Graphe* getGraph()
    {
        return &this->graphe; // Provide a way to access the graph
    }

    TrackballCamera* getCamera()
    {
        return &this->camera; // Provide a way to access the camera
    }
};