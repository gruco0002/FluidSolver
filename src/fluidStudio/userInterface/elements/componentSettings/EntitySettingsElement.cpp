#include "EntitySettingsElement.hpp"

#include "ImguiHelper.hpp"
#include "entities/ParticleRemover3D.hpp"
#include "userInterface/SimulationComponent.hpp"
#include "userInterface/StyledImGuiElements.hpp"
#include "userInterface/elements/ComponentPanelWindow.hpp"

namespace FluidStudio {

    void EntitySettingsElement::update() {
        auto& selection = ui_data.collection().get<ComponentPanelWindow>().selection();
        if (selection.kind != SimulationComponent::Kind::Entity) {
            return;
        }
        auto index = selection.index;

        FLUID_ASSERT(index < ui_data.window().simulator_visualizer_bundle.simulator->data.entities.size());

        auto ent = ui_data.window().simulator_visualizer_bundle.simulator->data.entities[index];
        if (StyledImGuiElements::slim_tree_node("Entity")) {
            ImGui::LabelText("Type", SimulationComponent::get_entity_type_name(ent));

            ImGui::TreePop();
        }

        if (std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(ent)) {
            update_particle_remover_3d_component(std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(ent));
        }
    }


    void EntitySettingsElement::update_particle_remover_3d_component(std::shared_ptr<LibFluid::SimulationEntity> tmp) {
        auto ent = std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(tmp);

        if (StyledImGuiElements::slim_tree_node("Volume")) {
            ImGui::InputFloat3("Center", reinterpret_cast<float*>(&ent->parameters.volume.center));
            ImGui::InputFloat3("Distance", reinterpret_cast<float*>(&ent->parameters.volume.distance_from_center));

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("Settings")) {
            ImGui::Checkbox("Remove if Outside", &ent->parameters.remove_if_outside);

            ImGui::TreePop();
        }
    }

} // namespace FluidStudio