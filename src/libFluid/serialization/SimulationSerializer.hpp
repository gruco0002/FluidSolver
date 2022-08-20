#pragma once

#include "Forward.hpp"
#include "Simulator.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

#include <memory>
#include <optional>
#include <nlohmann/json.hpp>

namespace LibFluid {

    class SimulationSerializer {
      private:
        size_t error_count = 0;
        size_t warning_count = 0;

      public:
        std::string filepath;

        struct Settings {
            bool save_particle_data = true;
            std::string particle_data_relative_filepath = "particles.data";
        } settings;

        explicit SimulationSerializer(const std::string& filepath);

        SimulationSerializer(const std::string& filepath, const Settings& settings);

        bool has_errors() const;
        bool has_warnings() const;

        SimulatorVisualizerBundle load_from_file();

        void save_to_file(const SimulatorVisualizerBundle& simulation);

        virtual ~SimulationSerializer() {
        }

      public:
        virtual std::shared_ptr<ISimulationVisualizer> deserialize_unknown_visualizer(const nlohmann::json&);

        virtual std::optional<nlohmann::json> serialize_unknown_visualizer(const std::shared_ptr<ISimulationVisualizer>&);


      public:
        static void load_particles(ParticleCollection& collection, const std::string& filepath);

        static void save_particles(ParticleCollection& collection, const std::string& filepath);


      private:
        nlohmann::json save_particle_spawner(const std::shared_ptr<ParticleSpawner>& spawner);
        std::shared_ptr<ParticleSpawner> load_particle_spawner(const nlohmann::json& node);

        nlohmann::json save_particle_remover(const std::shared_ptr<ParticleRemover>& remover);
        std::shared_ptr<ParticleRemover> load_particle_remover(const nlohmann::json& node);

        nlohmann::json save_particle_remover_3d(const std::shared_ptr<ParticleRemover3D>& remover);
        std::shared_ptr<ParticleRemover3D> load_particle_remover_3d(const nlohmann::json& node);



        nlohmann::json save_global_density_sensor(const std::shared_ptr<Sensors::GlobalDensitySensor>& sen);
        std::shared_ptr<Sensors::GlobalDensitySensor> load_global_density_sensor(const nlohmann::json& node);
        nlohmann::json save_global_pressure_sensor(const std::shared_ptr<Sensors::GlobalPressureSensor>& sen);
        std::shared_ptr<Sensors::GlobalPressureSensor> load_global_pressure_sensor(const nlohmann::json& node);
        nlohmann::json save_global_velocity_sensor(const std::shared_ptr<Sensors::GlobalVelocitySensor>& sen);
        std::shared_ptr<Sensors::GlobalVelocitySensor> load_global_velocity_sensor(const nlohmann::json& node);
        nlohmann::json save_global_energy_sensor(const std::shared_ptr<Sensors::GlobalEnergySensor>& sen);
        std::shared_ptr<Sensors::GlobalEnergySensor> load_global_energy_sensor(const nlohmann::json& node);
        nlohmann::json save_global_particle_count_sensor(const std::shared_ptr<Sensors::GlobalParticleCountSensor>& sen);
        std::shared_ptr<Sensors::GlobalParticleCountSensor> load_global_particle_count_sensor(const nlohmann::json& node);
        std::string get_full_particle_data_path();
        nlohmann::json save_scenario(const Simulator& simulation);
        void load_scenario(const nlohmann::json& node, Simulator& simulation);
        nlohmann::json save_timestep(const std::shared_ptr<ITimestepGenerator>& timestep);
        std::shared_ptr<ITimestepGenerator> load_timestep(const nlohmann::json& node);
        nlohmann::json save_solver(const Simulator& simulation);
        void load_solver(Simulator& simulation, const nlohmann::json& node);
        std::shared_ptr<ISimulationVisualizer> load_visualizer(const nlohmann::json& node);
        nlohmann::json save_visualizer(const std::shared_ptr<ISimulationVisualizer>& visualizer);
        nlohmann::json save_sensor_plane(const std::shared_ptr<Sensors::SensorPlane>& sen);
        std::shared_ptr<Sensors::SensorPlane> load_sensor_plane(const nlohmann::json& node);

        nlohmann::json save_compressed_neighborhood_storage_sensor(
                const std::shared_ptr<Sensors::CompressedNeighborStorageSensor>& sen);
        std::shared_ptr<Sensors::CompressedNeighborStorageSensor> load_compressed_neighborhood_storage_sensor(
                const nlohmann::json& node);

        nlohmann::json save_iisph_sensor(const std::shared_ptr<Sensors::IISPHSensor>& sen);
        std::shared_ptr<Sensors::IISPHSensor> load_iisph_sensor(const nlohmann::json& node);
    };

} // namespace FluidSolver
