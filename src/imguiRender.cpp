#include "imguiRender.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

void Render::render2D(std::vector<glm::vec3>& position, std::vector<glm::vec3>& positionCENTRE)
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Begin("Camera");

    if (ImGui::Button("Reset Camera"))
    {
        camera.set_to(glm::vec3(20.f, 0.f, 0.f)); // Reset camera to a position looking at the origin
    }

    if (ImGui::Button("Apply Centralisation"))
    {
        graphe.centralisation(); // Centralize the points in the graph
        for (int i = 0; i < graphe.pointList.size(); ++i)
        {
            position[i].x = graphe.pointList[i].first;
            position[i].y = graphe.pointList[i].second;
            position[i].z = 0.f; // Set z to 0 for 2D points
        }

        graphe.doDelaunayAndCalculateCenters();                     // Perform Delaunay triangulation and calculate centers
        positionCENTRE.clear();                                     // Clear the previous centers
        positionCENTRE.resize(graphe.nearCellulePointsList.size()); // Resize to the new number of centers
        for (int i = 0; i < graphe.nearCellulePointsList.size(); ++i)
        {
            positionCENTRE[i].x = graphe.nearCellulePointsList[i].first;
            positionCENTRE[i].y = graphe.nearCellulePointsList[i].second;
            positionCENTRE[i].z = 0.f; // Set z to 0 for 2D points
        }
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}