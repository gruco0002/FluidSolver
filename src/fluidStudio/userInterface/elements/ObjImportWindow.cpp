#include "ObjImportWindow.hpp"

#include "ImguiHelper.hpp"
#include "LibFluidMath.hpp"
#include "helpers/Log.hpp"
#include "importer/ObjLoader.hpp"
#include "importer/ParticleSampler.hpp"
#include "importer/methods/DistanceReductionMethod.hpp"
#include "importer/methods/DuplicateReductionMethod.hpp"
#include "importer/methods/GridSamplingMethod.hpp"
#include "importer/methods/UvSamplingMethod.hpp"
#include "importer/methods/VolumeReductionMethod.hpp"
#include "userInterface/helpers/ParticleCollectionHelper.hpp"

#include <fmt/core.h>
#include <nfd.h>

namespace FluidStudio {

    void ObjImportWindow::update() {
        auto popup_id = update_report_popup();

        if (!visible)
            return;


        if (ImGui::Begin("Obj Import", &visible)) {
            bool import_enabled = can_import();

            ImGui::Text("Import obj data into an existing scene.");
            ImGui::Separator();

            ImGui::Text("Current file: %s", current_file.c_str());
            ImGui::SameLine();
            if (ImGui::Button("Open")) {
                char* p = nullptr;
                auto res = NFD_OpenDialog("obj", nullptr, &p);
                if (res == NFD_OKAY) {
                    std::string path(p);
                    free(p);

                    this->current_file = path;
                }
            }

            ImGui::Separator();

            ImGui::InputFloat("Model Scale", &import_scale);
            ImGui::InputInt("Tag", reinterpret_cast<int*>(&particle_tag));

            ImGui::Separator();
            if (ImGui::BeginCombo("Sampling Method", sampler == Sampler::UvSampler ? "UV-Sampler" : "Grid-Sampler")) {
                if (ImGui::Selectable("UV-Sampler", sampler == Sampler::UvSampler)) {
                    sampler = Sampler::UvSampler;
                }
                if (ImGui::Selectable("Grid-Sampler", sampler == Sampler::GridSampler)) {
                    sampler = Sampler::GridSampler;
                }
                ImGui::EndCombo();
            }
            ImGui::Checkbox("Duplicate Reduction", &duplicate_reduction_enabled);
            ImGui::Checkbox("Distance Reduction", &distance_reduction_enabled);
            ImGui::Checkbox("Volume Reduction", &volume_reduction_enabled);

            ImGui::Separator();
            ImGui::Checkbox("Randomly shuffle samples before reduction", &shuffle_before_each_reduction);
            ImGui::Checkbox("Use random seed for shuffling", &use_random_seed_for_shuffling);

            ImGui::Separator();
            ImGui::LabelText("Particle Size (m)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size);
            ImGui::LabelText("Rest Density (kg/m^3)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density);
            float particle_mass = get_particle_mass();
            ImGui::LabelText("Particle Mass (kg)", "%.3f", particle_mass);

            ImGui::Separator();
            ImGui::Checkbox("Show Report after Import", &report_enabled);
            ImGui::Separator();


            if (!import_enabled) {
                glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);
                ImGui::TextColored(reinterpret_cast<const ImVec4&>(color), "Cannot import 3d data into a scene that is not setup for 3d data!");
            }

            if (ImGui::Button("Import into Scene") && import_enabled) {
                import_data_into_scene();
                reset();
                visible = false;

                if (report_enabled) {
                    show_report_popup(popup_id);
                }
            }
        }
        ImGui::End();
    }

    void ObjImportWindow::open_window() {
        visible = true;

        uint32_t next_free_tag = ParticleCollectionHelper::get_next_free_tag(ui_data.window().simulator_visualizer_bundle.simulator->data.collection);
        particle_tag = next_free_tag;
    }

    float ObjImportWindow::get_particle_mass() const {
        float particle_size = ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size;
        float rest_density = ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density;
        float particle_mass = LibFluid::Math::pow3(particle_size) * rest_density;
        return particle_mass;
    }

    void ObjImportWindow::reset() {
        import_scale = 1.0;
        current_file = "";
        particle_tag = 0;
        report_enabled = true;
        sampler = Sampler::UvSampler;
        duplicate_reduction_enabled = true;
        volume_reduction_enabled = true;
        distance_reduction_enabled = false;
        shuffle_before_each_reduction = false;
        use_random_seed_for_shuffling = false;
    }

    void ObjImportWindow::import_data_into_scene() {
        float particle_size = ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size;
        float rest_density = ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density;
        float particle_mass = get_particle_mass();

        auto collection = ui_data.window().simulator_visualizer_bundle.simulator->data.collection;

        LibFluid::Importer::ObjLoader obj_loader(current_file);
        obj_loader.scale = import_scale;
        auto mesh_data = obj_loader.load_as_meshdata();

        LibFluid::Importer::ParticleSampler particle_sampler;
        particle_sampler.shuffle_before_each_reduction = shuffle_before_each_reduction;
        particle_sampler.use_random_seed_for_shuffling = use_random_seed_for_shuffling;

        switch (sampler) {
            case Sampler::GridSampler:
                particle_sampler.sampling_method = std::make_shared<LibFluid::Importer::GridSamplingMethod>();
                break;
            case Sampler::UvSampler:
                particle_sampler.sampling_method = std::make_shared<LibFluid::Importer::UVSamplingMethod>();
                break;
        }


        if (duplicate_reduction_enabled) {
            particle_sampler.reduction_methods.push_back(std::make_shared<LibFluid::Importer::DuplicateReductionMethod>());
        }
        if (distance_reduction_enabled) {
            particle_sampler.reduction_methods.push_back(std::make_shared<LibFluid::Importer::DistanceReductionMethod>());
        }
        if (volume_reduction_enabled) {
            particle_sampler.reduction_methods.push_back(std::make_shared<LibFluid::Importer::VolumeReductionMethod>());
        }


        const auto& samples = particle_sampler.generate_samples(mesh_data, particle_size);

        report_data.created_particles = samples.size();
        report_data.area = mesh_data.get_area();
        report_data.triangle_count = mesh_data.triangles.size();
        report_data.import_scale = import_scale;
        report_data.use_random_seed_for_shuffling = use_random_seed_for_shuffling;
        report_data.shuffle_before_each_reduction = shuffle_before_each_reduction;

        // convert samples into particles
        for (const auto& sample : samples) {
            size_t index = collection->add();

            auto& pos = collection->get<LibFluid::MovementData3D>(index);
            pos.position = sample;
            pos.acceleration = glm::vec3(0.0f);
            pos.velocity = glm::vec3(0.0f);

            auto& info = collection->get<LibFluid::ParticleInfo>(index);

            info.type = LibFluid::ParticleType::ParticleTypeBoundary;
            info.tag = particle_tag;

            auto& data = collection->get<LibFluid::ParticleData>(index);
            data.density = rest_density;
            data.mass = particle_mass;
            data.pressure = 0.0f;
        }

        // add particle tag descriptor
        {
            LibFluid::TagDescriptors::Descriptor d;
            d.particle_tag = particle_tag;
            d.description = "Imported from " + std::filesystem::path(current_file).filename().string();
            d.title = std::filesystem::path(current_file).filename().string();
            ui_data.window().simulator_visualizer_bundle.simulator->data.tag_descriptors->descriptors.push_back(d);
        }


        // notify data structures of change
        ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        ui_data.window().simulator_visualizer_bundle.initialize();

        ui_data.window().timeline_service.override_timestep_result();

        ui_data.window().visualizer_parameter_changed();

        // log info about import
        LibFluid::Log::message(fmt::format("Imported {}:", current_file));
        LibFluid::Log::message(fmt::format("Model was scaled up by factor {}.", report_data.import_scale));
        LibFluid::Log::message(fmt::format("Model has {} triangles with a surface area of {}.", report_data.triangle_count, report_data.area));
        LibFluid::Log::message(fmt::format("This resulted in {} particles.", report_data.created_particles));
        LibFluid::Log::message(fmt::format("Shuffle before each reduction: {}, use a random seed for shuffling {}", report_data.shuffle_before_each_reduction, report_data.use_random_seed_for_shuffling));
    }
    bool ObjImportWindow::can_import() const {
        auto& collection = ui_data.window().simulator_visualizer_bundle.simulator->data.collection;
        return collection->is_type_present<LibFluid::MovementData3D>() && collection->is_type_present<LibFluid::ParticleInfo>() && collection->is_type_present<LibFluid::ParticleData>();
    }

    unsigned int ObjImportWindow::update_report_popup() {
        auto imgui_id = ImGui::GetID("Obj Import - Report");
        if (ImGui::BeginPopupModal("Obj Import - Report")) {
            ImGui::LabelText("Created Particles", "%d", report_data.created_particles);
            ImGui::LabelText("Model Scale", "%f", report_data.import_scale);
            ImGui::LabelText("Amount of Triangles", "%d", report_data.triangle_count);
            ImGui::LabelText("Model Surface Area", "%f", report_data.area);
            ImGui::LabelText("Shuffle before each reduction", "%s", report_data.shuffle_before_each_reduction ? "Enabled" : "Disabled");
            ImGui::LabelText("Use random seed for shuffling", "%s", report_data.use_random_seed_for_shuffling ? "Enabled" : "Disabled");

            ImGui::Separator();

            ImGui::LabelText("Particle Size (m)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size);
            ImGui::LabelText("Rest Density (kg/m^3)", "%.3f", ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density);
            float particle_mass = get_particle_mass();
            ImGui::LabelText("Particle Mass (kg)", "%.3f", particle_mass);

            ImGui::Separator();

            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        return imgui_id;
    }

    void ObjImportWindow::show_report_popup(unsigned int id) {
        ImGui::OpenPopup(id);
    }

} // namespace FluidStudio
