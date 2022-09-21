#include "PlyImportWindow.hpp"

#include "FluidMath.hpp"
#include "FluidSolverWindow.hpp"
#include "ImguiHelper.hpp"

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
            case LibFluid::ParticleTypeDead:
                return "Dead";
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

        if (ply_colors->t == tinyply::Type::UINT8) {
            for (size_t i = 0; i < ply_colors->count; i++) {
                uint8_t r, g, b;
                r = ply_colors->buffer.get()[i * 3 + 0];
                g = ply_colors->buffer.get()[i * 3 + 1];
                b = ply_colors->buffer.get()[i * 3 + 2];

                glm::vec3 color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);

                colors.push_back(color);
                if (mapped_colors.find(color) == mapped_colors.end()) {
                    mapped_colors[color] = LibFluid::ParticleType::ParticleTypeNormal;
                }
            }
        }
    }
    void PlyImportWindow::import_data_into_simulation() {
        // create an empty scenario
        ui_data.window().create_empty_3d_simulation();

        // set the parameters of the simulation to the specified ones
        auto& parameters = ui_data.window().simulator_visualizer_bundle.simulator->parameters;
        parameters.rest_density = rest_density;
        parameters.particle_size = particle_size;
        parameters.notify_that_data_changed();

        ui_data.window().simulator_visualizer_bundle.initialize();

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

                        auto& info = collection->get<LibFluid::ParticleInfo>(index);

                        info.type = mapped_colors[colors[i]];

                        auto& data = collection->get<LibFluid::ParticleData>(index);
                        data.density = 1.0f;
                        data.mass = 1.0f;
                        data.pressure = 0.0f;
                    }
                }
            }
        }

        // hide the window again
        visible = false;
        reset();
    }
    void PlyImportWindow::update() {
        if (!visible)
            return;

        load_colors_if_required();

        if (ImGui::Begin("Ply Import", &visible)) {
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

            if (ImGui::InputFloat("Particle Size (m)", &particle_size)) {
                if (particle_size <= 0) {
                    particle_size = 1.0f;
                }
            }

            if (ImGui::InputFloat("Rest Density (kg/m^3)", &rest_density)) {
                if (rest_density <= 0) {
                    rest_density = 1000.0f;
                }
            }

            float particle_mass = LibFluid::Math::pow3(particle_size) * rest_density;
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

                ImGui::Text("Color %d:", (i + 1));
                ImGui::SameLine();
                if (ImGui::BeginCombo("Type", particle_type_to_string(mapped.second))) {
                    if (ImGui::Selectable(particle_type_to_string(LibFluid::ParticleType::ParticleTypeNormal),
                                mapped.second == LibFluid::ParticleType::ParticleTypeNormal)) {
                        mapped_colors[mapped.first] = LibFluid::ParticleType::ParticleTypeNormal;
                    }

                    if (ImGui::Selectable(particle_type_to_string(LibFluid::ParticleType::ParticleTypeBoundary),
                                mapped.second == LibFluid::ParticleType::ParticleTypeBoundary)) {
                        mapped_colors[mapped.first] = LibFluid::ParticleType::ParticleTypeBoundary;
                    }

                    if (ImGui::Selectable(particle_type_to_string(LibFluid::ParticleType::ParticleTypeDead),
                                mapped.second == LibFluid::ParticleType::ParticleTypeDead)) {
                        mapped_colors[mapped.first] = LibFluid::ParticleType::ParticleTypeDead;
                    }

                    ImGui::EndCombo();
                }

                ImGui::PopID();
                i++;
            }


            ImGui::Separator();

            if (ImGui::Button("Import")) {
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
} // namespace FluidStudio