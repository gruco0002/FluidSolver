#include "PlyImportWindow.hpp"

#include "FluidMath.hpp"
#include "FluidSolverWindow.hpp"
#include "ImguiHelper.hpp"
#include "userInterface/helpers/ParticleCollectionHelper.hpp"

#include <filesystem>
#include <fstream>
#include <nfd.h>
#include <tinyply.h>


namespace FluidStudio {

    const char* particle_type_to_string(LibFluid::ParticleType type) {
        switch (type) {
            case LibFluid::ParticleTypeNormal:
                return "Normal";
            case LibFluid::ParticleTypeBoundary:
                return "Boundary";
            case LibFluid::ParticleTypeInactive:
                return "Inactive";
            default:
                return "Unknown";
        }
    }


    void PlyImportWindow::load_colors_if_required() {
        if (loaded_file == current_file) {
            return;
        }
        loaded_file = current_file;

        if (!std::filesystem::exists(loaded_file)) {
            return;
        }

        mapped_colors.clear();
        colors.clear();
        vertices.clear();


        std::fstream filestream(loaded_file);

        tinyply::PlyFile file;
        file.parse_header(filestream);

        std::shared_ptr<tinyply::PlyData> ply_vertices, ply_colors;

        ply_vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});
        ply_colors = file.request_properties_from_element("vertex", {"red", "green", "blue"});

        file.read(filestream);

        if (ply_vertices->t == tinyply::Type::FLOAT32) {
            for (size_t i = 0; i < ply_vertices->count; i++) {
                float x, y, z;
                x = ((float*)ply_vertices->buffer.get())[i * 3 + 0];
                y = ((float*)ply_vertices->buffer.get())[i * 3 + 1];
                z = ((float*)ply_vertices->buffer.get())[i * 3 + 2];

                glm::vec3 vertex = glm::vec3(x, z, y);

                vertices.push_back(vertex);
            }
        }

        uint32_t next_free_tag = ParticleCollectionHelper::get_next_free_tag(ui_data.window().simulator_visualizer_bundle.simulator->data.collection);

        if (ply_colors->t == tinyply::Type::UINT8) {
            for (size_t i = 0; i < ply_colors->count; i++) {
                uint8_t r, g, b;
                r = ply_colors->buffer.get()[i * 3 + 0];
                g = ply_colors->buffer.get()[i * 3 + 1];
                b = ply_colors->buffer.get()[i * 3 + 2];

                glm::vec3 color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);

                colors.push_back(color);
                if (mapped_colors.find(color) == mapped_colors.end()) {
                    auto info = MapInformation();
                    info.particle_tag = next_free_tag + mapped_colors.size();
                    mapped_colors[color] = info;
                }
            }
        }
    }
    void PlyImportWindow::import_data_into_simulation() {
        // set the parameters of the simulation to the specified ones
        const auto& parameters = ui_data.window().simulator_visualizer_bundle.simulator->parameters;
        float particle_size = parameters.particle_size;
        float rest_density = parameters.rest_density;
        float particle_mass = get_particle_mass();


        auto collection = ui_data.window().simulator_visualizer_bundle.simulator->data.collection;

        // import the data
        for (size_t i = 0; i < vertices.size(); i++) {
            if (i >= colors.size())
                break;

            if (mapped_colors.find(colors[i]) == mapped_colors.end())
                continue;

            auto scaled_vertex = vertices[i] * particle_size;

            for (int x = 0; x < particle_multiplier; x++) {
                for (int y = 0; y < particle_multiplier; y++) {
                    for (int z = 0; z < particle_multiplier; z++) {
                        size_t index = collection->add();

                        auto& pos = collection->get<LibFluid::MovementData3D>(index);
                        pos.position = scaled_vertex * (float)particle_multiplier + glm::vec3((float)x, (float)y, (float)z);
                        pos.acceleration = glm::vec3(0.0f);
                        pos.velocity = glm::vec3(0.0f);

                        auto& info = collection->get<LibFluid::ParticleInfo>(index);

                        const auto& map_info = mapped_colors[colors[i]];
                        info.type = map_info.particle_type;
                        info.tag = map_info.particle_tag;

                        auto& data = collection->get<LibFluid::ParticleData>(index);
                        data.density = rest_density;
                        data.mass = particle_mass;
                        data.pressure = 0.0f;
                    }
                }
            }
        }


        ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        ui_data.window().simulator_visualizer_bundle.initialize();


        // save the data in the timeline
        ui_data.window().timeline_service.reset();
        ui_data.window().timeline_service.save_timestep_result();

        // hide the window again
        visible = false;
        reset();
    }
    void PlyImportWindow::update() {
        if (!visible)
            return;

        load_colors_if_required();


        if (ImGui::Begin("Ply Import", &visible)) {
            bool import_enabled = can_import();

            ImGui::Text("Current file: %s", current_file.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Open")) {
                char* p = nullptr;
                auto res = NFD_OpenDialog("ply", nullptr, &p);
                if (res == NFD_OKAY) {
                    std::string path(p);
                    free(p);

                    this->current_file = path;
                }
            }

            if (ImGui::InputInt("Particle Multiplier", &this->particle_multiplier)) {
                if (particle_multiplier <= 0)
                    particle_multiplier = 1;
            }

            ImGui::Separator();
            ImGui::LabelText("Particle Size (m)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size);
            ImGui::LabelText("Rest Density (kg/m^3)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density);
            float particle_mass = get_particle_mass();
            ImGui::LabelText("Particle Mass (kg)", "%.3f", particle_mass);

            ImGui::Separator();

            ImGui::Text("Color mappings");
            int i = 0;
            for (auto mapped : mapped_colors) {
                ImGui::PushID(i);

                glm::vec4 color(mapped.first, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Text, *((ImVec4*)&color));
                ImGui::Bullet();
                ImGui::PopStyleColor();

                const auto& mapped_info = mapped.second;

                ImGui::Text("Color %d:", (i + 1));
                ImGui::SameLine();

                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);

                if (ImGui::BeginCombo("Type", particle_type_to_string(mapped_info.particle_type))) {
                    if (ImGui::Selectable(particle_type_to_string(LibFluid::ParticleType::ParticleTypeNormal),
                                mapped_info.particle_type == LibFluid::ParticleType::ParticleTypeNormal)) {
                        mapped_colors[mapped.first].particle_type = LibFluid::ParticleType::ParticleTypeNormal;
                    }

                    if (ImGui::Selectable(particle_type_to_string(LibFluid::ParticleType::ParticleTypeBoundary),
                                mapped_info.particle_type == LibFluid::ParticleType::ParticleTypeBoundary)) {
                        mapped_colors[mapped.first].particle_type = LibFluid::ParticleType::ParticleTypeBoundary;
                    }

                    if (ImGui::Selectable(particle_type_to_string(LibFluid::ParticleType::ParticleTypeInactive),
                                mapped_info.particle_type == LibFluid::ParticleType::ParticleTypeInactive)) {
                        mapped_colors[mapped.first].particle_type = LibFluid::ParticleType::ParticleTypeInactive;
                    }

                    ImGui::EndCombo();
                }

                ImGui::SameLine();
                ImGui::InputInt("Tag", reinterpret_cast<int*>(&mapped_colors[mapped.first].particle_tag));

                ImGui::PopItemWidth();
                ImGui::PopID();
                i++;
            }


            ImGui::Separator();

            if (!import_enabled) {
                glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);
                ImGui::TextColored(reinterpret_cast<const ImVec4&>(color), "Cannot import 3d data into a scene that is not setup for 3d data!");
            }

            if (ImGui::Button("Import into Scene") && import_enabled) {
                import_data_into_simulation();
            }
        }

        ImGui::End();
    }
    void PlyImportWindow::open_window() {
        visible = true;
    }
    void PlyImportWindow::reset() {
        current_file = "";
        loaded_file = "";
        particle_multiplier = 1;
        colors.clear();
        vertices.clear();
        mapped_colors.clear();
    }
    float PlyImportWindow::get_particle_mass() const {
        float particle_size = ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size;
        float rest_density = ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density;
        float particle_mass = LibFluid::Math::pow3(particle_size) * rest_density;
        return particle_mass;
    }

    bool PlyImportWindow::can_import() const {
        auto& collection = ui_data.window().simulator_visualizer_bundle.simulator->data.collection;
        return collection->is_type_present<LibFluid::MovementData3D>() && collection->is_type_present<LibFluid::ParticleInfo>() && collection->is_type_present<LibFluid::ParticleData>();
    }
} // namespace FluidStudio