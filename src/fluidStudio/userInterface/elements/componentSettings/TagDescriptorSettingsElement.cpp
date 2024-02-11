#include "TagDescriptorSettingsElement.hpp"
#include "ImguiHelper.hpp"
#include "userInterface/StyledImGuiElements.hpp"
#include "userInterface/elements/ComponentPanelWindow.hpp"
#include "userInterface/helpers/TypeInformationProvider.hpp"

namespace FluidStudio
{

    void TagDescriptorSettingsElement::update()
    {
        auto &selection = ui_data.collection().get<ComponentPanelWindow>().selection();
        if (selection.kind != SimulationComponent::Kind::TagDescriptor)
        {
            return;
        }
        auto index = selection.index;

        FLUID_ASSERT(index <
                     ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors.size());
        auto &descriptor =
            ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors[index];

        if (StyledImGuiElements::slim_tree_node("Tag Descriptor"))
        {
            ImGui::InputText("Title", &descriptor.title);
            ImGui::InputInt("Tag", reinterpret_cast<int *>(&descriptor.particle_tag));
            ImGui::InputTextMultiline("Description", &descriptor.description);

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("Edit Particles"))
        {
            {
                if (ImGui::BeginCombo("Type", TypeInformationProvider::particle_type_to_string(particle_type)))
                {
                    if (ImGui::Selectable(
                            TypeInformationProvider::particle_type_to_string(LibFluid::ParticleTypeNormal),
                            particle_type == LibFluid::ParticleTypeNormal))
                    {
                        particle_type = LibFluid::ParticleTypeNormal;
                    }
                    if (ImGui::Selectable(
                            TypeInformationProvider::particle_type_to_string(LibFluid::ParticleTypeBoundary),
                            particle_type == LibFluid::ParticleTypeBoundary))
                    {
                        particle_type = LibFluid::ParticleTypeBoundary;
                    }
                    if (ImGui::Selectable(
                            TypeInformationProvider::particle_type_to_string(LibFluid::ParticleTypeInactive),
                            particle_type == LibFluid::ParticleTypeInactive))
                    {
                        particle_type = LibFluid::ParticleTypeInactive;
                    }
                    ImGui::EndCombo();
                }

                ImGui::SameLine();
                if (ImGui::Button("Apply"))
                {
                    apply_type_to_particles(descriptor.particle_tag);
                }
            }

            ImGui::TreePop();
        }
    }

    void TagDescriptorSettingsElement::apply_type_to_particles(uint32_t particle_tag)
    {
        auto collection = ui_data.window().simulator_visualizer_bundle.simulator->data.collection;
        FLUID_ASSERT(collection != nullptr);

        for (size_t i = 0; i < collection->size(); i++)
        {
            auto &pi = collection->get<LibFluid::ParticleInfo>(i);
            if (pi.tag == particle_tag)
            {
                pi.type = particle_type;
            }
        }

        ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        ui_data.window().simulator_visualizer_bundle.simulator->initialize();
    }
} // namespace FluidStudio
