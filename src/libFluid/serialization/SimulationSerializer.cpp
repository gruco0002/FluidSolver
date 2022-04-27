#include "SimulationSerializer.hpp"

#include "Log.hpp"
#include "entities/ParticleRemover.hpp"
#include "entities/ParticleSpawner.hpp"
#include "fluidSolver/IISPHFluidSolver.hpp"
#include "fluidSolver/IISPHFluidSolver3D.hpp"
#include "fluidSolver/SESPHFluidSolver.hpp"
#include "fluidSolver/SESPHFluidSolver3D.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp"
#include "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp"
#include "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.hpp"
#include "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp"
#include "sensors/CompressedNeighborsStatistics.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorPlane.hpp"
#include "serialization/ParticleSerializer.hpp"
#include "serialization/YamlHelpers.hpp"
#include "time/ConstantTimestepGenerator.hpp"
#include "time/DynamicCflTimestepGenerator.hpp"
#include "visualizer/ContinousVisualizer.hpp"


#include <filesystem>


namespace FluidSolver
{


    YAML::Node SimulationSerializer::save_particle_spawner(const std::shared_ptr<ParticleSpawner>& spawner)
    {
        YAML::Node res;
        res["type"] = "particle-spawner";

        res["position"] = spawner->parameters.position;
        res["direction"] = spawner->parameters.direction;
        res["width"] = spawner->parameters.width;
        res["initial-velocity"] = spawner->parameters.initial_velocity;
        res["rest-density"] = spawner->parameters.rest_density;
        res["mass"] = spawner->parameters.mass;

        return res;
    }

    std::shared_ptr<ParticleSpawner> SimulationSerializer::load_particle_spawner(const YAML::Node& node)
    {
        auto spawner = std::make_shared<ParticleSpawner>();
        spawner->parameters.position = node["position"].as<glm::vec2>();
        spawner->parameters.direction = node["direction"].as<glm::vec2>();
        spawner->parameters.width = node["width"].as<float>();
        spawner->parameters.initial_velocity = node["initial-velocity"].as<float>();
        spawner->parameters.rest_density = node["rest-density"].as<float>();
        spawner->parameters.mass = node["mass"].as<float>();
        return spawner;
    }

    YAML::Node SimulationSerializer::save_particle_remover(const std::shared_ptr<ParticleRemover>& remover)
    {
        YAML::Node res;
        res["type"] = "particle-remover";

        res["area"]["left"] = remover->parameters.area.left;
        res["area"]["right"] = remover->parameters.area.right;
        res["area"]["top"] = remover->parameters.area.top;
        res["area"]["bottom"] = remover->parameters.area.bottom;
        res["remove-if-outside"] = remover->parameters.remove_if_outside;

        return res;
    }


    std::shared_ptr<ParticleRemover> SimulationSerializer::load_particle_remover(const YAML::Node& node)
    {
        auto remover = std::make_shared<ParticleRemover>();

        remover->parameters.area.left = node["area"]["left"].as<float>();
        remover->parameters.area.right = node["area"]["right"].as<float>();
        remover->parameters.area.top = node["area"]["top"].as<float>();
        remover->parameters.area.bottom = node["area"]["bottom"].as<float>();
        remover->parameters.remove_if_outside = node["remove-if-outside"].as<bool>();

        return remover;
    }

    YAML::Node SimulationSerializer::save_global_density_sensor(
        const std::shared_ptr<Sensors::GlobalDensitySensor>& sen)
    {
        YAML::Node node;

        node["type"] = "global-density-sensor";
        node["name"] = sen->parameters.name;
        node["save-to-file"] = sen->parameters.save_to_file;
        node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

        return node;
    }

    std::shared_ptr<Sensors::GlobalDensitySensor> SimulationSerializer::load_global_density_sensor(
        const YAML::Node& node)
    {
        auto res = std::make_shared<Sensors::GlobalDensitySensor>();
        res->parameters.name = node["name"].as<std::string>();
        res->parameters.save_to_file = node["save-to-file"].as<bool>();
        res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();

        return res;
    }

    YAML::Node SimulationSerializer::save_global_pressure_sensor(
        const std::shared_ptr<Sensors::GlobalPressureSensor>& sen)
    {
        YAML::Node node;

        node["type"] = "global-pressure-sensor";
        node["name"] = sen->parameters.name;
        node["save-to-file"] = sen->parameters.save_to_file;
        node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

        return node;
    }

