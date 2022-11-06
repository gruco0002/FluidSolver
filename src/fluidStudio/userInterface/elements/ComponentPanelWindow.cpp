#include "ComponentPanelWindow.hpp"

#include "ImguiHelper.hpp"
#include "entities/BoundaryPreprocessor.hpp"
#include "entities/ParticleRemover.hpp"
#include "entities/ParticleRemover3D.hpp"
#include "entities/ParticleSpawner.hpp"
#include "entities/VelocityAlterationByTag.hpp"
#include "sensors/CompressedNeighborsStatistics.hpp"
#include "sensors/IisphSensor.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorPlane.hpp"
#include "userInterface/helpers/TypeInformationProvider.hpp"
#include "visualizationOverlay/ParticleRemoverOverlay.hpp"
#include "visualizationOverlay/ParticleSelectionByTagOverlay.hpp"

namespace FluidStudio {


    void ComponentPanelWindow::update() {
        verify_selection_is_okay();

        if (ImGui::Begin("Components")) {
            // Draw components
            update_component_node("Solver", {SimulationComponent::Kind::Solver, 0});
            update_component_node("Timestep", {SimulationComponent::Kind::Timestep, 0});
            update_component_node("Output", {SimulationComponent::Kind::Output, 0});

            if (begin_structural_node("Visualizer")) {
                update_component_node("Editor", {SimulationComponent::Kind::EditorVisualizer, 0});
                update_component_node("Simulation", {SimulationComponent::Kind::SimulationVisualizer, 0});
                ImGui::TreePop();
            }


            if (begin_structural_node("Sensors")) {
                menu_sensor_names.resize(ui_data.window().simulator_visualizer_bundle.simulator->data.sensors.size());
                for (size_t i = 0; i < ui_data.window().simulator_visualizer_bundle.simulator->data.sensors.size(); i++) {
                    auto sen = ui_data.window().simulator_visualizer_bundle.simulator->data.sensors[i];

                    // set name
                    auto name = sen->parameters.name + ": " + TypeInformationProvider::get_sensor_type_name(sen) + " Sensor";
                    if (menu_sensor_names[i] != name) {
                        menu_sensor_names[i] = name;
                    }
                    update_component_node(menu_sensor_names[i].c_str(), {SimulationComponent::Kind::Sensor, i});
                }
                ImGui::TreePop();
            }

            if (begin_structural_node("Entities")) {
                menu_entity_names.resize(ui_data.window().simulator_visualizer_bundle.simulator->data.entities.size());
                for (size_t i = 0; i < ui_data.window().simulator_visualizer_bundle.simulator->data.entities.size(); i++) {
                    auto ent = ui_data.window().simulator_visualizer_bundle.simulator->data.entities[i];

                    // set name
                    auto name = "Entity " + std::to_string(i + 1) + ": " + TypeInformationProvider::get_entity_type_name(ent);
                    if (menu_entity_names[i] != name) {
                        menu_entity_names[i] = name;
                    }

                    update_component_node(menu_entity_names[i].c_str(), {SimulationComponent::Kind::Entity, i});
                }
                ImGui::TreePop();
            }

            if (begin_structural_node("Tag Descriptors")) {
                for (size_t i = 0; i < ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors.size(); i++) {
                    const auto& title = ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors[i].title;
                    update_component_node(title.c_str(), {SimulationComponent::Kind::TagDescriptor, i});
                }

                ImGui::TreePop();
            }

            // handle mouse click outside
            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                m_selection = {};

            // right click outside
            update_menu();
        }
        ImGui::End();

        update_selection_based_ui();
    }

