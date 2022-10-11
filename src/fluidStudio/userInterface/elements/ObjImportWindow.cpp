#include "ObjImportWindow.hpp"

#include "ImguiHelper.hpp"
#include "importer/ObjLoader.hpp"
#include "importer/ParticleSampler.hpp"

#include <nfd.h>

namespace FluidStudio {

    void ObjImportWindow::update() {
        auto popup_id = update_report_popup();

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
            ImGui::InputInt("Tag", reinterpret_cast<int*>(&particle_tag));

            ImGui::Separator();
            ImGui::LabelText("Particle Size (m)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size);
            ImGui::LabelText("Rest Density (kg/m^3)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density);
            float particle_mass = get_particle_mass();
            ImGui::LabelText("Particle Mass (kg)", "%.3f", particle_mass);

            ImGui::Separator();
            ImGui::Checkbox("Show Report after Import", &report_enabled);
            ImGui::Separator();


            if (!import_enabled) {
                glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);
                ImGui::TextColored(reinterpret_cast<const ImVec4&>(color), "Cannot import 3d data into a scene that is not setup for 3d data!");
            }

            if (ImGui::Button("Import into Scene") && import_enabled) {
                import_data_into_scene();
                reset();
                visible = false;

                if (report_enabled) {
                    show_report_popup(popup_id);
                }
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
        particle_tag = 0;
        report_enabled = true;
    }

    void ObjImportWindow::import_data_into_scene() {
        float particle_size = ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size;
        float rest_density = ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density;
        float particle_mass = get_particle_mass();

        auto collection = ui_data.window().simulator_visualizer_bundle.simulator->data.collection;

        LibFluid::Importer::ObjLoader obj_loader(current_file);
        obj_loader.scale = import_scale;
        auto mesh_data = obj_loader.load_as_meshdata();

        LibFluid::Importer::ParticleSampler sampler(mesh_data, particle_size);
        sampler.generate_samples();
        const auto& samples = sampler.get_samples();

        report_data.created_particles = samples.size();

        for (const auto& sample : samples) {
            size_t index = collection->add();

            auto& pos = collection->get<LibFluid::MovementData3D>(index);
            pos.position = sample;
            pos.acceleration = glm::vec3(0.0f);
            pos.velocity = glm::vec3(0.0f);

            auto& info = collection->get<LibFluid::ParticleInfo>(index);

            info.type = LibFluid::ParticleType::ParticleTypeBoundary;
            info.tag = particle_tag;

            auto& data = collection->get<LibFluid::ParticleData>(index);
            data.density = rest_density;
            data.mass = particle_mass;
            data.pressure = 0.0f;
        }

        ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        ui_data.window().simulator_visualizer_bundle.initialize();
    }
    bool ObjImportWindow::can_import() const {
        auto& collection = ui_data.window().simulator_visualizer_bundle.simulator->data.collection;
        return collection->is_type_present<LibFluid::MovementData3D>() && collection->is_type_present<LibFluid::ParticleInfo>() && collection->is_type_present<LibFluid::ParticleData>();
    }

    unsigned int ObjImportWindow::update_report_popup() {
        auto imgui_id = ImGui::GetID("Obj Import - Report");
        if (ImGui::BeginPopupModal("Obj Import - Report")) {
            ImGui::LabelText("Created Particles", "%d", report_data.created_particles);

            ImGui::Separator();

            ImGui::LabelText("Particle Size (m)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size);
            ImGui::LabelText("Rest Density (kg/m^3)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density);
            float particle_mass = get_particle_mass();
            ImGui::LabelText("Particle Mass (kg)", "%.3f", particle_mass);

            ImGui::Separator();
            
            if(ImGui::Button("Close")){
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        return imgui_id;
    }

    void ObjImportWindow::show_report_popup(unsigned int id) {
        ImGui::OpenPopup(id);
    }

} // namespace FluidStudio