    std::shared_ptr<Sensors::GlobalPressureSensor> SimulationSerializer::load_global_pressure_sensor(
        const YAML::Node& node)
    {
        auto res = std::make_shared<Sensors::GlobalPressureSensor>();
        res->parameters.name = node["name"].as<std::string>();
        res->parameters.save_to_file = node["save-to-file"].as<bool>();
        res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();

        return res;
    }

    YAML::Node SimulationSerializer::save_global_velocity_sensor(
        const std::shared_ptr<Sensors::GlobalVelocitySensor>& sen)
    {
        YAML::Node node;

        node["type"] = "global-velocity-sensor";
        node["name"] = sen->parameters.name;
        node["save-to-file"] = sen->parameters.save_to_file;
        node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

        return node;
    }

    std::shared_ptr<Sensors::GlobalVelocitySensor> SimulationSerializer::load_global_velocity_sensor(
        const YAML::Node& node)
    {
        auto res = std::make_shared<Sensors::GlobalVelocitySensor>();
        res->parameters.name = node["name"].as<std::string>();
        res->parameters.save_to_file = node["save-to-file"].as<bool>();
        res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();

        return res;
    }

    YAML::Node SimulationSerializer::save_sensor_plane(const std::shared_ptr<Sensors::SensorPlane>& sen)
    {
        YAML::Node node;

        node["type"] = "sensor-plane";
        node["name"] = sen->parameters.name;
        node["save-to-file"] = sen->parameters.save_to_file;
        node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

        // sensor plane custom settings
        node["size"]["width"] = sen->settings.width;
        node["size"]["height"] = sen->settings.height;

        node["location"]["origin"] = sen->settings.origin;
        node["location"]["span-x"] = sen->settings.span_x;
        node["location"]["span-y"] = sen->settings.span_y;

        node["samples"]["x"] = sen->settings.number_of_samples_x;
        node["samples"]["y"] = sen->settings.number_of_samples_y;
        node["samples"]["sub-sample-grid-size"] = sen->settings.sub_sample_grid_size;

        node["image"]["min-value"] = sen->settings.min_image_value;
        node["image"]["max-value"] = sen->settings.max_image_value;


        switch (sen->settings.sensor_type)
        {
        case Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypeDensity:
            node["data"] = "density";
            break;
        case Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypePressure:
            node["data"] = "pressure";
            break;
        case Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity:
            node["data"] = "velocity";
            break;
        default:
            Log::warning("[Saving] Unknown sensor plane type!");
            break;
        }

        node["calculate-every-nth-step"] = sen->settings.calculate_plane_every_nth_step;

        return node;
    }
    std::shared_ptr<Sensors::SensorPlane> SimulationSerializer::load_sensor_plane(const YAML::Node& node)
    {
        auto res = std::make_shared<Sensors::SensorPlane>();
        res->parameters.name = node["name"].as<std::string>();
        res->parameters.save_to_file = node["save-to-file"].as<bool>();
        res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();

        // sensor plane custom settings
        res->settings.width = node["size"]["width"].as<float>();
        res->settings.height = node["size"]["height"].as<float>();

        res->settings.origin = node["location"]["origin"].as<glm::vec3>();
        res->settings.span_x = node["location"]["span-x"].as<glm::vec3>();
        res->settings.span_y = node["location"]["span-y"].as<glm::vec3>();

        res->settings.number_of_samples_x = node["samples"]["x"].as<size_t>();
        res->settings.number_of_samples_y = node["samples"]["y"].as<size_t>();
        res->settings.sub_sample_grid_size = node["samples"]["sub-sample-grid-size"].as<size_t>();

        res->settings.min_image_value = node["image"]["min-value"].as<float>();
        res->settings.max_image_value = node["image"]["max-value"].as<float>();

        auto kind = node["data"].as<std::string>();
        if (kind == "density")
        {
            res->settings.sensor_type = Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypeDensity;
        }
        else if (kind == "pressure")
        {
            res->settings.sensor_type = Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypePressure;
        }
        else if (kind == "velocity")
        {
            res->settings.sensor_type = Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity;
        }
        else
        {
            Log::error("[LOADING] Unknown sensor plane type '" + kind + "'!");
            error_count++;
        }

        res->settings.calculate_plane_every_nth_step = node["calculate-every-nth-step"].as<size_t>(1);

        return res;
    }


