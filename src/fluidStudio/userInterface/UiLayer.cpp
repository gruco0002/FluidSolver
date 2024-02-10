#include "UiLayer.hpp"

#include <imgui_internal.h>


namespace FluidStudio {

    UiLayer::~UiLayer() {
        window = nullptr;
    }

    void UiLayer::render() {
        simulation_controls_window.update();
        main_window_menu.update();
        ply_import_window.update();
        log_window.update();
        timeline_window.update();
        sensor_graph_windows.update();
        component_panel_window.update();
        component_settings_window.update();
        obj_import_window.update();
        new_simulation_modal_window.update();
        insert_particles_window.update();
    }

    void UiLayer::initialize() {
        UiData data(window, ui_element_collection, this);

        // add all elements to the collection
        ui_element_collection.add(simulation_controls_window);
        ui_element_collection.add(main_window_menu);
        ui_element_collection.add(ply_import_window);
        ui_element_collection.add(log_window);
        ui_element_collection.add(timeline_window);
        ui_element_collection.add(sensor_graph_windows);
        ui_element_collection.add(component_panel_window);
        ui_element_collection.add(component_settings_window);
        ui_element_collection.add(obj_import_window);
        ui_element_collection.add(new_simulation_modal_window);
        ui_element_collection.add(insert_particles_window);

        // initialize all elements
        simulation_controls_window.initialize(data);
        main_window_menu.initialize(data);
        ply_import_window.initialize(data);
        log_window.initialize(data);
        timeline_window.initialize(data);
        sensor_graph_windows.initialize(data);
        component_panel_window.initialize(data);
        component_settings_window.initialize(data);
        obj_import_window.initialize(data);
        new_simulation_modal_window.initialize(data);
        insert_particles_window.initialize(data);
    }

    void UiLayer::setupDockingLayout(ImGuiID mainDockspaceId) {
        if (docking_setup_already_run) return;
        docking_setup_already_run = true;

        // setup layout
        ImGui::DockBuilderRemoveNode(
                mainDockspaceId);
        ImGui::DockBuilderAddNode(mainDockspaceId);

        // create docking layout
        auto leftSideDock = ImGui::DockBuilderSplitNode(mainDockspaceId, ImGuiDir_Left, 0.2f, nullptr,
                                                        &mainDockspaceId);
        auto rightDock = ImGui::DockBuilderSplitNode(mainDockspaceId, ImGuiDir_Right, 0.2f, nullptr, &mainDockspaceId);
        auto mainDock = ImGui::DockBuilderSplitNode(mainDockspaceId, ImGuiDir_Right, 0.6f, nullptr, &mainDockspaceId);

        auto bottomDock = ImGui::DockBuilderSplitNode(mainDock, ImGuiDir_Down, 0.2f, nullptr, &mainDock);
        auto topLeftDock = ImGui::DockBuilderSplitNode(leftSideDock, ImGuiDir_Up, 0.1, nullptr, &leftSideDock);



        // attach windows to docking layouts
        ImGui::DockBuilderDockWindow("Simulation Controls", topLeftDock);
        ImGui::DockBuilderDockWindow("Components", leftSideDock);

        ImGui::DockBuilderDockWindow("Simulation Visualization", mainDock);
        ImGui::DockBuilderDockWindow("Editor Visualization", mainDock);

        ImGui::DockBuilderDockWindow("Properties", rightDock);

        ImGui::DockBuilderDockWindow("Logs", bottomDock);
        ImGui::DockBuilderDockWindow("Timeline", bottomDock);



        // finish layout
        ImGui::DockBuilderFinish(mainDockspaceId);
    }

    void UiLayer::resetDockingLayout() {
        docking_setup_already_run = false;
    }

    UiLayer::UiLayer() = default;


} // namespace FluidStudio