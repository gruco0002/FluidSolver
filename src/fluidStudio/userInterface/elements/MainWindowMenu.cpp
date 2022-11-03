#include "MainWindowMenu.hpp"

#include "helpers/Log.hpp"
#include "serialization/MainSerializer.hpp"
#include "serializerExtensions/FluidStudioSerializerExtensions.hpp"
#include "userInterface/elements/NewSimulationModalWindow.hpp"
#include "userInterface/elements/ObjImportWindow.hpp"
#include "userInterface/elements/PlyImportWindow.hpp"
#include "visualizationOverlay/ParticleSelectionByTagOverlay.hpp"

#include "ImguiHelper.hpp"
#include "InsertParticlesWindow.hpp"

#include <nfd.h>

namespace FluidStudio {

    void MainWindowMenu::update() {
        if (ImGui::BeginMainMenuBar()) {
            bool can_change = ui_data.window().is_safe_to_access_simulation_data();

            if (ImGui::BeginMenu("File")) {
                if (ImGui::BeginMenu("New")) {
                    if (ImGui::MenuItem("2D Scenario", nullptr, false, can_change)) {
                        ui_data.collection().get<NewSimulationModalWindow>().open_window(NewSimulationModalWindow::NewSimulationKind::NewSimulationKind2D);
                    }
                    if (ImGui::MenuItem("3D Scenario", nullptr, false, can_change)) {
                        ui_data.collection().get<NewSimulationModalWindow>().open_window(NewSimulationModalWindow::NewSimulationKind::NewSimulationKind3D);
                    }
                    ImGui::EndMenu();
                }


                if (ImGui::MenuItem("Open", nullptr, false, can_change)) {
                    // open scenario

                    update_open_menu();
                }

                if (ImGui::MenuItem("Save", nullptr, false, can_change)) {
                    save_menu_open = true;
                }

                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu("Insert", can_change)) {
                if (ImGui::MenuItem("Import Ply File", nullptr, false, can_change)) {
                    // open the ply import window
                    ui_data.collection().get<PlyImportWindow>().open_window();
                }

                if (ImGui::MenuItem("Import Obj File", nullptr, false, can_change)) {
                    // open the ply import window
                    ui_data.collection().get<ObjImportWindow>().open_window();
                }

                if (ImGui::MenuItem("Particles", nullptr, false, can_change)) {
                    ui_data.collection().get<InsertParticlesWindow>().open_window();
                }


                ImGui::EndMenu();
            }

            if (is_3d_simulation()) {
                auto text_size = ImGui::CalcTextSize("3D-Simulation");
                ImGui::SameLine(ImGui::GetWindowWidth() - text_size.x - 20);
                ImGui::TextColored(ImVec4(0.204f, 0.753f, 0.922f, 1.0f), "3D-Simulation");
            } else {
                auto text_size = ImGui::CalcTextSize("2D-Simulation");
                ImGui::SameLine(ImGui::GetWindowWidth() - text_size.x - 20);
                ImGui::TextColored(ImVec4(0.204f, 0.922f, 0.753f, 1.0f), "2D-Simulation");
            }

            ImGui::EndMainMenuBar();
        }

        update_save_menu();
    }

    void MainWindowMenu::update_save_menu() {
        if (ImGui::BeginPopupModal("Save Simulation")) {
            static char* path = nullptr;
            static std::string particle_filepath = "particles.data";
            static bool save_particle_data = true;


            ImGui::TextWrapped(
                    "Save the current simulation as json file. Optionally you can save the particle data. If it already exists "
                    "and you do not want to override it, make sure to uncheck the checkbox and provide the name of the current "
                    "particle data file relative to the json file in the text field.");

            ImGui::Separator();

            if (ImGui::Button("Choose")) {
                char* p = nullptr;

                auto res = NFD_SaveDialog("json", nullptr, &p);
                if (res == NFD_OKAY) {
                    if (path != nullptr)
                        free(path);
                    path = p;
                    particle_filepath = std::filesystem::path(path).filename().replace_extension(".data").string();
                } else {
                    free(p);
                }
            }
            ImGui::SameLine();
            if (path != nullptr) {
                ImGui::LabelText("File", path);
            } else {
                ImGui::LabelText("File", "Not selected");
            }

            ImGui::Separator();
            ImGui::Checkbox("Save Particle Data", &save_particle_data);
            ImGui::InputText("Particle File", &particle_filepath);

            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Save") && path != nullptr) {
                // Save
                auto extensions = SerializerExtensions::create_serializer_extenstions();

                LibFluid::Serialization::SerializationContext context_output;

                LibFluid::Serialization::MainSerializer serializer(path, extensions);
                serializer.serialize_bundle(ui_data.window().simulator_visualizer_bundle, {save_particle_data, particle_filepath}, &context_output);

                // output warnings
                if (!context_output.issues.empty()) {
                    LibFluid::Log::warning("The serializer encountered issues during saving!");
                    for (const auto& issue : context_output.issues) {
                        LibFluid::Log::warning(issue.to_formatted_string());
                    }
                }

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        if (save_menu_open) {
            save_menu_open = false;
            ImGui::OpenPopup("Save Simulation");
        }
    }

    void MainWindowMenu::update_open_menu() {
        char* p = nullptr;
        auto res = NFD_OpenDialog("json", nullptr, &p);
        if (res == NFD_OKAY) {
            std::string path(p);
            free(p);

            // load simulation
            auto extensions = SerializerExtensions::create_serializer_extenstions();

            LibFluid::Serialization::SerializationContext context_output;

            LibFluid::Serialization::MainSerializer serializer(path, extensions);
            auto bundle = serializer.deserialize(&context_output);

            // output warnings
            if (!context_output.issues.empty()) {
                LibFluid::Log::error("The serializer encountered issues during loading!");
                for (const auto& issue : context_output.issues) {
                    LibFluid::Log::error(issue.to_formatted_string());
                }
            } else {
                ui_data.window().simulator_visualizer_bundle = bundle;
                ui_data.window().on_new_simulation();
            }
        }
    }


    bool MainWindowMenu::is_3d_simulation() const {
        if (ui_data.window().simulator_visualizer_bundle.simulator->data.collection->is_type_present<LibFluid::MovementData3D>()) {
            return true;
        }
        return false;
    }

} // namespace FluidStudio