    YAML::Node SimulationSerializer::save_global_energy_sensor(const std::shared_ptr<Sensors::GlobalEnergySensor>& sen)
    {
        YAML::Node node;

        node["type"] = "global-energy-sensor";
        node["name"] = sen->parameters.name;
        node["save-to-file"] = sen->parameters.save_to_file;
        node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

        node["relative-zero-height"] = sen->settings.relative_zero_height;

        return node;
    }

    std::shared_ptr<Sensors::GlobalEnergySensor> SimulationSerializer::load_global_energy_sensor(const YAML::Node& node)
    {
        auto res = std::make_shared<Sensors::GlobalEnergySensor>();
        res->parameters.name = node["name"].as<std::string>();
        res->parameters.save_to_file = node["save-to-file"].as<bool>();
        res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();
        res->settings.relative_zero_height = node["relative-zero-height"].as<float>();
        return res;
    }

    YAML::Node SimulationSerializer::save_global_particle_count_sensor(
        const std::shared_ptr<Sensors::GlobalParticleCountSensor>& sen)
    {
        YAML::Node node;

        node["type"] = "global-particle-count-sensor";
        node["name"] = sen->parameters.name;
        node["save-to-file"] = sen->parameters.save_to_file;
        node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

        return node;
    }

    std::shared_ptr<Sensors::GlobalParticleCountSensor> SimulationSerializer::load_global_particle_count_sensor(
        const YAML::Node& node)
    {
        auto res = std::make_shared<Sensors::GlobalParticleCountSensor>();
        res->parameters.name = node["name"].as<std::string>();
        res->parameters.save_to_file = node["save-to-file"].as<bool>();
        res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();
        return res;
    }

    YAML::Node SimulationSerializer::save_compressed_neighborhood_storage_sensor(
        const std::shared_ptr<Sensors::CompressedNeighborStorageSensor>& sen)
    {
        YAML::Node node;

        node["type"] = "compressed-neighborhood-storage-sensor";
        node["name"] = sen->parameters.name;
        node["save-to-file"] = sen->parameters.save_to_file;
        node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

        return node;
    }
    std::shared_ptr<Sensors::CompressedNeighborStorageSensor> SimulationSerializer::
        load_compressed_neighborhood_storage_sensor(const YAML::Node& node)
    {
        auto res = std::make_shared<Sensors::CompressedNeighborStorageSensor>();
        res->parameters.name = node["name"].as<std::string>();
        res->parameters.save_to_file = node["save-to-file"].as<bool>();
        res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();
        return res;
    }


    std::string SimulationSerializer::get_full_particle_data_path()
    {
        auto p = std::filesystem::path(filepath);
        auto combined = p.parent_path() / std::filesystem::path(settings.particle_data_relative_filepath);
        return combined.string();
    }

    YAML::Node SimulationSerializer::save_scenario(const Simulation& simulation)
    {
        YAML::Node res;

        // save particle data
        res["particles"] = settings.particle_data_relative_filepath;
        if (settings.save_particle_data)
        {
            SimulationSerializer::save_particles(*simulation.parameters.collection, get_full_particle_data_path());
        }

        // save general parameters
        res["gravity"] = simulation.parameters.gravity;
        res["particle-size"] = simulation.parameters.particle_size;
        res["rest-density"] = simulation.parameters.rest_density;

        // save entities
        for (auto ent : simulation.parameters.entities)
        {
            auto spawner = std::dynamic_pointer_cast<ParticleSpawner>(ent);
            if (spawner)
            {
                res["entities"].push_back(save_particle_spawner(spawner));
            }

            auto rem = std::dynamic_pointer_cast<ParticleRemover>(ent);
            if (rem)
            {
                res["entities"].push_back(save_particle_remover(rem));
            }
        }

        // save sensors
        for (auto sen : simulation.parameters.sensors)
        {

            auto gd = std::dynamic_pointer_cast<Sensors::GlobalDensitySensor>(sen);
            if (gd)
            {
                res["sensors"].push_back(save_global_density_sensor(gd));
            }

            auto gp = std::dynamic_pointer_cast<Sensors::GlobalPressureSensor>(sen);
            if (gp)
            {
                res["sensors"].push_back(save_global_pressure_sensor(gp));
            }

            auto gv = std::dynamic_pointer_cast<Sensors::GlobalVelocitySensor>(sen);
            if (gv)
            {
                res["sensors"].push_back(save_global_velocity_sensor(gv));
            }

            auto ge = std::dynamic_pointer_cast<Sensors::GlobalEnergySensor>(sen);
            if (ge)
            {
                res["sensors"].push_back(save_global_energy_sensor(ge));
            }

            auto gc = std::dynamic_pointer_cast<Sensors::GlobalParticleCountSensor>(sen);
            if (gc)
            {
                res["sensors"].push_back(save_global_particle_count_sensor(gc));
            }

            auto sp = std::dynamic_pointer_cast<Sensors::SensorPlane>(sen);
            if (sp)
            {
                res["sensors"].push_back(save_sensor_plane(sp));
            }

            auto cns = std::dynamic_pointer_cast<Sensors::CompressedNeighborStorageSensor>(sen);
            if (cns)
            {
                res["sensors"].push_back(save_compressed_neighborhood_storage_sensor(cns));
            }
        }

        return res;
    }

