#pragma once

#include "userInterface/UiElement.hpp"
#include "userInterface/helpers/SimulationComponent.hpp"
#include <string>
#include <vector>


namespace FluidStudio {

    class ComponentPanelWindow final : public UiElement {
      public:
        void update() override;

        const SimulationComponent& selection() const;

      private:
        std::vector<std::string> menu_entity_names;
        std::vector<std::string> menu_sensor_names;

        SimulationComponent m_selection;

        void update_menu();
        void update_selection_based_ui();
        void delete_component(const SimulationComponent& component);
        void update_component_node(const char* name, const SimulationComponent& component);

        bool begin_structural_node(const char* name);

        void verify_selection_is_okay();
    };

} // namespace FluidStudio