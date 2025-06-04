#include "imguiRender.hpp"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

void Render::render2D()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::Begin("Camera");

    if (ImGui::Button("Reset Camera"))
    {
        camera.set_to(glm::vec3(0.f, 0.f, 10.f)); // Reset camera to a position looking at the origin
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}