    void SimulationSerializer::load_scenario(const YAML::Node& node, Simulation& simulation)
    {

        // loading particles from file
        auto particles = node["particles"].as<std::string>();
        auto particles_file = std::filesystem::path(filepath).parent_path() / std::filesystem::path(particles);
        if (!std::filesystem::exists(particles_file))
        {
            Log::error("[LOADING] Particles file was not found under '" + particles_file.string() + "'!");
            error_count++;
            return;
        }
        SimulationSerializer::load_particles(*simulation.parameters.collection, particles_file.string());

        // loading general parameters
        simulation.parameters.gravity = node["gravity"].as<float>();
        simulation.parameters.particle_size = node["particle-size"].as<float>();
        simulation.parameters.rest_density = node["rest-density"].as<float>();

        // loading entities
        if (node["entities"])
        {
            for (auto& ent_node : node["entities"])
            {
                if (ent_node["type"].as<std::string>() == "particle-spawner")
                {
                    simulation.parameters.entities.push_back(load_particle_spawner(ent_node));
                }
                else if (ent_node["type"].as<std::string>() == "particle-remover")
                {
                    simulation.parameters.entities.push_back(load_particle_remover(ent_node));
                }
                else
                {
                    warning_count++;
                    Log::warning("[LOADING] Unknown entity type: '" + ent_node["type"].as<std::string>() + "'!");
                }
            }
        }

        // loading sensors
        if (node["sensors"])
        {
            for (auto& sen_node : node["sensors"])
            {
                auto type_str = sen_node["type"].as<std::string>();
                if (type_str == "global-density-sensor")
                {
                    simulation.parameters.sensors.push_back(load_global_density_sensor(sen_node));
                }
                else if (type_str == "global-pressure-sensor")
                {
                    simulation.parameters.sensors.push_back(load_global_pressure_sensor(sen_node));
                }
                else if (type_str == "global-velocity-sensor")
                {
                    simulation.parameters.sensors.push_back(load_global_velocity_sensor(sen_node));
                }
                else if (type_str == "global-energy-sensor")
                {
                    simulation.parameters.sensors.push_back(load_global_energy_sensor(sen_node));
                }
                else if (type_str == "global-particle-count-sensor")
                {
                    simulation.parameters.sensors.push_back(load_global_particle_count_sensor(sen_node));
                }
                else if (type_str == "sensor-plane")
                {
                    simulation.parameters.sensors.push_back(load_sensor_plane(sen_node));
                }
                else if (type_str == "compressed-neighborhood-storage-sensor")
                {
                    simulation.parameters.sensors.push_back(load_compressed_neighborhood_storage_sensor(sen_node));
                }
                else
                {
                    warning_count++;
                    Log::warning("[LOADING] Unknown sensor type '" + type_str + "'!");
                }
            }
        }
    }

    YAML::Node SimulationSerializer::save_timestep(const std::shared_ptr<ITimestepGenerator>& timestep)
    {
        YAML::Node node;

        if (std::dynamic_pointer_cast<ConstantTimestepGenerator>(timestep))
        {
            auto t = std::dynamic_pointer_cast<ConstantTimestepGenerator>(timestep);

            node["type"] = "constant";
            node["timestep"] = t->settings.timestep;
        }
        else if (std::dynamic_pointer_cast<DynamicCflTimestepGenerator>(timestep))
        {
            auto t = std::dynamic_pointer_cast<DynamicCflTimestepGenerator>(timestep);

            node["type"] = "dynamic-cfl";
            node["max-timestep"] = t->settings.max_timestep;
            node["min-timestep"] = t->settings.min_timestep;
            node["cfl"] = t->settings.cfl_number;
        }
        else
        {
            error_count++;
            Log::error("[SAVING] Unsupported timestep type!");
        }


        return node;
    }

