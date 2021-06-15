#pragma once

#include "Simulation.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

#include <memory>
#include <optional>
#include <yaml-cpp/yaml.h>

namespace FluidSolver
{

    class SimulationSerializer {
      private:
        size_t error_count = 0;
        size_t warning_count = 0;

      public:
        std::string filepath;

        struct Settings
        {
            bool save_particle_data = true;
            std::string particle_data_relative_filepath = "particles.data";
        } settings;

        explicit SimulationSerializer(const std::string& filepath);

        SimulationSerializer(const std::string& filepath, const Settings& settings);

        bool has_errors() const;
        bool has_warnings() const;

        Simulation load_from_file();

        void save_to_file(const Simulation& simulation);

        virtual ~SimulationSerializer()
        {
        }

      public:
        virtual std::shared_ptr<ISimulationVisualizer> deserialize_unknown_visualizer(const YAML::Node&);

        virtual std::optional<YAML::Node> serialize_unknown_visualizer(const std::shared_ptr<ISimulationVisualizer>&);


      public:
        static void load_particles(ParticleCollection& collection, const std::string& filepath);

        static void save_particles(ParticleCollection& collection, const std::string& filepath);
    };

} // namespace FluidSolver
