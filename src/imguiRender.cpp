#include "imguiRender.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>
#include <random>
#include "utils.hpp"

void Render::render2D()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Begin("Centralisation Control");

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

    if (ImGui::Button("Stop Centralisation"))
    {
        itrCentralisation = 1; // Set the counter to 1000 for centralisation
    }

    ImGui::Text("Current centralisation method: %s", graphe.useWelzl ? "Welzl Circle" : (graphe.useCentroid ? "Centroid" : "Square"));

    if (ImGui::Button("Switch to Square"))
    {
        graphe.nbrCentralisation = 0;     // Reset the number of centralisations
        itrCentralisation        = 0;     // Reset the centralisation counter
        graphe.useWelzl          = false; // Toggle the use of Welzl's algorithm
        graphe.useCentroid       = false; // Toggle the use of centroid calculation
        graphe.useSquare         = true;  // Toggle the use of
        graphe.kNearest          = false; // Toggle the use of k-nearest neighbors
    }
    if (ImGui::Button("Switch to Welzl Circle"))
    {
        graphe.nbrCentralisation = 0;     // Reset the number of centralisations
        itrCentralisation        = 0;     // Reset the centralisation counter
        graphe.useWelzl          = true;  // Toggle the use of Welzl's algorithm
        graphe.useCentroid       = false; // Toggle the use of centroid calculation
        graphe.useSquare         = false; // Toggle the use of square calculation
        graphe.kNearest          = false; // Toggle the use of k-nearest neighbors
    }
    if (ImGui::Button("Switch to Centroid"))
    {
        graphe.nbrCentralisation = 0;     // Reset the number of centralisations
        itrCentralisation        = 0;     // Reset the centralisation counter
        graphe.useWelzl          = false; // Toggle the use of Welzl's algorithm
        graphe.useCentroid       = true;  // Toggle the use of centroid calculation
        graphe.useSquare         = false; // Toggle the use of square calculation
        graphe.kNearest          = false; // Toggle the use of k-nearest neighbors
    }
    if (ImGui::Button("Switch to k-nearest"))
    {
        graphe.nbrCentralisation = 0;     // Reset the number of centralisations
        itrCentralisation        = 0;     // Reset the centralisation counter
        graphe.useWelzl          = false; // Toggle the use of Welzl's algorithm
        graphe.useCentroid       = false; // Toggle the use of centroid calculation
        graphe.useSquare         = false; // Toggle the use of square calculation
        graphe.kNearest          = true;  // Toggle the use of k-nearest neighbors
    }

    ImGui::Text("Center of Point 0 with Welzl Circle : ");
    ImGui::Text("(%f, %f)", graphe.welzlCenterOf0.first, graphe.welzlCenterOf0.second);
    ImGui::Text("Center of Point 0 with Centroid : ");
    ImGui::Text("(%f, %f)", graphe.centroidCenterOf0.first, graphe.centroidCenterOf0.second);

    ImGui::End();

    ImGui::Begin("Graph Parameters");
    ImGui::Text("Number of centralisations applied: %d", graphe.nbrCentralisation);
    ImGui::Text("Number of points in the graph: %zu", graphe.pointList.size());
    ImGui::Text("Number of triangles: %zu", graphe.trianglesPoints.size());
    if (ImGui::Button("Show Triangles"))
    {
        drawTriangles = !drawTriangles; // Toggle the drawing of triangles
    }

    if (ImGui::Button("Show Cells"))
    {
        drawCelluleBorder = !drawCelluleBorder; // Toggle the drawing of centers
    }

    if (ImGui::Button("Show Points"))
    {
        drawPoints = !drawPoints; // Toggle the drawing of points
    }

    if (ImGui::Button("Show Vertex"))
    {
        drawVertex = !drawVertex; // Toggle the drawing of vertices
    }

    if (ImGui::Button("Show Circles"))
    {
        drawCircles = !drawCircles; // Toggle the drawing of circles
    }

    ImGui::SliderFloat("Delta for centralisation", &graphe.step, 0.f, 1.f);

    if (ImGui::Button("Point from mouse positiion"))
    {
        mousePoint = !mousePoint; // Toggle the drawing of vertices
    }

    int previousNbrPoints = graphe.nbrPoints; // Store the previous number of points
    ImGui::SliderInt("Nbr of points", &graphe.nbrPoints, 10, 5000);
    if (previousNbrPoints != graphe.nbrPoints)
    {
        nbrPointsChanged              = true; // Set the flag to true if the number of points has changed
        graphe.currentCVTEnergie      = 0.f;  // Reset the current CVT energy
        graphe.currentIdxEnergiePoint = 0;    // Reset the index of the point for which we want to calculate the CVT energy
    }

    ImGui::End();

    ImGui::Begin("CVT Energie");
    if (itrCentralisation == 0)
    {
        ImGui::Text("Choose a new point :");
        if (ImGui::Button("New Point"))
        {
            while (true)
            {
                // Select a random index from the pointList using C++11 random library
                static std::random_device             rd;
                static std::mt19937                   gen(rd());
                std::uniform_int_distribution<size_t> distrib(0, graphe.pointList.size() - 1);
                graphe.currentIdxEnergiePoint = static_cast<int>(distrib(gen));

                if (std::find(graphe.idxPointBorder.begin(), graphe.idxPointBorder.end(), graphe.currentIdxEnergiePoint) != graphe.idxPointBorder.end())
                {
                    continue;
                }
                break;
            }

            graphe.currentCVTEnergie = graphe.calcul_CVT_energie(graphe.currentIdxEnergiePoint); // Calculate the CVT energy for the selected point
        }
        ImGui::Text("Current point: (%f, %f)", graphe.pointList[graphe.currentIdxEnergiePoint].first, graphe.pointList[graphe.currentIdxEnergiePoint].second);
        ImGui::Text("Current CVT Energie: %f", graphe.currentCVTEnergie);
    }
    else
    {
        ImGui::Text("Waiting the centralisation is finished...");
    }

    if (ImGui::Button("Save Energies") && itrCentralisation == 0)
    {
        save_energies_to_csv(graphe.energies, ASSETS_PATH + std::string{"PointExemple/pointListV0kNear4.csv"}); // Save the energies to a CSV file
        ImGui::Text("Energies saved to CVTenergie.csv");
    }

    if (itrCentralisation == 1)
    {
        graphe.currentCVTEnergie = graphe.calcul_CVT_energie(graphe.currentIdxEnergiePoint); // Calculate the CVT energy for the selected point
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
