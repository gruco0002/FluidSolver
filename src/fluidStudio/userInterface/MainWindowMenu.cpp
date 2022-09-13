#include "MainWindowMenu.hpp"

#include "ExtendedSimulationSerializer.hpp"
#include "userInterface/PlyImportWindow.hpp"

#include <ImguiHelper.hpp>
#include <nfd.h>

namespace FluidStudio {

    void MainWindowMenu::update() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                bool can_change = ui_data.window().is_safe_to_access_simulation_data();

                if (ImGui::MenuItem("New", nullptr, false, can_change)) {
                    // create a new scenario
                    ui_data.window().create_empty_simulation();
                }

                if (ImGui::MenuItem("Open", nullptr, false, can_change)) {
                    // open scenario

                    update_open_menu();
                }

                if (ImGui::MenuItem("Save", nullptr, false, can_change)) {
                    save_menu_open = true;
                }

                if (ImGui::BeginMenu("Import", can_change)) {
                    if (ImGui::MenuItem("Ply File", nullptr, false, can_change)) {
                        // open the ply import window
                        ui_data.collection().get<PlyImportWindow>().open_window();
                    }

                    ImGui::EndMenu();
                }


                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Test")) {
                bool can_change = ui_data.window().is_safe_to_access_simulation_data();

                if (ImGui::MenuItem("Test 3D", nullptr, false, can_change)) {
                    ui_data.window().create_3d_test_simulation();
                }

                ImGui::EndMenu();
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
                ExtendedSimulationSerializer s(path, {save_particle_data, particle_filepath});
                s.save_to_file(ui_data.window().simulator_visualizer_bundle);
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
            ExtendedSimulationSerializer s(path);
            auto simulation = s.load_from_file();
            if (!s.has_errors()) {
                ui_data.window().simulator_visualizer_bundle = simulation;
                ui_data.window().on_new_simulation();
            }
        }
    }

} // namespace FluidStudio