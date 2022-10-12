#include "EntitySettingsElement.hpp"

#include "ImguiHelper.hpp"
#include "entities/BoundaryPreprocessor.hpp"
#include "entities/ParticleRemover3D.hpp"
#include "entities/VelocityAlterationByTag.hpp"
#include "userInterface/StyledImGuiElements.hpp"
#include "userInterface/elements/ComponentPanelWindow.hpp"
#include "userInterface/helpers/SimulationComponent.hpp"
#include "userInterface/helpers/TypeInformationProvider.hpp"

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
            ImGui::LabelText("Type", "%s", TypeInformationProvider::get_entity_type_name(ent));
            ImGui::TextWrapped("%s", TypeInformationProvider::get_entity_description(ent));

            ImGui::TreePop();
        }

        if (std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(ent)) {
            update_particle_remover_3d_component(ent);
        }

        if (std::dynamic_pointer_cast<LibFluid::BoundaryPreprocessor>(ent)) {
            update_boundary_preprocessor_component(ent);
        }

        if (std::dynamic_pointer_cast<LibFluid::VelocityAlterationByTag>(ent)) {
            update_velocity_alteration_by_tag_component(ent);
        }
    }


    void EntitySettingsElement::update_particle_remover_3d_component(std::shared_ptr<LibFluid::SimulationEntity> tmp) {
        auto ent = std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(tmp);
        FLUID_ASSERT(ent != nullptr);

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
    void EntitySettingsElement::update_boundary_preprocessor_component(std::shared_ptr<LibFluid::SimulationEntity> tmp) {
        auto ent = std::dynamic_pointer_cast<LibFluid::BoundaryPreprocessor>(tmp);
        FLUID_ASSERT(ent != nullptr);

        if (StyledImGuiElements::slim_tree_node("Parameters")) {
            ImGui::TreePop();
        }
    }
    void EntitySettingsElement::update_velocity_alteration_by_tag_component(std::shared_ptr<LibFluid::SimulationEntity> tmp) {
        auto ent = std::dynamic_pointer_cast<LibFluid::VelocityAlterationByTag>(tmp);
        FLUID_ASSERT(ent != nullptr);

        if (StyledImGuiElements::slim_tree_node("Parameters")) {
            if (ImGui::InputInt("Tag", reinterpret_cast<int*>(&ent->parameters.tag))) {
                ent->parameters.notify_that_data_changed();
            }
            if (ImGui::InputFloat3("Velocity", reinterpret_cast<float*>(&ent->parameters.velocity))) {
                ent->parameters.notify_that_data_changed();
            }

            ImGui::TreePop();
        }
    }

} // namespace FluidStudio