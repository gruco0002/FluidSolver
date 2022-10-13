#include "MainSerializer.hpp"

#include "serialization/ParticleSerializer.hpp"
#include "serialization/serializers/ScenarioSerializer.hpp"
#include "serialization/serializers/SolverSerializer.hpp"
#include "serialization/serializers/TimestepGeneratorSerializer.hpp"
#include "serialization/serializers/VisualizerSerializer.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

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

        // TODO: implement

        if (output_context != nullptr) {
            *output_context = std::move(internal_context);
        }

        return SimulatorVisualizerBundle();
    }

    std::filesystem::path MainSerializer::get_full_particle_data_path(const SerializationContext& context) const {
        auto p = std::filesystem::path(filepath);
        auto combined = p.parent_path() / context.particle_data_relative_filepath;
        return std::move(combined);
    }

} // namespace LibFluid::Serialization