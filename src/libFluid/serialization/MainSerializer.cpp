#include "MainSerializer.hpp"

#include "serialization/serializers/ScenarioSerializer.hpp"
#include "serialization/serializers/SolverSerializer.hpp"
#include "serialization/serializers/VisualizerSerializer.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization {

    MainSerializer::MainSerializer(const std::filesystem::path& filepath) {
        this->filepath = filepath;
    }

    void MainSerializer::serialize_bundle(SimulatorVisualizerBundle& bundle, SerializationContext* output_context) {
        SerializationContext internal_context;

        // set context variables
        {
            internal_context.json_filepath = filepath;
        }

        // start serializing
        nlohmann::json config;
        {
            // set version
            config["version"] = 1;

            // save values
            ScenarioSerializer scenario_serializer(internal_context);
            config["scenario"] = scenario_serializer.serialize(bundle.simulator);

            SolverSerializer solver_serializer(internal_context);
            config["solver"] = solver_serializer.serialize(bundle.simulator->data.fluid_solver);

            VisualizerSerializer visualizer_serializer(internal_context);
            config["visualizer"] = visualizer_serializer.serialize(bundle.visualizer);
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

    void MainSerializer::serialize_simulator_only(std::shared_ptr<Simulator> simulator, SerializationContext* output_context) {
        SimulatorVisualizerBundle bundle;
        bundle.simulator = simulator;
        bundle.visualizer = nullptr;

        serialize_bundle(bundle, output_context);
    }

    SimulatorVisualizerBundle MainSerializer::deserialize(SerializationContext* output_context) {
        SerializationContext internal_context;

        // TODO: implement

        if (output_context != nullptr) {
            *output_context = std::move(internal_context);
        }

        return SimulatorVisualizerBundle();
    }
} // namespace LibFluid::Serialization