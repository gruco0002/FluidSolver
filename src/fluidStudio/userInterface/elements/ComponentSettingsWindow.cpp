#include "ComponentSettingsWindow.hpp"

#include "ComponentPanelWindow.hpp"
#include "ImguiHelper.hpp"
#include "userInterface/StyledImGuiElements.hpp"

namespace FluidStudio
{

    void ComponentSettingsWindow::update()
    {
        if (ImGui::Begin("Properties"))
        {
            auto &selection = ui_data.collection().get<ComponentPanelWindow>().selection();
            if (selection.kind != SimulationComponent::Kind::None)
            {
                update_component_settings(selection);
            }
        }
        ImGui::End();
    }

    void ComponentSettingsWindow::update_component_settings(const SimulationComponent &component)
    {
        if (component.kind == SimulationComponent::Kind::Solver)
        {
            solver_settings_element.update();
        }
        else if (component.kind == SimulationComponent::Kind::Timestep)
        {
            timestep_settings_element.update();
        }
        else if (component.kind == SimulationComponent::Kind::Sensor)
        {
            sensor_settings_element.update();
        }
        else if (component.kind == SimulationComponent::Kind::Output)
        {
            update_output_component();
        }
        else if (component.kind == SimulationComponent::Kind::SimulationVisualizer)
        {
            visualizer_settings_element.update();
        }
        else if (component.kind == SimulationComponent::Kind::Entity)
        {
            entity_settings_element.update();
        }
        else if (component.kind == SimulationComponent::Kind::TagDescriptor)
        {
            tag_descriptor_settings_element.update();
        }
        else if (component.kind == SimulationComponent::Kind::EditorVisualizer)
        {
            editor_visualizer_settings_element.update();
        }
    }
    void ComponentSettingsWindow::update_output_component()
    {
        auto &output = ui_data.window().simulator_visualizer_bundle.simulator->output;

        if (StyledImGuiElements::slim_tree_node("Output"))
        {
            ImGui::InputText("Directory", &output->parameters.output_folder);

            ImGui::TreePop();
        }
    }

    void ComponentSettingsWindow::initialize(const UiData &ui_data)
    {
        // initialize this element
        UiElement::initialize(ui_data);

        // add ui elements to collection
        ui_data.collection().add(sensor_settings_element);
        ui_data.collection().add(solver_settings_element);
        ui_data.collection().add(timestep_settings_element);
        ui_data.collection().add(visualizer_settings_element);
        ui_data.collection().add(entity_settings_element);
        ui_data.collection().add(tag_descriptor_settings_element);
        ui_data.collection().add(editor_visualizer_settings_element);

        // initialize ui elements
        sensor_settings_element.initialize(ui_data);
        solver_settings_element.initialize(ui_data);
        timestep_settings_element.initialize(ui_data);
        visualizer_settings_element.initialize(ui_data);
        entity_settings_element.initialize(ui_data);
        tag_descriptor_settings_element.initialize(ui_data);
        editor_visualizer_settings_element.initialize(ui_data);
    }

} // namespace FluidStudio
