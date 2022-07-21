#include "ImguiHelper.hpp"

#include "DirectoryHelper.hpp"
#include "font/RobotoMedium.cpp"

#include <GLFW/glfw3.h>

void ImGuiHelper::Init(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    auto context = ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuizmo::SetImGuiContext(context);


    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.IniFilename = FluidStudio::DirectoryHelper::imgui_config_file();

    // taking high-dpi displays into account
    float scaleX, scaleY;
    glfwGetWindowContentScale(window, &scaleX, &scaleY);
    float avgScale = (scaleX + scaleY) / 2.0f;

#ifdef __APPLE__
    avgScale = 1.0f;
#endif


    io.Fonts->AddFontFromMemoryCompressedTTF(RobotoMedium_compressed_data, RobotoMedium_compressed_size, 15 * avgScale);


    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGui::GetStyle().ScaleAllSizes(avgScale);
}

void ImGuiHelper::Uninit() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void ImGuiHelper::PreRender() {
    // feed inputs to dear imgui, start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiHelper::PostRender() {
    // Render dear imgui into screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
