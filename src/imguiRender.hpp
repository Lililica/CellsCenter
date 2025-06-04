

#include <imgui.h>
#include "trackball/TrackBall.hpp"
class Render {
private:
    ImGuiIO& io;

    TrackballCamera camera{10.f, 45.f, 0.f}; // Initialize the camera with a distance of 10, angleX of 45 degrees, and angleY of 0 degrees

public:
    Render()
        : io(ImGui::GetIO()) {};

    void render2D();

    TrackballCamera* getCamera()
    {
        return &this->camera; // Provide a way to access the camera
    }
};