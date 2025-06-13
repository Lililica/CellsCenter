#include "imguiRender.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

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
        itrCentralisation += 1; // Set the counter to 1 for centralisation
    }
    if (ImGui::Button("Apply Centralisation x10"))
    {
        itrCentralisation += 10; // Set the counter to 10 for centralisation
    }

    if (ImGui::Button("Apply Centralisation x100"))
    {
        itrCentralisation += 100; // Set the counter to 100 for centralisation
    }

    if (ImGui::Button("Apply Centralisation x1000"))
    {
        itrCentralisation += 1000; // Set the counter to 1000 for centralisation
    }

    ImGui::End();

    ImGui::Begin("Graph Parameters");
    ImGui::Text("Number of centralisations applied: %d", graphe.nbrCentralisation);
    ImGui::Text("Number of points in the graph: %zu", graphe.pointList.size());
    ImGui::Text("Number of triangles: %zu", graphe.trianglesPoints.size());
    if (ImGui::Button("Show Triangles"))
    {
        drawTriangles = !drawTriangles; // Toggle the drawing of triangles
    }

    if (ImGui::Button("Show Points"))
    {
        drawPoints = !drawPoints; // Toggle the drawing of points
    }

    if (ImGui::Button("Show Vertex"))
    {
        drawVertex = !drawVertex; // Toggle the drawing of vertices
    }

    ImGui::SliderFloat("Delta for centralisation", &graphe.step, 0.f, 1.f);

    if (ImGui::Button("Point from mouse positiion"))
    {
        mousePoint = !mousePoint; // Toggle the drawing of vertices
    }

    int previousNbrPoints = graphe.nbrPoints; // Store the previous number of points
    ImGui::SliderInt("Nbr of points", &graphe.nbrPoints, 10, 1000);
    if (previousNbrPoints != graphe.nbrPoints)
    {
        nbrPointsChanged = true; // Set the flag to true if the number of points has changed
    }

    ImGui::End();

    if (itrCentralisation > 0)
    {
        graphe.centralisation(); // Centralize the points in the graph
        for (int i = 0; i < graphe.pointList.size(); ++i)
        {
            position[i].x = graphe.pointList[i].first;
            position[i].y = graphe.pointList[i].second;
            position[i].z = 0.f; // Set z to 0 for 2D points
        }

        itrCentralisation--;
        graphe.nbrCentralisation++; // Increment the number of centralisations applied
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

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}