    void ComponentPanelWindow::update_menu() {
        bool is_safe = ui_data.window().is_safe_to_access_simulation_data();
        if (ImGui::BeginPopupContextWindow(0, 1, false)) {
            if (ImGui::BeginMenu("Add Sensor")) {
                auto& data = ui_data.window().simulator_visualizer_bundle.simulator->data;
                if (ImGui::MenuItem("Global Density", nullptr, nullptr, is_safe)) {
                    auto sen = std::make_shared<LibFluid::Sensors::GlobalDensitySensor>();
                    sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                    data.sensors.push_back(sen);
                    data.notify_that_data_changed();
                }
                if (ImGui::MenuItem("Global Pressure", nullptr, nullptr, is_safe)) {
                    auto sen = std::make_shared<LibFluid::Sensors::GlobalPressureSensor>();
                    sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                    data.sensors.push_back(sen);
                    data.notify_that_data_changed();
                }
                if (ImGui::MenuItem("Global Velocity", nullptr, nullptr, is_safe)) {
                    auto sen = std::make_shared<LibFluid::Sensors::GlobalVelocitySensor>();
                    sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                    data.sensors.push_back(sen);
                    data.notify_that_data_changed();
                }
                if (ImGui::MenuItem("Global Energy", nullptr, nullptr, is_safe)) {
                    auto sen = std::make_shared<LibFluid::Sensors::GlobalEnergySensor>();
                    sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                    data.sensors.push_back(sen);
                    data.notify_that_data_changed();
                }
                if (ImGui::MenuItem("Global Particle Count", nullptr, nullptr, is_safe)) {
                    auto sen = std::make_shared<LibFluid::Sensors::GlobalParticleCountSensor>();
                    sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                    data.sensors.push_back(sen);
                    data.notify_that_data_changed();
                }
                if (ImGui::MenuItem("3D Sensor Plane", nullptr, nullptr, is_safe)) {
                    auto sen = std::make_shared<LibFluid::Sensors::SensorPlane>();
                    sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                    data.sensors.push_back(sen);
                    data.notify_that_data_changed();
                }
                if (ImGui::MenuItem("Compressed Neighborhood Storage", nullptr, nullptr, is_safe)) {
                    auto sen = std::make_shared<LibFluid::Sensors::CompressedNeighborStorageSensor>();
                    sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                    data.sensors.push_back(sen);
                    data.notify_that_data_changed();
                }
                if (ImGui::MenuItem("IISPH", nullptr, nullptr, is_safe)) {
                    auto sen = std::make_shared<LibFluid::Sensors::IISPHSensor>();
                    sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                    data.sensors.push_back(sen);
                    data.notify_that_data_changed();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Add Entity")) {
                auto& data = ui_data.window().simulator_visualizer_bundle.simulator->data;
                if (ImGui::MenuItem("Particle Remover 3D", nullptr, nullptr, is_safe)) {
                    auto ent = std::make_shared<LibFluid::ParticleRemover3D>();
                    ent->parameters.volume.center = glm::vec3(0.0f);
                    ent->parameters.volume.distance_from_center = glm::vec3(1.0f);
                    ent->parameters.remove_if_outside = true;
                    data.entities.push_back(ent);
                    data.notify_that_data_changed();
                }

                if (ImGui::MenuItem("Boundary Preprocessor", nullptr, nullptr, is_safe)) {
                    auto ent = std::make_shared<LibFluid::BoundaryPreprocessor>();
                    data.entities.push_back(ent);
                    data.notify_that_data_changed();
                }

                if (ImGui::MenuItem("Velocity Alteration by Tag", nullptr, nullptr, is_safe)) {
                    auto ent = std::make_shared<LibFluid::VelocityAlterationByTag>();
                    data.entities.push_back(ent);
                    data.notify_that_data_changed();
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Add Tag Descriptor", nullptr, nullptr, is_safe)) {
                auto& data = ui_data.window().simulator_visualizer_bundle.simulator->data;
                auto size = data.tag_descriptors->descriptors.size() + 1;
                auto name = "Tag Descriptor " + std::to_string(size);

                data.tag_descriptors->descriptors.push_back({name, "", 0});
                data.notify_that_data_changed();
            }

            ImGui::EndPopup();
        }
    }

    void ComponentPanelWindow::update_component_node(const char* name, const SimulationComponent& component) {
        ImGuiTreeNodeFlags flags = ((component == m_selection) ? ImGuiTreeNodeFlags_Selected : 0) |
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Bullet;

        bool opened = ImGui::TreeNodeEx((void*)((size_t)name + component.index * 1565412), flags, "%s", name);

        if (ImGui::IsItemClicked()) {
            m_selection = component;
        }


        bool component_deleted = false;

        if (component.can_delete()) {
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Delete Component"))
                    component_deleted = true;

                ImGui::EndPopup();
            }
        }

        if (opened) {
            ImGui::TreePop();
        }

        if (component_deleted) {
            // reset selection if required
            if (m_selection == component)
                m_selection = {};

            // remove component
            delete_component(component);
        }
    }


    void ComponentPanelWindow::delete_component(const SimulationComponent& component) {
        if (component.kind == SimulationComponent::Kind::Sensor) {
            // delete the sensor if existing

            // check for existance
            if (ui_data.window().simulator_visualizer_bundle.simulator->data.sensors.size() <= component.index) {
                return;
            }

            // delete the sensor
            for (size_t i = component.index; i < ui_data.window().simulator_visualizer_bundle.simulator->data.sensors.size() - 1; i++) {
                ui_data.window().simulator_visualizer_bundle.simulator->data.sensors[i] = ui_data.window().simulator_visualizer_bundle.simulator->data.sensors[i + 1];
            }
            ui_data.window().simulator_visualizer_bundle.simulator->data.sensors.pop_back();
            ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        } else if (component.kind == SimulationComponent::Kind::Entity) {
            // delete the entity if existing

            // check for existance
            if (ui_data.window().simulator_visualizer_bundle.simulator->data.entities.size() <= component.index) {
                return;
            }

            // delete the entity
            for (size_t i = component.index; i < ui_data.window().simulator_visualizer_bundle.simulator->data.entities.size() - 1; i++) {
                ui_data.window().simulator_visualizer_bundle.simulator->data.entities[i] = ui_data.window().simulator_visualizer_bundle.simulator->data.entities[i + 1];
            }
            ui_data.window().simulator_visualizer_bundle.simulator->data.entities.pop_back();
            ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        } else if (component.kind == SimulationComponent::Kind::TagDescriptor) {
            // delete the tag descriptor if existing

            auto& tag_descriptors = ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors;

            // check for existance
            if (tag_descriptors->descriptors.size() <= component.index) {
                return;
            }

            // delete the descriptor
            for (size_t i = component.index; i < tag_descriptors->descriptors.size() - 1; i++) {
                tag_descriptors->descriptors[i] = std::move(tag_descriptors->descriptors[i + 1]);
            }
            tag_descriptors->descriptors.pop_back();
            ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        }
    }


    void ComponentPanelWindow::update_selection_based_ui() {
        std::shared_ptr<OverlayInstance> new_overlay_instance = nullptr;
        auto current_overlay_instance = ui_data.window().visualization_overlay.data.overlay_instance;
        if (m_selection.kind == SimulationComponent::Kind::Entity) {
            // check if overlay needs to be activated / deactivated
            auto ent = ui_data.window().simulator_visualizer_bundle.simulator->data.entities[m_selection.index];

            auto remover3d = std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(ent);
            if (remover3d != nullptr) {
                // activate the overlay for the remover 3d
                auto overlay_casted = std::dynamic_pointer_cast<ParticleRemoverOverlay>(current_overlay_instance);
                if (overlay_casted != nullptr && overlay_casted->entity == ent) {
                    new_overlay_instance = current_overlay_instance;
                } else {
                    new_overlay_instance = std::make_shared<ParticleRemoverOverlay>(remover3d);
                }
            }
        } else if (m_selection.kind == SimulationComponent::Kind::TagDescriptor) {
            FLUID_ASSERT(m_selection.index < ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors.size());
            auto td = ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors[m_selection.index];

            auto overlay_casted = std::dynamic_pointer_cast<ParticleSelectionByTagOverlay>(current_overlay_instance);
            if (overlay_casted != nullptr && overlay_casted->get_particle_tag() == td.particle_tag) {
                new_overlay_instance = current_overlay_instance;
            } else {
                new_overlay_instance = std::make_shared<ParticleSelectionByTagOverlay>(ui_data.window().simulator_visualizer_bundle.simulator, td.particle_tag);
            }
        }

        if (new_overlay_instance != current_overlay_instance) {
            // only update on a change
            ui_data.window().visualization_overlay.set_new_overlay_instance(new_overlay_instance);
        }
    }
    const SimulationComponent& ComponentPanelWindow::selection() const {
        return m_selection;
    }

    bool ComponentPanelWindow::begin_structural_node(const char* name) {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
        return ImGui::TreeNodeEx(name, flags);
    }

    void ComponentPanelWindow::verify_selection_is_okay() {
        if (m_selection.kind == SimulationComponent::Kind::Entity) {
            if (m_selection.index >= ui_data.window().simulator_visualizer_bundle.simulator->data.entities.size()) {
                // invalid selection, sanitize
                m_selection.index = 0;
                m_selection.kind = SimulationComponent::Kind::None;
            }
        } else if (m_selection.kind == SimulationComponent::Kind::Sensor) {
            if (m_selection.index >= ui_data.window().simulator_visualizer_bundle.simulator->data.sensors.size()) {
                // invalid selection, sanitize
                m_selection.index = 0;
                m_selection.kind = SimulationComponent::Kind::None;
            }
        } else if (m_selection.kind == SimulationComponent::Kind::TagDescriptor) {
            if (ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors == nullptr ||
                    m_selection.index >= ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors.size()) {
                // invalid selection, sanitize
                m_selection.index = 0;
                m_selection.kind = SimulationComponent::Kind::None;
            }
        }
    }

} // namespace FluidStudio