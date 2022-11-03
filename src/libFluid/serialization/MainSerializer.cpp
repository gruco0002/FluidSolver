#include "MainSerializer.hpp"

#include "serialization/ParticleSerializer.hpp"
#include "serialization/extensions/RootSerializerExtension.hpp"
#include "serialization/serializers/ScenarioSerializer.hpp"
#include "serialization/serializers/SolverSerializer.hpp"
#include "serialization/serializers/TagDescriptorsSerializer.hpp"
#include "serialization/serializers/TimestepGeneratorSerializer.hpp"
#include "serialization/serializers/VisualizerSerializer.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>

namespace LibFluid::Serialization {

    MainSerializer::MainSerializer(const std::filesystem::path& filepath) {
        this->filepath = filepath;
    }

    MainSerializer::MainSerializer(const std::filesystem::path& filepath, SerializerExtensions serializer_extensions) {
        this->filepath = filepath;
        this->serializer_extensions = serializer_extensions;
    }

    void MainSerializer::serialize_bundle(SimulatorVisualizerBundle& bundle, const SerializeSettings& settings, SerializationContext* output_context) {
        SerializationContext internal_context;

        // set context variables
        {
            internal_context.json_filepath = filepath;
            internal_context.particle_data_relative_filepath = settings.particle_data_relative_filepath;
        }

        // check if we need to save the particles previously
        if (settings.save_particle_data) {
            Serialization::ParticleSerializer ser(get_full_particle_data_path(internal_context));
            auto collection = bundle.simulator->data.collection;
            ser.serialize(*collection);
        }

        // start serializing
        nlohmann::json config;
        {
            // set version
            config["version"] = 2;

            // save values
            ScenarioSerializer scenario_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("scenario");
            config["scenario"] = scenario_serializer.serialize(bundle.simulator);
            internal_context.end_section();


            SolverSerializer solver_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("solver");
            config["solver"] = solver_serializer.serialize(bundle.simulator->data.fluid_solver);
            internal_context.end_section();


            TimestepGeneratorSerializer timestep_generator_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("timestep-generator");
            config["timestep-generator"] = timestep_generator_serializer.serialize(bundle.simulator->data.timestep_generator);
            internal_context.end_section();


            VisualizerSerializer visualizer_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("visualizer");
            config["visualizer"] = visualizer_serializer.serialize(bundle.visualizer);
            internal_context.end_section();

            TagDescriptorsSerializer tag_descriptors_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("tag-descriptors");
            config["tag-descriptors"] = tag_descriptors_serializer.serialize(bundle.simulator->data.tag_descriptors);
            internal_context.end_section();

            // serialize using root extensions
            {
                for (const auto& ext : this->serializer_extensions.root_serializer_extensions) {
                    auto node_name = ext->get_node_name();

                    internal_context.begin_section(node_name);

                    auto node = ext->serialize(internal_context);
                    if (node.has_value()) {
                        config[node_name] = std::move(node.value());
                    }

                    internal_context.end_section();
                }
            }
        }

        // write to file
        {
            std::ofstream filestream(internal_context.json_filepath);
            filestream << std::setw(4) << config << std::endl;
        }

        // move context to output if required
        if (output_context != nullptr) {
            *output_context = std::move(internal_context);
        }
    }

    void MainSerializer::serialize_simulator_only(std::shared_ptr<Simulator> simulator, const SerializeSettings& settings, SerializationContext* output_context) {
        SimulatorVisualizerBundle bundle;
        bundle.simulator = std::move(simulator);
        bundle.visualizer = nullptr;

        serialize_bundle(bundle, settings, output_context);
    }

    SimulatorVisualizerBundle MainSerializer::deserialize(SerializationContext* output_context) {
        SerializationContext internal_context;
        bool has_fatal_error = false;

        // create empty result
        SimulatorVisualizerBundle bundle;

        // load data from file
        nlohmann::json config;
        {
            std::ifstream stream(filepath);
            stream >> config;
        }

        // set context variables that we know of
        {
            internal_context.json_filepath = filepath;
        }

        // check the file version
        if (!config.contains("version") || config["version"].get<int>() != 2) {
            internal_context.add_issue("Invalid file version number!");
            has_fatal_error = true;
        }

        // start deserializing
        if (!has_fatal_error) {
            ScenarioSerializer scenario_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("scenario");
            bundle.simulator = scenario_serializer.deserialize(config["scenario"]);
            internal_context.end_section();


            SolverSerializer solver_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("solver");
            bundle.simulator->data.fluid_solver = solver_serializer.deserialize(config["solver"]);
            internal_context.end_section();


            TimestepGeneratorSerializer timestep_generator_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("timestep-generator");
            bundle.simulator->data.timestep_generator = timestep_generator_serializer.deserialize(config["timestep-generator"]);
            internal_context.end_section();


            VisualizerSerializer visualizer_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("visualizer");
            bundle.visualizer = visualizer_serializer.deserialize(config["visualizer"]);
            internal_context.end_section();

            TagDescriptorsSerializer tag_descriptors_serializer(internal_context, serializer_extensions);
            internal_context.begin_section("tag-descriptors");
            if (config.contains("tag-descriptors")) {
                bundle.simulator->data.tag_descriptors = tag_descriptors_serializer.deserialize(config["tag-descriptors"]);
            } else {
                bundle.simulator->data.tag_descriptors = std::make_shared<TagDescriptors>();
            }
            internal_context.end_section();

            // deserialize using root extensions
            {
                for (const auto& ext : serializer_extensions.root_serializer_extensions) {
                    auto node_name = ext->get_node_name();

                    internal_context.begin_section(node_name);

                    std::optional<nlohmann::json> node;
                    if (config.contains(node_name)) {
                        node = config[node_name];
                    }

                    ext->deserialize(node, internal_context);

                    internal_context.end_section();
                }
            }
        }

        // deserialize particle data
        {
            // create the particle collection
            bundle.simulator->data.collection = std::make_shared<ParticleCollection>();

            // try to load the particles from file
            auto particle_file_full_path = get_full_particle_data_path(internal_context);
            if (std::filesystem::exists(particle_file_full_path)) {
                ParticleSerializer particle_serializer(particle_file_full_path);
                particle_serializer.deserialize(*bundle.simulator->data.collection);
            } else {
                internal_context.add_issue("Could not find particle file!");
                has_fatal_error = true;
            }
        }

        // move context to output if required
        if (output_context != nullptr) {
            *output_context = std::move(internal_context);
        }

        return bundle;
    }

    std::filesystem::path MainSerializer::get_full_particle_data_path(const SerializationContext& context) const {
        auto p = std::filesystem::path(filepath);
        auto combined = p.parent_path() / context.particle_data_relative_filepath;
        return std::move(combined);
    }

} // namespace LibFluid::Serialization