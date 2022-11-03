
#include "FluidConsoleSerializerExtensions.hpp"
#include "Simulator.hpp"
#include "SimulatorVisualizerBundle.hpp"
#include "helpers/Log.hpp"
#include "serialization/MainSerializer.hpp"
#include "serialization/ParticleSerializer.hpp"

#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <string>


void print_help(cxxopts::Options& options) {
    std::cout << std::endl
              << options.help({
                         "",
                         "Console",
                 })
              << std::endl;
}

void dump_particle_data(const std::shared_ptr<LibFluid::ParticleCollection>& collection,
        std::filesystem::path filepath) {
    if (!std::filesystem::exists(filepath.parent_path())) {
        std::filesystem::create_directories(filepath.parent_path());
    }

    LibFluid::Serialization::ParticleSerializer particle_serializer(filepath);
    particle_serializer.serialize(*collection);
}

int main(int argc, char* argv[]) {
    LibFluid::Log::print_to_console = true;


    cxxopts::Options options("FluidConsole", "SPH Fluid Solver console application. Allows simulating scenarios "
                                             "without a user interface or input by the user.");
    options.add_options()("h,help", "Show help and information about the FluidConsole application.");
    options.add_options("Console")("l,length", "Amount of time (in seconds) the scenario should be simulated.",
            cxxopts::value<float>()->default_value("60.0"))(
            "v,verbose", "If this flag is provided more information is printed to the console",
            cxxopts::value<
                    bool>())("f,file", "[Required] Filepath to a scenario json file",
            cxxopts::value<
                    std::string>())("o,output",
            "Path to the output folder which will later contain the sensor data.",
            cxxopts::value<std::string>()->default_value(
                    "./output"))("d,dump",
            "Amount of time (in seconds) the particle data will be "
            "dumped into a file. If set to zero (the default value) "
            "nothing will be saved.",
            cxxopts::value<float>()->default_value("0.0"));


    try {
        // parse the parameters
        auto result = options.parse(argc, argv);

        if (result["help"].as<bool>()) {
            print_help(options);
            return 0;
        }

        struct
        {
            float simulation_length = 1.0f;
            bool verbose = false;
            std::string filepath = "";
            std::string outputPath = "";
            float dump_every = 0.0f;
            bool enable_particle_data_dump = false;
        } settings;

        try {
            settings.simulation_length = result["length"].as<float>();
            settings.verbose = result["verbose"].as<bool>();
            settings.filepath = result["file"].as<std::string>();
            settings.outputPath = result["output"].as<std::string>();
            settings.dump_every = result["dump"].as<float>();
        } catch (const std::exception& e) {
            LibFluid::Log::error("[Console] Invalid or missing arguments: " + std::string(e.what()));
            return 5;
        }

        if (settings.dump_every != 0.0f) {
            settings.enable_particle_data_dump = true;
        }


        if (!std::filesystem::exists(settings.filepath)) {
            LibFluid::Log::error("[Console] Specified simulation file does not exist!");
            return 6;
        }


        if (settings.verbose)
            LibFluid::Log::message("[Console] Starting in console mode.");

        // Load file
        LibFluid::Serialization::SerializationContext context_output;

        auto serializer_extensions = FluidConsole::FluidConsoleSerializerExtensions::create_extensions();
        LibFluid::Serialization::MainSerializer serializer(settings.filepath, serializer_extensions);
        LibFluid::SimulatorVisualizerBundle bundle = serializer.deserialize(&context_output);

        if (!context_output.issues.empty()) {
            LibFluid::Log::error("[Console] Loading of scenario caused errors!");
            for (const auto& issue : context_output.issues) {
                LibFluid::Log::error(issue.to_formatted_string());
            }
            return 3;
        }

        bundle.simulator->output->parameters.output_folder = settings.outputPath;


        // check compatibility
        if (settings.verbose)
            LibFluid::Log::message("[Console] Checking if components are compatibile with each other.");

        bundle.initialize();
        LibFluid::CompatibilityReport report;
        bundle.create_compatibility_report(report);
        if (report.has_issues()) {
            report.log_issues();
            return 4;
        }


        // start simulating
        if (settings.verbose)
            LibFluid::Log::message("[Console] Starting simulation process.");

        // simulate
        float last_time_message = 0.0f;

        // set up particle data dumps
        float last_time_dump = 0.0f;
        size_t dump_counter = 0;
        if (settings.enable_particle_data_dump) {
            if (settings.verbose) {
                LibFluid::Log::message("[Console] Dumping initial particle data to file.");
            }
            dump_particle_data(bundle.simulator->data.collection, std::filesystem::path(settings.outputPath) / "particle-data" / (std::to_string(dump_counter) + ".data"));
            dump_counter++;
        }


        while (bundle.simulator->get_current_timepoint().simulation_time <= settings.simulation_length) {
            bundle.simulator->execute_simulation_step();

            // informational log messages
            if (bundle.simulator->get_current_timepoint().simulation_time >= last_time_message) {
                if (settings.verbose)
                    LibFluid::Log::message("[Console] Simulated " + std::to_string(last_time_message) +
                            " seconds of the simulation.");
                last_time_message += 1.0f;
            }

            // check particle data dump status
            if (settings.enable_particle_data_dump) {
                last_time_dump += bundle.simulator->get_current_timepoint().actual_time_step;
                if (last_time_dump >= settings.dump_every) {
                    if (settings.verbose) {
                        LibFluid::Log::message("[Console] Dumping particle data to file.");
                    }

                    last_time_dump = std::fmodf(last_time_dump, settings.dump_every);
                    dump_particle_data(bundle.simulator->data.collection, std::filesystem::path(settings.outputPath) / "particle-data" / (std::to_string(dump_counter) + ".data"));
                    dump_counter++;
                }
            }
        }


        LibFluid::Log::message("[Console] Simulation has finished.");


        return 0;
    } catch (cxxopts::option_not_exists_exception& exc) {
        LibFluid::Log::print_to_console = true;
        LibFluid::Log::error(exc.what());
        print_help(options);
        return 2;
    }
}