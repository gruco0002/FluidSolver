#pragma once

#include "userInterface/SimulationComponent.hpp"
#include "userInterface/UiElement.hpp"
#include <string>
#include <vector>


namespace FluidStudio {

    class ComponentPanelWindow final : public UiElement {
      public:
        void update() override;

        const SimulationComponent& selection() const;

      private:
        std::vector<std::string> menu_entity_names;

        SimulationComponent m_selection;

        void update_menu();
        void update_selection_based_ui();
        void delete_component(const SimulationComponent& component);
        void update_component_node(const char* name, const SimulationComponent& component);
    };

} // namespace FluidStudio