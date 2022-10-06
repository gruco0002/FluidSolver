#include "ObjImportWindow.hpp"
#include <nfd.h>

#include "ImguiHelper.hpp"

namespace FluidStudio {

    void ObjImportWindow::update() {
        if (!visible)
            return;


        if (ImGui::Begin("Obj Import", &visible)) {
            bool import_enabled = can_import();

            ImGui::Text("Import obj data into an existing scene.");
            ImGui::Separator();

            ImGui::Text("Current file: %s", current_file.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Open")) {
                char* p = nullptr;
                auto res = NFD_OpenDialog("obj", nullptr, &p);
                if (res == NFD_OKAY) {
                    std::string path(p);
                    free(p);

                    this->current_file = path;
                }
            }

            ImGui::Separator();

            ImGui::InputFloat("Model Scale", &import_scale);

            ImGui::Separator();
            ImGui::LabelText("Particle Size (m)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size);
            ImGui::LabelText("Rest Density (kg/m^3)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density);
            float particle_mass = get_particle_mass();
            ImGui::LabelText("Particle Mass (kg)", "%.3f", particle_mass);

            ImGui::Separator();


            if (!import_enabled) {
                glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);
                ImGui::TextColored(reinterpret_cast<const ImVec4&>(color), "Cannot import 3d data into a scene that is not setup for 3d data!");
            }

            if (ImGui::Button("Import into Scene") && import_enabled) {
                import_data_into_scene();
                reset();
                visible = false;
            }
        }
        ImGui::End();
    }

    void ObjImportWindow::open_window() {
        visible = true;
    }

    float ObjImportWindow::get_particle_mass() const {
        float particle_size = ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size;
        float rest_density = ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density;
        float particle_mass = LibFluid::Math::pow3(particle_size) * rest_density;
        return particle_mass;
    }

    void ObjImportWindow::reset() {
        import_scale = 1.0;
        current_file = "";
    }

    void ObjImportWindow::import_data_into_scene() {
        // TODO: implement
    }
    bool ObjImportWindow::can_import() const {
        auto& collection = ui_data.window().simulator_visualizer_bundle.simulator->data.collection;
        return collection->is_type_present<LibFluid::MovementData3D>() && collection->is_type_present<LibFluid::ParticleInfo>() && collection->is_type_present<LibFluid::ParticleData>();
    }

} // namespace FluidStudio
