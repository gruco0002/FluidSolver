#include "UiLayer.hpp"


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
        UiData data(window, ui_element_collection);

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

    UiLayer::UiLayer() = default;


} // namespace FluidStudio