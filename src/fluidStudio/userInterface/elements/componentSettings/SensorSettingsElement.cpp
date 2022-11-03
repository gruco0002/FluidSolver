#include "SensorSettingsElement.hpp"

#include "ImguiHelper.hpp"
#include "sensors/IisphSensor.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorPlane.hpp"
#include "userInterface/StyledImGuiElements.hpp"
#include "userInterface/elements/ComponentPanelWindow.hpp"
#include "userInterface/elements/SensorGraphWindows.hpp"
#include "userInterface/helpers/TypeInformationProvider.hpp"

namespace FluidStudio {


    void SensorSettingsElement::update() {
        auto& selection = ui_data.collection().get<ComponentPanelWindow>().selection();
        if (selection.kind != SimulationComponent::Kind::Sensor) {
            return;
        }
        auto index = selection.index;


        FLUID_ASSERT(index < ui_data.window().simulator_visualizer_bundle.simulator->data.sensors.size());
        auto sen = ui_data.window().simulator_visualizer_bundle.simulator->data.sensors[index];

        if (StyledImGuiElements::slim_tree_node("Sensor")) {
            ImGui::LabelText("Type", "%s Sensor", TypeInformationProvider::get_sensor_type_name(sen));
            if (ImGui::Button("Open Graph")) {
                ui_data.collection().get<SensorGraphWindows>().open_sensor_window(index);
            }
            ImGui::InputText("Name", &sen->parameters.name);
            ImGui::Checkbox("Keep Data in Memory", &sen->parameters.keep_data_in_memory);
            ImGui::Separator();
            ImGui::Checkbox("Save to File", &sen->parameters.save_to_file);
            ImGui::InputText("Filename", &sen->parameters.filename);

            ImGui::TreePop();
        }

        if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalEnergySensor>(sen)) {
            update_global_energy_sensor_component(std::dynamic_pointer_cast<LibFluid::Sensors::GlobalEnergySensor>(sen));
        } else if (std::dynamic_pointer_cast<LibFluid::Sensors::SensorPlane>(sen)) {
            update_sensor_plane_component(std::dynamic_pointer_cast<LibFluid::Sensors::SensorPlane>(sen));
        } else if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalParticleCountSensor>(sen)) {
            update_particle_count_sensor(sen);
        }
    }

    void SensorSettingsElement::update_global_energy_sensor_component(std::shared_ptr<LibFluid::Sensor> sen) {
        auto casted = std::dynamic_pointer_cast<LibFluid::Sensors::GlobalEnergySensor>(sen);

        if (StyledImGuiElements::slim_tree_node("Zero-Levels")) {
            ImGui::InputFloat("Zero Height", &casted->settings.relative_zero_height);

            ImGui::TreePop();
        }
    }

    void SensorSettingsElement::update_sensor_plane_component(std::shared_ptr<LibFluid::Sensor> tmp) {
        auto sen = std::dynamic_pointer_cast<LibFluid::Sensors::SensorPlane>(tmp);

        if (StyledImGuiElements::slim_tree_node("Value")) {
            using namespace LibFluid::Sensors;

            const char* current_type_name = nullptr;
            switch (sen->settings.sensor_type) {
                case SensorPlane::SensorPlaneType::SensorPlaneTypeDensity:
                    current_type_name = "Density";
                    break;
                case SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity:
                    current_type_name = "Velocity";
                    break;
                case SensorPlane::SensorPlaneType::SensorPlaneTypePressure:
                    current_type_name = "Pressure";
                    break;
                default:
                    current_type_name = "Unknown";
                    break;
            }

            if (ImGui::BeginCombo("Type", current_type_name)) {
                if (ImGui::Selectable("Density",
                            sen->settings.sensor_type == SensorPlane::SensorPlaneType::SensorPlaneTypeDensity)) {
                    sen->settings.sensor_type = SensorPlane::SensorPlaneType::SensorPlaneTypeDensity;
                }

                if (ImGui::Selectable("Velocity",
                            sen->settings.sensor_type == SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity)) {
                    sen->settings.sensor_type = SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity;
                }

                if (ImGui::Selectable("Pressure",
                            sen->settings.sensor_type == SensorPlane::SensorPlaneType::SensorPlaneTypePressure)) {
                    sen->settings.sensor_type = SensorPlane::SensorPlaneType::SensorPlaneTypePressure;
                }

                ImGui::EndCombo();
            }

            if (ImGui::InputInt("Calc. every nth step", (int*)&sen->settings.calculate_plane_every_nth_step)) {
                if (sen->settings.calculate_plane_every_nth_step == 0) {
                    sen->settings.calculate_plane_every_nth_step = 1;
                }
            }

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("Location & Size")) {
            ImGui::InputFloat3("Origin", (float*)&sen->settings.origin);
            ImGui::InputFloat3("Span X", (float*)&sen->settings.span_x);
            ImGui::InputFloat3("Span Y", (float*)&sen->settings.span_y);
            ImGui::InputFloat("Width", &sen->settings.width);
            ImGui::InputFloat("Height", &sen->settings.height);

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("Samples")) {
            ImGui::InputInt("X-Samples", (int*)&sen->settings.number_of_samples_x);
            ImGui::InputInt("Y-Samples", (int*)&sen->settings.number_of_samples_y);
            ImGui::InputInt("Sub-Samples", (int*)&sen->settings.sub_sample_grid_size);

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("Image")) {
            ImGui::InputFloat("Min. value", &sen->settings.min_image_value);
            ImGui::InputFloat("Max. value", &sen->settings.max_image_value);

            ImGui::TreePop();
        }
    }
    void SensorSettingsElement::update_particle_count_sensor(std::shared_ptr<LibFluid::Sensor> sen) {
        auto pc = std::dynamic_pointer_cast<LibFluid::Sensors::GlobalParticleCountSensor>(sen);
        FLUID_ASSERT(pc != nullptr);

        if (StyledImGuiElements::slim_tree_node("Data")) {
            ImGui::Text("Last sensor values");
            const auto& data = pc->get_last_data();

            ImGui::LabelText("Normal Particles", "%d", (int)data.normal_particles);
            ImGui::LabelText("Boundary Particles", "%d", (int)data.boundary_particles);
            ImGui::LabelText("Inactive Particles", "%d", (int)data.inactive_particles);

            ImGui::TreePop();
        }
    }

} // namespace FluidStudio