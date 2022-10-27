#include "TagDescriptorSettingsElement.hpp"
#include "ImguiHelper.hpp"
#include "userInterface/StyledImGuiElements.hpp"
#include "userInterface/elements/ComponentPanelWindow.hpp"

namespace FluidStudio {

    void TagDescriptorSettingsElement::update() {
        auto& selection = ui_data.collection().get<ComponentPanelWindow>().selection();
        if (selection.kind != SimulationComponent::Kind::TagDescriptor) {
            return;
        }
        auto index = selection.index;


        FLUID_ASSERT(index < ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors.size());
        auto& descriptor = ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors[index];

        if (StyledImGuiElements::slim_tree_node("Tag Descriptor")) {
            ImGui::InputText("Title", &descriptor.title);
            ImGui::InputInt("Tag", reinterpret_cast<int*>(&descriptor.particle_tag));
            ImGui::InputTextMultiline("Description", &descriptor.description);

            ImGui::TreePop();
        }
    }
} // namespace FluidStudio