    std::shared_ptr<ITimestepGenerator> SimulationSerializer::load_timestep(const YAML::Node& node)
    {

        if (node["type"].as<std::string>() == "constant")
        {
            auto res = std::make_shared<ConstantTimestepGenerator>();
            res->settings.timestep = node["timestep"].as<float>();
            return res;
        }
        else if (node["type"].as<std::string>() == "dynamic-cfl")
        {
            auto res = std::make_shared<DynamicCflTimestepGenerator>();
            res->settings.max_timestep = node["max-timestep"].as<float>();
            res->settings.min_timestep = node["min-timestep"].as<float>();
            res->settings.cfl_number = node["cfl"].as<float>();
            return res;
        }
        else
        {
            error_count++;
            Log::error("[LOADING] Unknown timestep type '" + node["type"].as<std::string>() + "'!");
        }
        return nullptr;
    }

    YAML::Node SimulationSerializer::save_solver(const Simulation& simulation)
    {
        YAML::Node node;

        // solver, neighborhood search, kernel
        auto s = simulation.parameters.fluid_solver;

        if (std::dynamic_pointer_cast<SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(
                s))
        {
            auto f = std::dynamic_pointer_cast<
                SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(s);

            node["type"] = "sesph";
            node["neigborhood-search"]["type"] = "quadratic-dynamic-allocated";
            node["kernel"]["type"] = "cubic-spline-kernel";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;
        }
        else if (std::dynamic_pointer_cast<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(s))
        {
            auto f = std::dynamic_pointer_cast<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(s);
            node["type"] = "sesph";
            node["neigborhood-search"]["type"] = "hashed";
            node["kernel"]["type"] = "cubic-spline-kernel";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;
        }
        else if (std::dynamic_pointer_cast<
                     IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(s))
        {
            auto f = std::dynamic_pointer_cast<
                IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(s);

            node["type"] = "iisph";
            node["neigborhood-search"]["type"] = "quadratic-dynamic-allocated";
            node["kernel"]["type"] = "cubic-spline-kernel";

            node["gamma"] = f->settings.Gamma;
            node["omega"] = f->settings.Omega;
            node["max-density-error"] = f->settings.MaxDensityErrorAllowed;
            node["max-iterations"] = f->settings.MaxNumberOfIterations;
            node["min-iterations"] = f->settings.MinNumberOfIterations;
            node["viscosity"] = f->settings.Viscosity;
        }
        else if (std::dynamic_pointer_cast<IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(s))
        {
            auto f = std::dynamic_pointer_cast<IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(s);

            node["type"] = "iisph";
            node["neigborhood-search"]["type"] = "hashed";
            node["kernel"]["type"] = "cubic-spline-kernel";

            node["gamma"] = f->settings.Gamma;
            node["omega"] = f->settings.Omega;
            node["max-density-error"] = f->settings.MaxDensityErrorAllowed;
            node["max-iterations"] = f->settings.MaxNumberOfIterations;
            node["min-iterations"] = f->settings.MinNumberOfIterations;
            node["viscosity"] = f->settings.Viscosity;
        }
        else if (std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(s))
        {
            auto f =
                std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(s);

            node["type"] = "sesph-3d";
            node["neigborhood-search"]["type"] = "quadratic-dynamic-allocated-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;
        }
        else if (std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(s))
        {
            auto f = std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(s);

            node["type"] = "sesph-3d";
            node["neigborhood-search"]["type"] = "hashed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;
        }
        else if (std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(s))
        {
            auto f =
                std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(s);

            node["type"] = "sesph-3d";
            node["neigborhood-search"]["type"] = "compressed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;
        }
        else if (std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(s))
        {
            auto f =
                std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(s);

            node["type"] = "iisph-3d";
            node["neigborhood-search"]["type"] = "quadratic-dynamic-allocated-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            node["gamma"] = f->settings.gamma;
            node["omega"] = f->settings.omega;
            node["max-density-error"] = f->settings.max_density_error_allowed;
            node["max-iterations"] = f->settings.max_number_of_iterations;
            node["min-iterations"] = f->settings.min_number_of_iterations;
            node["viscosity"] = f->settings.viscosity;
        }
        else if (std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(s))
        {
            auto f = std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(s);

            node["type"] = "iisph-3d";
            node["neigborhood-search"]["type"] = "hashed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            node["gamma"] = f->settings.gamma;
            node["omega"] = f->settings.omega;
            node["max-density-error"] = f->settings.max_density_error_allowed;
            node["max-iterations"] = f->settings.max_number_of_iterations;
            node["min-iterations"] = f->settings.min_number_of_iterations;
            node["viscosity"] = f->settings.viscosity;
        }
        else if (std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(s))
        {
            auto f =
                std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(s);

            node["type"] = "iisph-3d";
            node["neigborhood-search"]["type"] = "compressed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            node["gamma"] = f->settings.gamma;
            node["omega"] = f->settings.omega;
            node["max-density-error"] = f->settings.max_density_error_allowed;
            node["max-iterations"] = f->settings.max_number_of_iterations;
            node["min-iterations"] = f->settings.min_number_of_iterations;
            node["viscosity"] = f->settings.viscosity;
        }
        else
        {
            error_count++;
            Log::error("[SAVING] Unsupported combination of fluid solver components!");
        }


        // timestep
        node["timestep"] = save_timestep(simulation.parameters.timestep);


        return node;
    }

