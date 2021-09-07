#pragma once

#include "Forward.hpp"
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


      private:
        YAML::Node save_particle_spawner(const std::shared_ptr<ParticleSpawner>& spawner);
        std::shared_ptr<ParticleSpawner> load_particle_spawner(const YAML::Node& node);
        YAML::Node save_particle_remover(const std::shared_ptr<ParticleRemover>& remover);
        std::shared_ptr<ParticleRemover> load_particle_remover(const YAML::Node& node);
        YAML::Node save_global_density_sensor(const std::shared_ptr<Sensors::GlobalDensitySensor>& sen);
        std::shared_ptr<Sensors::GlobalDensitySensor> load_global_density_sensor(const YAML::Node& node);
        YAML::Node save_global_pressure_sensor(const std::shared_ptr<Sensors::GlobalPressureSensor>& sen);
        std::shared_ptr<Sensors::GlobalPressureSensor> load_global_pressure_sensor(const YAML::Node& node);
        YAML::Node save_global_velocity_sensor(const std::shared_ptr<Sensors::GlobalVelocitySensor>& sen);
        std::shared_ptr<Sensors::GlobalVelocitySensor> load_global_velocity_sensor(const YAML::Node& node);
        YAML::Node save_global_energy_sensor(const std::shared_ptr<Sensors::GlobalEnergySensor>& sen);
        std::shared_ptr<Sensors::GlobalEnergySensor> load_global_energy_sensor(const YAML::Node& node);
        YAML::Node save_global_particle_count_sensor(const std::shared_ptr<Sensors::GlobalParticleCountSensor>& sen);
        std::shared_ptr<Sensors::GlobalParticleCountSensor> load_global_particle_count_sensor(const YAML::Node& node);
        std::string get_full_particle_data_path();
        YAML::Node save_scenario(const Simulation& simulation);
        void load_scenario(const YAML::Node& node, Simulation& simulation);
        YAML::Node save_timestep(const std::shared_ptr<ITimestep>& timestep);
        std::shared_ptr<ITimestep> load_timestep(const YAML::Node& node);
        YAML::Node save_solver(const Simulation& simulation);
        void load_solver(Simulation& simulation, const YAML::Node& node);
        std::shared_ptr<ISimulationVisualizer> load_visualizer(const YAML::Node& node);
        YAML::Node save_visualizer(const std::shared_ptr<ISimulationVisualizer>& visualizer);
        YAML::Node save_sensor_plane(const std::shared_ptr<Sensors::SensorPlane>& sen);
        std::shared_ptr<Sensors::SensorPlane> load_sensor_plane(const YAML::Node& node);

        YAML::Node save_compressed_neighborhood_storage_sensor(
            const std::shared_ptr<Sensors::CompressedNeighborStorageSensor>& sen);
        std::shared_ptr<Sensors::CompressedNeighborStorageSensor> load_compressed_neighborhood_storage_sensor(
            const YAML::Node& node);
    };

} // namespace FluidSolver
