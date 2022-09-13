#pragma once

#include "userInterface/SimulationComponent.hpp"
#include "userInterface/UiElement.hpp"
#include <string>
#include <vector>


namespace FluidStudio {

    class ComponentPanelWindow final : public UiElement {
      public:
        void update() override;

        const Component& selection() const;

      private:
        std::vector<std::string> menu_entity_names;

        Component m_selection;

        void update_menu();
        void update_selection_based_ui();
        void delete_component(const Component& component);
        void update_component_node(const char* name, const Component& component);
    };

} // namespace FluidStudio