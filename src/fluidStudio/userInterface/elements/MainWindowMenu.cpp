#include "MainWindowMenu.hpp"

#include "Log.hpp"
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


            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Select Particles by Tag", nullptr, false, can_change)) {
                    tag_selection_modal_open = true;
                }

                bool is_particle_selection_active = std::dynamic_pointer_cast<ParticleSelectionByTagOverlay>(ui_data.window().visualization_overlay.data.overlay_instance) != nullptr;
                if (ImGui::MenuItem("Cancel Selection", nullptr, false, is_particle_selection_active)) {
                    ui_data.window().visualization_overlay.set_new_overlay_instance(nullptr);
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


            if (ImGui::BeginMenu("Test")) {
                if (ImGui::MenuItem("Test 3D", nullptr, false, can_change)) {
                    ui_data.window().create_3d_test_simulation();
                }

                ImGui::EndMenu();
            }


            ImGui::EndMainMenuBar();
        }

        update_save_menu();
        update_tag_selection_modal();
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

    void MainWindowMenu::update_tag_selection_modal() {
        static uint32_t particle_tag = 0;
        if (ImGui::BeginPopupModal("Particle Selection by Tag")) {
            ImGui::InputInt("Tag", reinterpret_cast<int*>(&particle_tag));

            if (particle_tag == (uint32_t)-1) {
                ImGui::Text("This disables the current selection.");
            }

            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Select")) {
                // select particles

                if (particle_tag == (uint32_t)-1) {
                    ui_data.window().visualization_overlay.set_new_overlay_instance(nullptr);
                } else {
                    auto overlay = std::make_shared<ParticleSelectionByTagOverlay>(ui_data.window().simulator_visualizer_bundle.simulator, particle_tag);
                    ui_data.window().visualization_overlay.set_new_overlay_instance(overlay);
                }

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        if (tag_selection_modal_open) {
            particle_tag = 0;
            tag_selection_modal_open = false;
            ImGui::OpenPopup("Particle Selection by Tag");
        }
    }

} // namespace FluidStudio