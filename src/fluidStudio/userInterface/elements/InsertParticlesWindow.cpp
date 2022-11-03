#include "InsertParticlesWindow.hpp"

#include "ImguiHelper.hpp"
#include "userInterface/helpers/ParticleCollectionHelper.hpp"
#include "userInterface/helpers/TypeInformationProvider.hpp"
#include "LibFluidMath.hpp"

namespace FluidStudio {

    void InsertParticlesWindow::update() {
        if (!is_open) {
            return;
        }

        if (ImGui::Begin("Insert Particles", &is_open)) {
            ImGui::Text("Insert a cube of particles into the simulation.");
            ImGui::InputInt("Width", reinterpret_cast<int*>(&particle_settings.width));
            ImGui::InputInt("Height", reinterpret_cast<int*>(&particle_settings.height));
            ImGui::InputInt("Depth", reinterpret_cast<int*>(&particle_settings.depth));
            ImGui::InputInt("Tag", reinterpret_cast<int*>(&particle_settings.tag));
            if (ImGui::BeginCombo("Type", TypeInformationProvider::particle_type_to_string(particle_settings.particle_type))) {
                if (ImGui::Selectable(TypeInformationProvider::particle_type_to_string(LibFluid::ParticleType::ParticleTypeNormal),
                            particle_settings.particle_type == LibFluid::ParticleType::ParticleTypeNormal)) {
                    particle_settings.particle_type = LibFluid::ParticleType::ParticleTypeNormal;
                }

                if (ImGui::Selectable(TypeInformationProvider::particle_type_to_string(LibFluid::ParticleType::ParticleTypeBoundary),
                            particle_settings.particle_type == LibFluid::ParticleType::ParticleTypeBoundary)) {
                    particle_settings.particle_type = LibFluid::ParticleType::ParticleTypeBoundary;
                }

                if (ImGui::Selectable(TypeInformationProvider::particle_type_to_string(LibFluid::ParticleType::ParticleTypeInactive),
                            particle_settings.particle_type == LibFluid::ParticleType::ParticleTypeInactive)) {
                    particle_settings.particle_type = LibFluid::ParticleType::ParticleTypeInactive;
                }

                ImGui::EndCombo();
            }

            if (ImGui::Button("Insert")) {
                insert_into_simulation();
                is_open = false;
            }
        }
        ImGui::End();
    }

    void InsertParticlesWindow::open_window() {
        reset();
        is_open = true;

        uint32_t next_free_tag = ParticleCollectionHelper::get_next_free_tag(ui_data.window().simulator_visualizer_bundle.simulator->data.collection);
        particle_settings.tag = next_free_tag;
    }

    void InsertParticlesWindow::reset() {
        particle_settings.width = 10;
        particle_settings.height = 10;
        particle_settings.depth = 10;

        particle_settings.tag = 0;
        particle_settings.particle_type = LibFluid::ParticleTypeBoundary;
    }

    void InsertParticlesWindow::insert_into_simulation() {
        // set the parameters of the simulation to the specified ones
        const auto& parameters = ui_data.window().simulator_visualizer_bundle.simulator->parameters;
        float particle_size = parameters.particle_size;
        float rest_density = parameters.rest_density;
        float particle_mass = LibFluid::Math::pow3(particle_size) * rest_density;


        auto collection = ui_data.window().simulator_visualizer_bundle.simulator->data.collection;

        // insert the particles
        for (int x = 0; x < particle_settings.width; x++) {
            for (int y = 0; y < particle_settings.height; y++) {
                for (int z = 0; z < particle_settings.depth; z++) {
                    size_t index = collection->add();

                    auto& pos = collection->get<LibFluid::MovementData3D>(index);
                    pos.position = glm::vec3((float)x, (float)y, (float)z) * particle_size;
                    pos.acceleration = glm::vec3(0.0f);
                    pos.velocity = glm::vec3(0.0f);

                    auto& info = collection->get<LibFluid::ParticleInfo>(index);

                    info.type = particle_settings.particle_type;
                    info.tag = particle_settings.tag;

                    auto& data = collection->get<LibFluid::ParticleData>(index);
                    data.density = rest_density;
                    data.mass = particle_mass;
                    data.pressure = 0.0f;
                }
            }
        }

        // update the collection
        ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        ui_data.window().simulator_visualizer_bundle.initialize();

        ui_data.window().timeline_service.override_timestep_result();
    }
} // namespace FluidStudio