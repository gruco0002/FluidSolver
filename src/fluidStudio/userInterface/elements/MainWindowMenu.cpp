#include "MainWindowMenu.hpp"

#include "ImguiHelper.hpp"
#include "InsertParticlesWindow.hpp"
#include "helpers/Log.hpp"
#include "helpers/SimulatorHelpers.hpp"
#include "serialization/MainSerializer.hpp"
#include "serializerExtensions/FluidStudioRootSerializerExt.hpp"
#include "serializerExtensions/FluidStudioSerializerExtensions.hpp"
#include "userInterface/UiLayer.hpp"
#include "userInterface/elements/NewSimulationModalWindow.hpp"
#include "userInterface/elements/ObjImportWindow.hpp"
#include "userInterface/elements/PlyImportWindow.hpp"
#include "userInterface/helpers/FileDialogHelper.hpp"
#include "visualizationOverlay/ParticleSelectionByTagOverlay.hpp"

namespace FluidStudio
{

    void MainWindowMenu::update()
    {
        if (ImGui::BeginMainMenuBar())
        {
            bool can_change = ui_data.window().is_safe_to_access_simulation_data();

            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::BeginMenu("New"))
                {
                    if (ImGui::MenuItem("2D Scenario", nullptr, false, can_change))
                    {
                        ui_data.collection().get<NewSimulationModalWindow>().open_window(
                            NewSimulationModalWindow::NewSimulationKind::NewSimulationKind2D);
                    }
                    if (ImGui::MenuItem("3D Scenario", nullptr, false, can_change))
                    {
                        ui_data.collection().get<NewSimulationModalWindow>().open_window(
                            NewSimulationModalWindow::NewSimulationKind::NewSimulationKind3D);
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Open", nullptr, false, can_change))
                {
                    // open scenario

                    update_open_menu();
                }

                if (ImGui::MenuItem("Save", nullptr, false, can_change))
                {
                    save_menu_open = true;
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Insert", can_change))
            {
                if (ImGui::MenuItem("Import Ply File", nullptr, false, can_change))
                {
                    // open the ply import window
                    ui_data.collection().get<PlyImportWindow>().open_window();
                }

                if (ImGui::MenuItem("Import Obj File", nullptr, false, can_change))
                {
                    // open the ply import window
                    ui_data.collection().get<ObjImportWindow>().open_window();
                }

                if (ImGui::MenuItem("Particles", nullptr, false, can_change))
                {
                    ui_data.collection().get<InsertParticlesWindow>().open_window();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View", can_change))
            {
                if (ImGui::MenuItem("Reset Window Layout", nullptr, false, can_change))
                {
                    ui_data.uiLayer().resetDockingLayout();
                }

                ImGui::EndMenu();
            }

            if (SimulatorHelpers::is_3d_simulation(ui_data.window().simulator_visualizer_bundle.simulator))
            {
                auto text_size = ImGui::CalcTextSize("3D-Simulation");
                ImGui::SameLine(ImGui::GetWindowWidth() - text_size.x - 20);
                ImGui::TextColored(ImVec4(0.204f, 0.753f, 0.922f, 1.0f), "3D-Simulation");
            }
            else
            {
                auto text_size = ImGui::CalcTextSize("2D-Simulation");
                ImGui::SameLine(ImGui::GetWindowWidth() - text_size.x - 20);
                ImGui::TextColored(ImVec4(0.204f, 0.922f, 0.753f, 1.0f), "2D-Simulation");
            }

            ImGui::EndMainMenuBar();
        }

        update_save_menu();
    }

    void MainWindowMenu::update_save_menu()
    {
        if (ImGui::BeginPopupModal("Save Simulation"))
        {
            static std::optional<std::string> path;
            static std::string particle_filepath = "particles.data";
            static bool save_particle_data = true;

            ImGui::TextWrapped("Save the current simulation as json file. Optionally you can save the particle data. "
                               "If it already exists "
                               "and you do not want to override it, make sure to uncheck the checkbox and provide the "
                               "name of the current "
                               "particle data file relative to the json file in the text field.");

            ImGui::Separator();

            if (ImGui::Button("Choose"))
            {
                path = FileDialogHelper::show_safe_file_dialog("json");
                if (path.has_value())
                {
                    particle_filepath =
                        std::filesystem::path(path.value()).filename().replace_extension(".data").string();
                }
            }
            ImGui::SameLine();

            if (path.has_value())
            {
                ImGui::LabelText("File", "%s", path.value().c_str());
            }
            else
            {
                ImGui::LabelText("File", "Not selected");
            }

            ImGui::Separator();
            ImGui::Checkbox("Save Particle Data", &save_particle_data);
            ImGui::InputText("Particle File", &particle_filepath);

            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Save") && path.has_value())
            {
                // Save
                auto extensions = SerializerExtensions::create_serializer_extenstions();
                extensions.root_serializer_extensions.push_back(
                    std::make_shared<FluidStudioRootSerializerExt>(ui_data.window().editor_visualizer));

                LibFluid::Serialization::SerializationContext context_output;

                LibFluid::Serialization::MainSerializer serializer(path.value(), extensions);
                serializer.serialize_bundle(ui_data.window().simulator_visualizer_bundle,
                                            {save_particle_data, particle_filepath}, &context_output);

                // output warnings
                if (!context_output.issues.empty())
                {
                    LibFluid::Log::warning("The serializer encountered issues during saving!");
                    for (const auto &issue : context_output.issues)
                    {
                        LibFluid::Log::warning(issue.to_formatted_string());
                    }
                }

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        if (save_menu_open)
        {
            save_menu_open = false;
            ImGui::OpenPopup("Save Simulation");
        }
    }

    void MainWindowMenu::update_open_menu()
    {
        auto res = FileDialogHelper::show_open_file_dialog("json");
        if (res.has_value())
        {
            // load simulation
            auto extensions = SerializerExtensions::create_serializer_extenstions();
            auto root_ext = std::make_shared<FluidStudioRootSerializerExt>();
            extensions.root_serializer_extensions.push_back(root_ext);

            LibFluid::Serialization::SerializationContext context_output;

            LibFluid::Serialization::MainSerializer serializer(res.value(), extensions);
            auto bundle = serializer.deserialize(&context_output);

            // output warnings
            if (!context_output.issues.empty())
            {
                LibFluid::Log::error("The serializer encountered issues during loading!");
                for (const auto &issue : context_output.issues)
                {
                    LibFluid::Log::error(issue.to_formatted_string());
                }
            }
            else
            {
                ui_data.window().simulator_visualizer_bundle = bundle;
                ui_data.window().editor_visualizer = root_ext->get_editor_visualizer();
                ui_data.window().on_new_simulation();
            }
        }
    }

} // namespace FluidStudio
