#include "MainSerializer.hpp"

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

            // TODO: implement

            //config["scenario"] = save_scenario();
            //config["solver"] = save_solver();

            //config["visualizer"] = save_visualizer(simulation.visualizer);
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