    void SimulationSerializer::load_solver(Simulation& simulation, const YAML::Node& node)
    {

        simulation.parameters.fluid_solver = nullptr;

        if (node["type"].as<std::string>() == "sesph")
        {
            if (node["kernel"]["type"].as<std::string>() == "cubic-spline-kernel")
            {
                if (node["neigborhood-search"]["type"].as<std::string>() == "quadratic-dynamic-allocated")
                {
                    auto res = std::make_shared<
                        SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>();

                    res->settings.StiffnessK = node["stiffness"].as<float>();
                    res->settings.Viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else if (node["neigborhood-search"]["type"].as<std::string>() == "hashed")
                {
                    auto res = std::make_shared<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>();

                    res->settings.StiffnessK = node["stiffness"].as<float>();
                    res->settings.Viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else
                {
                    error_count++;
                    Log::error("[LOADING] Unknown neighborhood search type '" +
                               node["neigborhood-search"]["type"].as<std::string>() + "'!");
                }
            }
            else
            {
                error_count++;
                Log::error("[LOADING] Unknown kernel type '" + node["kernel"]["type"].as<std::string>() + "'!");
            }
        }
        else if (node["type"].as<std::string>() == "iisph")
        {
            if (node["kernel"]["type"].as<std::string>() == "cubic-spline-kernel")
            {
                if (node["neigborhood-search"]["type"].as<std::string>() == "quadratic-dynamic-allocated")
                {
                    auto res = std::make_shared<
                        IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>();

                    res->settings.Gamma = node["gamma"].as<float>();
                    res->settings.Omega = node["omega"].as<float>();
                    res->settings.MaxDensityErrorAllowed = node["max-density-error"].as<float>();
                    res->settings.MinNumberOfIterations = node["min-iterations"].as<size_t>();
                    res->settings.MaxNumberOfIterations = node["max-iterations"].as<size_t>();
                    res->settings.Viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else if (node["neigborhood-search"]["type"].as<std::string>() == "hashed")
                {
                    auto res = std::make_shared<IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>();

                    res->settings.Gamma = node["gamma"].as<float>();
                    res->settings.Omega = node["omega"].as<float>();
                    res->settings.MaxDensityErrorAllowed = node["max-density-error"].as<float>();
                    res->settings.MinNumberOfIterations = node["min-iterations"].as<size_t>();
                    res->settings.MaxNumberOfIterations = node["max-iterations"].as<size_t>();
                    res->settings.Viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else
                {
                    error_count++;
                    Log::error("[LOADING] Unknown neighborhood search type '" +
                               node["neigborhood-search"]["type"].as<std::string>() + "'!");
                }
            }
            else
            {
                error_count++;
                Log::error("[LOADING] Unknown kernel type '" + node["kernel"]["type"].as<std::string>() + "'!");
            }
        }
        else if (node["type"].as<std::string>() == "sesph-3d")
        {
            if (node["kernel"]["type"].as<std::string>() == "cubic-spline-kernel-3d")
            {
                if (node["neigborhood-search"]["type"].as<std::string>() == "quadratic-dynamic-allocated-3d")
                {
                    auto res =
                        std::make_shared<SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>();

                    res->settings.StiffnessK = node["stiffness"].as<float>();
                    res->settings.Viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else if (node["neigborhood-search"]["type"].as<std::string>() == "hashed-3d")
                {
                    auto res = std::make_shared<SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>();

                    res->settings.StiffnessK = node["stiffness"].as<float>();
                    res->settings.Viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else if (node["neigborhood-search"]["type"].as<std::string>() == "compressed-3d")
                {
                    auto res =
                        std::make_shared<SESPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>();

                    res->settings.StiffnessK = node["stiffness"].as<float>();
                    res->settings.Viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else
                {
                    error_count++;
                    Log::error("[LOADING] Unknown neighborhood search type '" +
                               node["neigborhood-search"]["type"].as<std::string>() + "'!");
                }
            }
            else
            {
                error_count++;
                Log::error("[LOADING] Unknown kernel type '" + node["kernel"]["type"].as<std::string>() + "'!");
            }
        }
        else if (node["type"].as<std::string>() == "iisph-3d")
        {
            if (node["kernel"]["type"].as<std::string>() == "cubic-spline-kernel-3d")
            {
                if (node["neigborhood-search"]["type"].as<std::string>() == "quadratic-dynamic-allocated-3d")
                {
                    auto res =
                        std::make_shared<IISPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>();

                    res->settings.gamma = node["gamma"].as<float>();
                    res->settings.omega = node["omega"].as<float>();
                    res->settings.max_density_error_allowed = node["max-density-error"].as<float>();
                    res->settings.min_number_of_iterations = node["min-iterations"].as<size_t>();
                    res->settings.max_number_of_iterations = node["max-iterations"].as<size_t>();
                    res->settings.viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else if (node["neigborhood-search"]["type"].as<std::string>() == "hashed-3d")
                {
                    auto res = std::make_shared<IISPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>();

                    res->settings.gamma = node["gamma"].as<float>();
                    res->settings.omega = node["omega"].as<float>();
                    res->settings.max_density_error_allowed = node["max-density-error"].as<float>();
                    res->settings.min_number_of_iterations = node["min-iterations"].as<size_t>();
                    res->settings.max_number_of_iterations = node["max-iterations"].as<size_t>();
                    res->settings.viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else if (node["neigborhood-search"]["type"].as<std::string>() == "compressed-3d")
                {
                    auto res =
                        std::make_shared<IISPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>();

                    res->settings.gamma = node["gamma"].as<float>();
                    res->settings.omega = node["omega"].as<float>();
                    res->settings.max_density_error_allowed = node["max-density-error"].as<float>();
                    res->settings.min_number_of_iterations = node["min-iterations"].as<size_t>();
                    res->settings.max_number_of_iterations = node["max-iterations"].as<size_t>();
                    res->settings.viscosity = node["viscosity"].as<float>();

                    simulation.parameters.fluid_solver = res;
                }
                else
                {
                    error_count++;
                    Log::error("[LOADING] Unknown neighborhood search type '" +
                               node["neigborhood-search"]["type"].as<std::string>() + "'!");
                }
            }
            else
            {
                error_count++;
                Log::error("[LOADING] Unknown kernel type '" + node["kernel"]["type"].as<std::string>() + "'!");
            }
        }
        else
        {
            error_count++;
            Log::error("[LOADING] Unknown solver type '" + node["type"].as<std::string>() + "'!");
        }

        // timestep
        simulation.parameters.timestep = load_timestep(node["timestep"]);
    }

    std::shared_ptr<ISimulationVisualizer> SimulationSerializer::load_visualizer(const YAML::Node& node)
    {

        if (node["type"].as<std::string>() == "no-visualizer")
        {
            return nullptr;
        }
        else if (node["type"].as<std::string>() == "continous")
        {
            auto r = std::make_shared<ContinousVisualizer>();

            // default parameters
            r->settings.viewport.left = node["viewport"]["left"].as<float>();
            r->settings.viewport.right = node["viewport"]["right"].as<float>();
            r->settings.viewport.top = node["viewport"]["top"].as<float>();
            r->settings.viewport.bottom = node["viewport"]["bottom"].as<float>();
            r->parameters.render_target.width = node["render-target"]["width"].as<size_t>();
            r->parameters.render_target.height = node["render-target"]["height"].as<size_t>();

            // custom parameters for the continous visualizer
            r->settings.clear_color = node["settings"]["background"].as<Image::Color>();
            r->settings.minimum_render_density = node["settings"]["minimum-density"].as<float>();

            return r;
        }
        else
        {
            auto res = deserialize_unknown_visualizer(node);
            if (res != nullptr)
            {
                return res;
            }
            else
            {
                warning_count++;
                Log::warning("[LOADING] Unknown visualizer type '" + node["type"].as<std::string>() + "'!");
            }
        }

        return nullptr;
    }


    YAML::Node SimulationSerializer::save_visualizer(const std::shared_ptr<ISimulationVisualizer>& visualizer)
    {
        YAML::Node node;

        if (visualizer == nullptr)
        {
            node["type"] = "no-visualizer";
        }
        else if (std::dynamic_pointer_cast<const ContinousVisualizer>(visualizer) != nullptr)
        {
            auto r = std::dynamic_pointer_cast<const ContinousVisualizer>(visualizer);
            node["type"] = "continous";

            // default parameters
            node["viewport"]["left"] = r->settings.viewport.left;
            node["viewport"]["right"] = r->settings.viewport.right;
            node["viewport"]["top"] = r->settings.viewport.top;
            node["viewport"]["bottom"] = r->settings.viewport.bottom;
            node["render-target"]["width"] = r->parameters.render_target.width;
            node["render-target"]["height"] = r->parameters.render_target.height;

            // custom parameters for the continous visualizer
            node["settings"]["background"] = r->settings.clear_color;
            node["settings"]["minimum-density"] = r->settings.minimum_render_density;
        }
        else
        {
            auto res = serialize_unknown_visualizer(visualizer);
            if (res.has_value())
            {
                node = std::move(res.value());
            }
            else
            {
                warning_count++;
                Log::warning("[SAVING] Unsupported visualizer type!");
            }
        }

        return node;
    }


    SimulationSerializer::SimulationSerializer(const std::string& filepath) : filepath(filepath)
    {
    }

    SimulationSerializer::SimulationSerializer(const std::string& filepath, const Settings& settings)
        : filepath(filepath), settings(settings)
    {
    }

    bool SimulationSerializer::has_errors() const
    {
        return error_count > 0;
    }

    bool SimulationSerializer::has_warnings() const
    {
        return warning_count > 0;
    }


    Simulation SimulationSerializer::load_from_file()
    {
        // reset error statistics
        error_count = 0;
        warning_count = 0;

        // setup basic parameters
        Simulation res;
        res.parameters.invalidate = true;

        res.parameters.collection = std::make_shared<ParticleCollection>();

        // check version
        YAML::Node config = YAML::LoadFile(filepath);
        if (!config["version"] || config["version"].as<int>() != 1)
        {
            Log::error("[LOADING] Invalid or missing version. Version has to be 1.");
            error_count++;
            return res;
        }


        // load the data
        try
        {
            load_scenario(config["scenario"], res);
            load_solver(res, config["solver"]);
            res.parameters.visualizer = load_visualizer(config["visualizer"]);
        }
        catch (const std::exception& e)
        {
            this->error_count++;
            Log::error("[LOADING] An exception occured during loading: " + std::string(e.what()));
            return res;
        }


        return res;
    }


    void SimulationSerializer::save_to_file(const Simulation& simulation)
    {
        // reset error statistics
        error_count = 0;
        warning_count = 0;

        YAML::Node config;

        // set version
        config["version"] = "1";


        // save values
        config["scenario"] = save_scenario(simulation);
        config["solver"] = save_solver(simulation);
        config["visualizer"] = save_visualizer(simulation.parameters.visualizer);

        // save data to file
        std::ofstream filestream(filepath);
        filestream << config;
    }

    void SimulationSerializer::load_particles(ParticleCollection& collection, const std::string& filepath)
    {
        ParticleSerializer stream(filepath);
        stream.deserialize(collection);
    }

    void SimulationSerializer::save_particles(ParticleCollection& collection, const std::string& filepath)
    {
        ParticleSerializer ser(filepath +".new");
        ser.serialize(collection);
    }


    std::shared_ptr<ISimulationVisualizer> SimulationSerializer::deserialize_unknown_visualizer(const YAML::Node& node)
    {
        return nullptr;
    }
    std::optional<YAML::Node> SimulationSerializer::serialize_unknown_visualizer(
        const std::shared_ptr<ISimulationVisualizer>& visualizer)
    {
        return {};
    }


} // namespace FluidSolver