
#include "Log.hpp"
#include "Simulation.hpp"
#include "serialization/SimulationSerializer.hpp"

#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <string>


void printHelp(cxxopts::Options& options)
{
    std::cout << std::endl
              << options.help({
                     "",
                     "Console",
                 })
              << std::endl;
}

void dump_particle_data(const std::shared_ptr<FluidSolver::ParticleCollection>& collection,
                        std::filesystem::path filepath)
{
    if (!std::filesystem::exists(filepath.parent_path()))
    {
        std::filesystem::create_directories(filepath.parent_path());
    }

    FluidSolver::SimulationSerializer::save_particles(*collection, filepath.string());
}

int main(int argc, char* argv[])
{
    FluidSolver::Log::print_to_console = true;


    cxxopts::Options options("FluidConsole", "SPH Fluid Solver console application. Allows simulating scenarios "
                                             "without a user interface or input by the user.");
    options.add_options()("h,help", "Show help and information about the FluidConsole application.");
    options.add_options("Console")("l,length", "Amount of time (in seconds) the scenario should be simulated.",
                                   cxxopts::value<float>()->default_value("60.0"))(
        "v,verbose", "If this flag is provided more information is printed to the console",
        cxxopts::value<
            bool>())("f,file", "[Required] Filepath to a scenario yaml file",
                     cxxopts::value<
                         std::string>())("o,output",
                                         "Path to the output folder which will later contain the sensor data.",
                                         cxxopts::value<std::string>()->default_value(
                                             "./output"))("d,dump",
                                                          "Amount of time (in seconds) the particle data will be "
                                                          "dumped into a file. If set to zero (the default value) "
                                                          "nothing will be saved.",
                                                          cxxopts::value<float>()->default_value("0.0"));


    try
    {
        // parse the parameters
        auto result = options.parse(argc, argv);

        if (result["help"].as<bool>())
        {
            printHelp(options);
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

        try
        {
            settings.simulation_length = result["length"].as<float>();
            settings.verbose = result["verbose"].as<bool>();
            settings.filepath = result["file"].as<std::string>();
            settings.outputPath = result["output"].as<std::string>();
            settings.dump_every = result["dump"].as<float>();
        }
        catch (const std::exception& e)
        {
            FluidSolver::Log::error("[Console] Invalid or missing arguments: " + std::string(e.what()));
            return 5;
        }

        if (settings.dump_every != 0.0f)
        {
            settings.enable_particle_data_dump = true;
        }


        if (!std::filesystem::exists(settings.filepath))
        {
            FluidSolver::Log::error("[Console] Specified simulation file does not exist!");
            return 6;
        }


        if (settings.verbose)
            FluidSolver::Log::message("[Console] Starting in console mode.");

        // Load file
        FluidSolver::SimulationSerializer s(settings.filepath);
        FluidSolver::Simulation simulation = s.load_from_file();
        if (s.has_errors())
        {
            FluidSolver::Log::error("[Console] Loading of scenario caused errors!");
            return 3;
        }

        simulation.parameters.output.parameters.output_folder = settings.outputPath;


        // check compatibility
        if (settings.verbose)
            FluidSolver::Log::message("[Console] Checking if components are compatibile with each other.");

        simulation.manual_initialize();
        auto compatibility_report = simulation.check();
        if (compatibility_report.has_issues())
        {
            compatibility_report.log_issues();
            return 4;
        }


        // start simulating
        if (settings.verbose)
            FluidSolver::Log::message("[Console] Starting simulation process.");

        // simulate
        float last_time_message = 0.0f;

        // set up particle data dumps
        float last_time_dump = 0.0f;
        size_t dump_counter = 0;
        if (settings.enable_particle_data_dump)
        {
            if (settings.verbose)
            {
                FluidSolver::Log::message("[Console] Dumping initial particle data to file.");
            }
            dump_particle_data(simulation.parameters.collection, std::filesystem::path(settings.outputPath) /
                                                                     "particle-data" /
                                                                     (std::to_string(dump_counter) + ".data"));
            dump_counter++;
        }


        while (simulation.get_current_timepoint().simulation_time <= settings.simulation_length)
        {
            simulation.execute_simulation_step();

            // informational log messages
            if (simulation.get_current_timepoint().simulation_time >= last_time_message)
            {
                if (settings.verbose)
                    FluidSolver::Log::message("[Console] Simulated " + std::to_string(last_time_message) +
                                              " seconds of the simulation.");
                last_time_message += 1.0f;
            }

            // check particle data dump status
            if (settings.enable_particle_data_dump)
            {
                last_time_dump += simulation.get_current_timepoint().current_time_step;
                if (last_time_dump >= settings.dump_every)
                {
                    if (settings.verbose)
                    {
                        FluidSolver::Log::message("[Console] Dumping particle data to file.");
                    }

                    last_time_dump = std::fmodf(last_time_dump, settings.dump_every);
                    dump_particle_data(simulation.parameters.collection, std::filesystem::path(settings.outputPath) /
                                                                             "particle-data" /
                                                                             (std::to_string(dump_counter) + ".data"));
                    dump_counter++;
                }
            }
        }

        // manual save of the output
        simulation.parameters.output.manual_save();

        FluidSolver::Log::message("[Console] Simulation has finished.");


        return 0;
    }
    catch (cxxopts::option_not_exists_exception& exc)
    {
        FluidSolver::Log::print_to_console = true;
        FluidSolver::Log::error(exc.what());
        printHelp(options);
        return 2;
    }
}