#pragma once

#include "userInterface/UiElement.hpp"
#include "userInterface/elements/componentSettings/EditorVisualizerSettingsElement.hpp"
#include "userInterface/elements/componentSettings/EntitySettingsElement.hpp"
#include "userInterface/elements/componentSettings/SensorSettingsElement.hpp"
#include "userInterface/elements/componentSettings/SolverSettingsElement.hpp"
#include "userInterface/elements/componentSettings/TagDescriptorSettingsElement.hpp"
#include "userInterface/elements/componentSettings/TimestepSettingsElement.hpp"
#include "userInterface/elements/componentSettings/VisualizerSettingsElement.hpp"
#include "userInterface/helpers/SimulationComponent.hpp"


namespace FluidStudio {
    class ComponentSettingsWindow : public UiElement {
      public:
        void update() override;
        void initialize(const UiData& ui_data) override;

      private:
        SensorSettingsElement sensor_settings_element;
        SolverSettingsElement solver_settings_element;
        TimestepSettingsElement timestep_settings_element;
        VisualizerSettingsElement visualizer_settings_element;
        EntitySettingsElement entity_settings_element;
        TagDescriptorSettingsElement tag_descriptor_settings_element;
        EditorVisualizerSettingsElement editor_visualizer_settings_element;

        void update_component_settings(const SimulationComponent& component);

        void update_output_component();
    };
} // namespace FluidStudio
