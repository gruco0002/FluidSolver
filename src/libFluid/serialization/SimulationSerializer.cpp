#include "SimulationSerializer.hpp"

#include "Log.hpp"
#include "SimulatorVisualizerBundle.hpp"
#include "entities/BoundaryPreprocessor.hpp"
#include "entities/ParticleRemover.hpp"
#include "entities/ParticleRemover3D.hpp"
#include "entities/ParticleSpawner.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp"
#include "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp"
#include "fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch3D.hpp"
#include "fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearch3D.hpp"
#include "fluidSolver/solver/IISPHFluidSolver.hpp"
#include "fluidSolver/solver/IISPHFluidSolver3D.hpp"
#include "fluidSolver/solver/SESPHFluidSolver.hpp"
#include "fluidSolver/solver/SESPHFluidSolver3D.hpp"
#include "sensors/CompressedNeighborsStatistics.hpp"
#include "sensors/IisphSensor.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorPlane.hpp"
#include "serialization/ParticleSerializer.hpp"
#include "serialization/helpers/JsonHelpers.hpp"
#include "time/ConstantTimestepGenerator.hpp"
#include "time/DynamicCflTimestepGenerator.hpp"
#include "visualizer/ContinousVisualizer.hpp"


#include <filesystem>


namespace LibFluid {


    nlohmann::json SimulationSerializer::save_particle_spawner(const std::shared_ptr<ParticleSpawner>& spawner) {
        nlohmann::json res;
        res["type"] = "particle-spawner";

        res["position"] = spawner->parameters.position;
        res["direction"] = spawner->parameters.direction;
        res["width"] = spawner->parameters.width;
        res["initial-velocity"] = spawner->parameters.initial_velocity;
        res["rest-density"] = spawner->parameters.rest_density;
        res["mass"] = spawner->parameters.mass;

        return res;
    }

    std::shared_ptr<ParticleSpawner> SimulationSerializer::load_particle_spawner(const nlohmann::json& node) {
        auto spawner = std::make_shared<ParticleSpawner>();
        spawner->parameters.position = node["position"].get<glm::vec2>();
        spawner->parameters.direction = node["direction"].get<glm::vec2>();
        spawner->parameters.width = node["width"].get<float>();
        spawner->parameters.initial_velocity = node["initial-velocity"].get<float>();
        spawner->parameters.rest_density = node["rest-density"].get<float>();
        spawner->parameters.mass = node["mass"].get<float>();
        return spawner;
    }

    nlohmann::json SimulationSerializer::save_particle_remover(const std::shared_ptr<ParticleRemover>& remover) {
        nlohmann::json res;
        res["type"] = "particle-remover";

        res["area"]["left"] = remover->parameters.area.left;
        res["area"]["right"] = remover->parameters.area.right;
        res["area"]["top"] = remover->parameters.area.top;
        res["area"]["bottom"] = remover->parameters.area.bottom;
        res["remove-if-outside"] = remover->parameters.remove_if_outside;

        return res;
    }


    std::shared_ptr<ParticleRemover> SimulationSerializer::load_particle_remover(const nlohmann::json& node) {
        auto remover = std::make_shared<ParticleRemover>();

        remover->parameters.area.left = node["area"]["left"].get<float>();
        remover->parameters.area.right = node["area"]["right"].get<float>();
        remover->parameters.area.top = node["area"]["top"].get<float>();
        remover->parameters.area.bottom = node["area"]["bottom"].get<float>();
        remover->parameters.remove_if_outside = node["remove-if-outside"].get<bool>();

        return remover;
    }

    nlohmann::json SimulationSerializer::save_global_density_sensor(
            const std::shared_ptr<Sensors::GlobalDensitySensor>& sen) {
        nlohmann::json node;

        node["type"] = "global-density-sensor";
        save_basic_sensor_data(node, sen);

        return node;
    }

    std::shared_ptr<Sensors::GlobalDensitySensor> SimulationSerializer::load_global_density_sensor(
            const nlohmann::json& node) {
        auto res = std::make_shared<Sensors::GlobalDensitySensor>();
        load_basic_sensor_data(node, res);

        return res;
    }

    nlohmann::json SimulationSerializer::save_global_pressure_sensor(
            const std::shared_ptr<Sensors::GlobalPressureSensor>& sen) {
        nlohmann::json node;

        node["type"] = "global-pressure-sensor";
        save_basic_sensor_data(node, sen);

        return node;
    }

    std::shared_ptr<Sensors::GlobalPressureSensor> SimulationSerializer::load_global_pressure_sensor(
            const nlohmann::json& node) {
        auto res = std::make_shared<Sensors::GlobalPressureSensor>();
        load_basic_sensor_data(node, res);

        return res;
    }

    nlohmann::json SimulationSerializer::save_global_velocity_sensor(
            const std::shared_ptr<Sensors::GlobalVelocitySensor>& sen) {
        nlohmann::json node;

        node["type"] = "global-velocity-sensor";
        save_basic_sensor_data(node, sen);

        return node;
    }

    std::shared_ptr<Sensors::GlobalVelocitySensor> SimulationSerializer::load_global_velocity_sensor(
            const nlohmann::json& node) {
        auto res = std::make_shared<Sensors::GlobalVelocitySensor>();
        load_basic_sensor_data(node, res);

        return res;
    }

    nlohmann::json SimulationSerializer::save_sensor_plane(const std::shared_ptr<Sensors::SensorPlane>& sen) {
        nlohmann::json node;

        node["type"] = "sensor-plane";
        save_basic_sensor_data(node, sen);

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


        switch (sen->settings.sensor_type) {
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
    std::shared_ptr<Sensors::SensorPlane> SimulationSerializer::load_sensor_plane(const nlohmann::json& node) {
        auto res = std::make_shared<Sensors::SensorPlane>();
        load_basic_sensor_data(node, res);

        // sensor plane custom settings
        res->settings.width = node["size"]["width"].get<float>();
        res->settings.height = node["size"]["height"].get<float>();

        res->settings.origin = node["location"]["origin"].get<glm::vec3>();
        res->settings.span_x = node["location"]["span-x"].get<glm::vec3>();
        res->settings.span_y = node["location"]["span-y"].get<glm::vec3>();

        res->settings.number_of_samples_x = node["samples"]["x"].get<size_t>();
        res->settings.number_of_samples_y = node["samples"]["y"].get<size_t>();
        res->settings.sub_sample_grid_size = node["samples"]["sub-sample-grid-size"].get<size_t>();

        res->settings.min_image_value = node["image"]["min-value"].get<float>();
        res->settings.max_image_value = node["image"]["max-value"].get<float>();

        auto kind = node["data"].get<std::string>();
        if (kind == "density") {
            res->settings.sensor_type = Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypeDensity;
        } else if (kind == "pressure") {
            res->settings.sensor_type = Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypePressure;
        } else if (kind == "velocity") {
            res->settings.sensor_type = Sensors::SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity;
        } else {
            Log::error("[LOADING] Unknown sensor plane type '" + kind + "'!");
            error_count++;
        }

        if (node.contains("calculate-every-nth-step")) {
            res->settings.calculate_plane_every_nth_step = node["calculate-every-nth-step"].get<size_t>();
        } else {
            res->settings.calculate_plane_every_nth_step = 1;
        }


        return res;
    }


    nlohmann::json SimulationSerializer::save_global_energy_sensor(const std::shared_ptr<Sensors::GlobalEnergySensor>& sen) {
        nlohmann::json node;

        node["type"] = "global-energy-sensor";
        save_basic_sensor_data(node, sen);

        node["relative-zero-height"] = sen->settings.relative_zero_height;

        return node;
    }

    std::shared_ptr<Sensors::GlobalEnergySensor> SimulationSerializer::load_global_energy_sensor(const nlohmann::json& node) {
        auto res = std::make_shared<Sensors::GlobalEnergySensor>();
        load_basic_sensor_data(node, res);
        res->settings.relative_zero_height = node["relative-zero-height"].get<float>();
        return res;
    }

    nlohmann::json SimulationSerializer::save_global_particle_count_sensor(
            const std::shared_ptr<Sensors::GlobalParticleCountSensor>& sen) {
        nlohmann::json node;

        node["type"] = "global-particle-count-sensor";
        save_basic_sensor_data(node, sen);

        return node;
    }

    std::shared_ptr<Sensors::GlobalParticleCountSensor> SimulationSerializer::load_global_particle_count_sensor(
            const nlohmann::json& node) {
        auto res = std::make_shared<Sensors::GlobalParticleCountSensor>();
        load_basic_sensor_data(node, res);
        return res;
    }

    nlohmann::json SimulationSerializer::save_compressed_neighborhood_storage_sensor(
            const std::shared_ptr<Sensors::CompressedNeighborStorageSensor>& sen) {
        nlohmann::json node;

        node["type"] = "compressed-neighborhood-storage-sensor";
        save_basic_sensor_data(node, sen);

        return node;
    }
    std::shared_ptr<Sensors::CompressedNeighborStorageSensor> SimulationSerializer::
            load_compressed_neighborhood_storage_sensor(const nlohmann::json& node) {
        auto res = std::make_shared<Sensors::CompressedNeighborStorageSensor>();
        load_basic_sensor_data(node, res);
        return res;
    }

    nlohmann::json SimulationSerializer::save_iisph_sensor(const std::shared_ptr<Sensors::IISPHSensor>& sen) {
        nlohmann::json node;

        node["type"] = "iisph-sensor";
        save_basic_sensor_data(node, sen);

        return node;
    }
    std::shared_ptr<Sensors::IISPHSensor> SimulationSerializer::load_iisph_sensor(const nlohmann::json& node) {
        auto res = std::make_shared<Sensors::IISPHSensor>();
        load_basic_sensor_data(node, res);
        return res;
    }


    std::string SimulationSerializer::get_full_particle_data_path() {
        auto p = std::filesystem::path(filepath);
        auto combined = p.parent_path() / std::filesystem::path(settings.particle_data_relative_filepath);
        return combined.string();
    }

    nlohmann::json SimulationSerializer::save_scenario(const Simulator& simulation) {
        nlohmann::json res;

        // save particle data
        res["particles"] = settings.particle_data_relative_filepath;
        if (settings.save_particle_data) {
            SimulationSerializer::save_particles(*simulation.data.collection, get_full_particle_data_path());
        }

        // save general parameters
        res["gravity"] = simulation.parameters.gravity;
        res["particle-size"] = simulation.parameters.particle_size;
        res["rest-density"] = simulation.parameters.rest_density;

        // save entities
        for (auto ent : simulation.data.entities) {
            auto spawner = std::dynamic_pointer_cast<ParticleSpawner>(ent);
            if (spawner) {
                res["entities"].push_back(save_particle_spawner(spawner));
            }

            auto rem = std::dynamic_pointer_cast<ParticleRemover>(ent);
            if (rem) {
                res["entities"].push_back(save_particle_remover(rem));
            }

            auto rem3d = std::dynamic_pointer_cast<ParticleRemover3D>(ent);
            if (rem3d) {
                res["entities"].push_back(save_particle_remover_3d(rem3d));
            }

            auto prep = std::dynamic_pointer_cast<BoundaryPreprocessor>(ent);
            if (prep) {
                res["entities"].push_back(save_boundary_preprocessor(prep));
            }
        }

        // save sensors
        for (auto sen : simulation.data.sensors) {
            auto gd = std::dynamic_pointer_cast<Sensors::GlobalDensitySensor>(sen);
            if (gd) {
                res["sensors"].push_back(save_global_density_sensor(gd));
            }

            auto gp = std::dynamic_pointer_cast<Sensors::GlobalPressureSensor>(sen);
            if (gp) {
                res["sensors"].push_back(save_global_pressure_sensor(gp));
            }

            auto gv = std::dynamic_pointer_cast<Sensors::GlobalVelocitySensor>(sen);
            if (gv) {
                res["sensors"].push_back(save_global_velocity_sensor(gv));
            }

            auto ge = std::dynamic_pointer_cast<Sensors::GlobalEnergySensor>(sen);
            if (ge) {
                res["sensors"].push_back(save_global_energy_sensor(ge));
            }

            auto gc = std::dynamic_pointer_cast<Sensors::GlobalParticleCountSensor>(sen);
            if (gc) {
                res["sensors"].push_back(save_global_particle_count_sensor(gc));
            }

            auto sp = std::dynamic_pointer_cast<Sensors::SensorPlane>(sen);
            if (sp) {
                res["sensors"].push_back(save_sensor_plane(sp));
            }

            auto cns = std::dynamic_pointer_cast<Sensors::CompressedNeighborStorageSensor>(sen);
            if (cns) {
                res["sensors"].push_back(save_compressed_neighborhood_storage_sensor(cns));
            }

            auto iisph = std::dynamic_pointer_cast<Sensors::IISPHSensor>(sen);
            if (iisph) {
                res["sensors"].push_back(save_iisph_sensor(iisph));
            }
        }

        return res;
    }

    void SimulationSerializer::load_scenario(const nlohmann::json& node, Simulator& simulation) {
        // loading particles from file
        auto particles = node["particles"].get<std::string>();
        auto particles_file = std::filesystem::path(filepath).parent_path() / std::filesystem::path(particles);
        if (!std::filesystem::exists(particles_file)) {
            Log::error("[LOADING] Particles file was not found under '" + particles_file.string() + "'!");
            error_count++;
            return;
        }
        SimulationSerializer::load_particles(*simulation.data.collection, particles_file.string());

        // loading general parameters
        simulation.parameters.gravity = node["gravity"].get<float>();
        simulation.parameters.particle_size = node["particle-size"].get<float>();
        simulation.parameters.rest_density = node["rest-density"].get<float>();

        // loading entities
        if (node.contains("entities")) {
            for (auto& ent_node : node["entities"]) {
                auto node_type = ent_node["type"].get<std::string>();

                if (node_type == "particle-spawner") {
                    simulation.data.entities.push_back(load_particle_spawner(ent_node));
                } else if (node_type == "particle-remover") {
                    simulation.data.entities.push_back(load_particle_remover(ent_node));
                } else if (node_type == "particle-remover-3d") {
                    simulation.data.entities.push_back(load_particle_remover_3d(ent_node));
                } else if (node_type == "boundary-preprocessor") {
                    simulation.data.entities.push_back(load_boundary_preprocessor(ent_node));
                } else {
                    warning_count++;
                    Log::warning("[LOADING] Unknown entity type: '" + ent_node["type"].get<std::string>() + "'!");
                }
            }
        }

        // loading sensors
        if (node.contains("sensors")) {
            for (auto& sen_node : node["sensors"]) {
                auto type_str = sen_node["type"].get<std::string>();
                if (type_str == "global-density-sensor") {
                    simulation.data.sensors.push_back(load_global_density_sensor(sen_node));
                } else if (type_str == "global-pressure-sensor") {
                    simulation.data.sensors.push_back(load_global_pressure_sensor(sen_node));
                } else if (type_str == "global-velocity-sensor") {
                    simulation.data.sensors.push_back(load_global_velocity_sensor(sen_node));
                } else if (type_str == "global-energy-sensor") {
                    simulation.data.sensors.push_back(load_global_energy_sensor(sen_node));
                } else if (type_str == "global-particle-count-sensor") {
                    simulation.data.sensors.push_back(load_global_particle_count_sensor(sen_node));
                } else if (type_str == "sensor-plane") {
                    simulation.data.sensors.push_back(load_sensor_plane(sen_node));
                } else if (type_str == "compressed-neighborhood-storage-sensor") {
                    simulation.data.sensors.push_back(load_compressed_neighborhood_storage_sensor(sen_node));
                } else if (type_str == "iisph-sensor") {
                    simulation.data.sensors.push_back(load_iisph_sensor(sen_node));
                } else {
                    warning_count++;
                    Log::warning("[LOADING] Unknown sensor type '" + type_str + "'!");
                }
            }
        }
    }

    nlohmann::json SimulationSerializer::save_timestep(const std::shared_ptr<TimestepGenerator>& timestep) {
        nlohmann::json node;

        if (std::dynamic_pointer_cast<ConstantTimestepGenerator>(timestep)) {
            auto t = std::dynamic_pointer_cast<ConstantTimestepGenerator>(timestep);

            node["type"] = "constant";
            node["timestep"] = t->settings.timestep;
        } else if (std::dynamic_pointer_cast<DynamicCflTimestepGenerator>(timestep)) {
            auto t = std::dynamic_pointer_cast<DynamicCflTimestepGenerator>(timestep);

            node["type"] = "dynamic-cfl";
            node["max-timestep"] = t->settings.max_timestep;
            node["min-timestep"] = t->settings.min_timestep;
            node["cfl"] = t->settings.cfl_number;
        } else {
            error_count++;
            Log::error("[SAVING] Unsupported timestep type!");
        }


        return node;
    }

    std::shared_ptr<TimestepGenerator> SimulationSerializer::load_timestep(const nlohmann::json& node) {
        if (node["type"].get<std::string>() == "constant") {
            auto res = std::make_shared<ConstantTimestepGenerator>();
            res->settings.timestep = node["timestep"].get<float>();
            return res;
        } else if (node["type"].get<std::string>() == "dynamic-cfl") {
            auto res = std::make_shared<DynamicCflTimestepGenerator>();
            res->settings.max_timestep = node["max-timestep"].get<float>();
            res->settings.min_timestep = node["min-timestep"].get<float>();
            res->settings.cfl_number = node["cfl"].get<float>();
            return res;
        } else {
            error_count++;
            Log::error("[LOADING] Unknown timestep type '" + node["type"].get<std::string>() + "'!");
        }
        return nullptr;
    }

    nlohmann::json SimulationSerializer::save_solver(const Simulator& simulation) {
        nlohmann::json node;

        // solver, neighborhood search, kernel
        auto s = simulation.data.fluid_solver;

        if (std::dynamic_pointer_cast<SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(
                    s)) {
            auto f = std::dynamic_pointer_cast<
                    SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(s);

            node["type"] = "sesph";
            node["neigborhood-search"]["type"] = "quadratic-dynamic-allocated";
            node["kernel"]["type"] = "cubic-spline-kernel";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;
        } else if (std::dynamic_pointer_cast<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(s)) {
            auto f = std::dynamic_pointer_cast<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(s);
            node["type"] = "sesph";
            node["neigborhood-search"]["type"] = "hashed";
            node["kernel"]["type"] = "cubic-spline-kernel";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;
        } else if (std::dynamic_pointer_cast<
                           IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(s)) {
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
        } else if (std::dynamic_pointer_cast<IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>(s)) {
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
        } else if (std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(s)) {
            auto f =
                    std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(s);

            node["type"] = "sesph-3d";
            node["neigborhood-search"]["type"] = "quadratic-dynamic-allocated-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;

            node["single-layer-boundary-enabled"] = f->settings.single_layer_boundary;
            if (f->settings.single_layer_boundary) {
                node["single-layer-settings"]["gamma-1"] = f->settings.single_layer_boundary_gamma_1;
                node["single-layer-settings"]["gamma-2"] = f->settings.single_layer_boundary_gamma_2;
            }
        } else if (std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(s)) {
            auto f = std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(s);

            node["type"] = "sesph-3d";
            node["neigborhood-search"]["type"] = "hashed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;

            node["single-layer-boundary-enabled"] = f->settings.single_layer_boundary;
            if (f->settings.single_layer_boundary) {
                node["single-layer-settings"]["gamma-1"] = f->settings.single_layer_boundary_gamma_1;
                node["single-layer-settings"]["gamma-2"] = f->settings.single_layer_boundary_gamma_2;
            }
        } else if (std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(s)) {
            auto f =
                    std::dynamic_pointer_cast<SESPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(s);

            node["type"] = "sesph-3d";
            node["neigborhood-search"]["type"] = "compressed-3d";
            node["kernel"]["type"] = "cubic-spline-kernel-3d";

            node["stiffness"] = f->settings.StiffnessK;
            node["viscosity"] = f->settings.Viscosity;

            node["single-layer-boundary-enabled"] = f->settings.single_layer_boundary;
            if (f->settings.single_layer_boundary) {
                node["single-layer-settings"]["gamma-1"] = f->settings.single_layer_boundary_gamma_1;
                node["single-layer-settings"]["gamma-2"] = f->settings.single_layer_boundary_gamma_2;
            }
        } else if (std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>(s)) {
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

            node["single-layer-boundary-enabled"] = f->settings.single_layer_boundary;
            if (f->settings.single_layer_boundary) {
                node["single-layer-settings"]["gamma-1"] = f->settings.single_layer_boundary_gamma_1;
                // TODO: eventually gamma should be in here
            }
        } else if (std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>(s)) {
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

            node["single-layer-boundary-enabled"] = f->settings.single_layer_boundary;
            if (f->settings.single_layer_boundary) {
                node["single-layer-settings"]["gamma-1"] = f->settings.single_layer_boundary_gamma_1;
                // TODO: eventually gamma should be in here
            }
        } else if (std::dynamic_pointer_cast<IISPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>(s)) {
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

            node["single-layer-boundary-enabled"] = f->settings.single_layer_boundary;
            if (f->settings.single_layer_boundary) {
                node["single-layer-settings"]["gamma-1"] = f->settings.single_layer_boundary_gamma_1;
                // TODO: eventually gamma should be in here
            }
        } else {
            error_count++;
            Log::error("[SAVING] Unsupported combination of fluid solver components!");
        }


        // timestep
        node["timestep"] = save_timestep(simulation.data.timestep_generator);


        return node;
    }

    void SimulationSerializer::load_solver(Simulator& simulation, const nlohmann::json& node) {
        simulation.data.fluid_solver = nullptr;

        if (node["type"].get<std::string>() == "sesph") {
            if (node["kernel"]["type"].get<std::string>() == "cubic-spline-kernel") {
                if (node["neigborhood-search"]["type"].get<std::string>() == "quadratic-dynamic-allocated") {
                    auto res = std::make_shared<
                            SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>();

                    res->settings.StiffnessK = node["stiffness"].get<float>();
                    res->settings.Viscosity = node["viscosity"].get<float>();

                    simulation.data.fluid_solver = res;
                } else if (node["neigborhood-search"]["type"].get<std::string>() == "hashed") {
                    auto res = std::make_shared<SESPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>();

                    res->settings.StiffnessK = node["stiffness"].get<float>();
                    res->settings.Viscosity = node["viscosity"].get<float>();

                    simulation.data.fluid_solver = res;
                } else {
                    error_count++;
                    Log::error("[LOADING] Unknown neighborhood search type '" +
                            node["neigborhood-search"]["type"].get<std::string>() + "'!");
                }
            } else {
                error_count++;
                Log::error("[LOADING] Unknown kernel type '" + node["kernel"]["type"].get<std::string>() + "'!");
            }
        } else if (node["type"].get<std::string>() == "iisph") {
            if (node["kernel"]["type"].get<std::string>() == "cubic-spline-kernel") {
                if (node["neigborhood-search"]["type"].get<std::string>() == "quadratic-dynamic-allocated") {
                    auto res = std::make_shared<
                            IISPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>();

                    res->settings.Gamma = node["gamma"].get<float>();
                    res->settings.Omega = node["omega"].get<float>();
                    res->settings.MaxDensityErrorAllowed = node["max-density-error"].get<float>();
                    res->settings.MinNumberOfIterations = node["min-iterations"].get<size_t>();
                    res->settings.MaxNumberOfIterations = node["max-iterations"].get<size_t>();
                    res->settings.Viscosity = node["viscosity"].get<float>();

                    simulation.data.fluid_solver = res;
                } else if (node["neigborhood-search"]["type"].get<std::string>() == "hashed") {
                    auto res = std::make_shared<IISPHFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch>>();

                    res->settings.Gamma = node["gamma"].get<float>();
                    res->settings.Omega = node["omega"].get<float>();
                    res->settings.MaxDensityErrorAllowed = node["max-density-error"].get<float>();
                    res->settings.MinNumberOfIterations = node["min-iterations"].get<size_t>();
                    res->settings.MaxNumberOfIterations = node["max-iterations"].get<size_t>();
                    res->settings.Viscosity = node["viscosity"].get<float>();

                    simulation.data.fluid_solver = res;
                } else {
                    error_count++;
                    Log::error("[LOADING] Unknown neighborhood search type '" +
                            node["neigborhood-search"]["type"].get<std::string>() + "'!");
                }
            } else {
                error_count++;
                Log::error("[LOADING] Unknown kernel type '" + node["kernel"]["type"].get<std::string>() + "'!");
            }
        } else if (node["type"].get<std::string>() == "sesph-3d") {
            if (node["kernel"]["type"].get<std::string>() == "cubic-spline-kernel-3d") {
                if (node["neigborhood-search"]["type"].get<std::string>() == "quadratic-dynamic-allocated-3d") {
                    auto res =
                            std::make_shared<SESPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>();

                    res->settings.StiffnessK = node["stiffness"].get<float>();
                    res->settings.Viscosity = node["viscosity"].get<float>();

                    if (node.contains("single-layer-boundary-enabled")) {
                        res->settings.single_layer_boundary = node["single-layer-boundary-enabled"].get<bool>();
                        if (res->settings.single_layer_boundary) {
                            res->settings.single_layer_boundary_gamma_1 = node["single-layer-settings"]["gamma-1"].get<float>();
                            res->settings.single_layer_boundary_gamma_2 = node["single-layer-settings"]["gamma-2"].get<float>();
                        }
                    }

                    simulation.data.fluid_solver = res;
                } else if (node["neigborhood-search"]["type"].get<std::string>() == "hashed-3d") {
                    auto res = std::make_shared<SESPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>();

                    res->settings.StiffnessK = node["stiffness"].get<float>();
                    res->settings.Viscosity = node["viscosity"].get<float>();

                    if (node.contains("single-layer-boundary-enabled")) {
                        res->settings.single_layer_boundary = node["single-layer-boundary-enabled"].get<bool>();
                        if (res->settings.single_layer_boundary) {
                            res->settings.single_layer_boundary_gamma_1 = node["single-layer-settings"]["gamma-1"].get<float>();
                            res->settings.single_layer_boundary_gamma_2 = node["single-layer-settings"]["gamma-2"].get<float>();
                        }
                    }

                    simulation.data.fluid_solver = res;
                } else if (node["neigborhood-search"]["type"].get<std::string>() == "compressed-3d") {
                    auto res =
                            std::make_shared<SESPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>();

                    res->settings.StiffnessK = node["stiffness"].get<float>();
                    res->settings.Viscosity = node["viscosity"].get<float>();

                    if (node.contains("single-layer-boundary-enabled")) {
                        res->settings.single_layer_boundary = node["single-layer-boundary-enabled"].get<bool>();
                        if (res->settings.single_layer_boundary) {
                            res->settings.single_layer_boundary_gamma_1 = node["single-layer-settings"]["gamma-1"].get<float>();
                            res->settings.single_layer_boundary_gamma_2 = node["single-layer-settings"]["gamma-2"].get<float>();
                        }
                    }

                    simulation.data.fluid_solver = res;
                } else {
                    error_count++;
                    Log::error("[LOADING] Unknown neighborhood search type '" +
                            node["neigborhood-search"]["type"].get<std::string>() + "'!");
                }
            } else {
                error_count++;
                Log::error("[LOADING] Unknown kernel type '" + node["kernel"]["type"].get<std::string>() + "'!");
            }
        } else if (node["type"].get<std::string>() == "iisph-3d") {
            if (node["kernel"]["type"].get<std::string>() == "cubic-spline-kernel-3d") {
                if (node["neigborhood-search"]["type"].get<std::string>() == "quadratic-dynamic-allocated-3d") {
                    auto res =
                            std::make_shared<IISPHFluidSolver3D<CubicSplineKernel3D, QuadraticNeighborhoodSearch3D>>();

                    res->settings.gamma = node["gamma"].get<float>();
                    res->settings.omega = node["omega"].get<float>();
                    res->settings.max_density_error_allowed = node["max-density-error"].get<float>();
                    res->settings.min_number_of_iterations = node["min-iterations"].get<size_t>();
                    res->settings.max_number_of_iterations = node["max-iterations"].get<size_t>();
                    res->settings.viscosity = node["viscosity"].get<float>();

                    if (node.contains("single-layer-boundary-enabled")) {
                        res->settings.single_layer_boundary = node["single-layer-boundary-enabled"].get<bool>();
                        if (res->settings.single_layer_boundary) {
                            res->settings.single_layer_boundary_gamma_1 = node["single-layer-settings"]["gamma-1"].get<float>();
                            // TODO: move gamme here as gamma-2
                        }
                    }

                    simulation.data.fluid_solver = res;
                } else if (node["neigborhood-search"]["type"].get<std::string>() == "hashed-3d") {
                    auto res = std::make_shared<IISPHFluidSolver3D<CubicSplineKernel3D, HashedNeighborhoodSearch3D>>();

                    res->settings.gamma = node["gamma"].get<float>();
                    res->settings.omega = node["omega"].get<float>();
                    res->settings.max_density_error_allowed = node["max-density-error"].get<float>();
                    res->settings.min_number_of_iterations = node["min-iterations"].get<size_t>();
                    res->settings.max_number_of_iterations = node["max-iterations"].get<size_t>();
                    res->settings.viscosity = node["viscosity"].get<float>();

                    if (node.contains("single-layer-boundary-enabled")) {
                        res->settings.single_layer_boundary = node["single-layer-boundary-enabled"].get<bool>();
                        if (res->settings.single_layer_boundary) {
                            res->settings.single_layer_boundary_gamma_1 = node["single-layer-settings"]["gamma-1"].get<float>();
                            // TODO: move gamme here as gamma-2
                        }
                    }

                    simulation.data.fluid_solver = res;
                } else if (node["neigborhood-search"]["type"].get<std::string>() == "compressed-3d") {
                    auto res =
                            std::make_shared<IISPHFluidSolver3D<CubicSplineKernel3D, CompressedNeighborhoodSearch>>();

                    res->settings.gamma = node["gamma"].get<float>();
                    res->settings.omega = node["omega"].get<float>();
                    res->settings.max_density_error_allowed = node["max-density-error"].get<float>();
                    res->settings.min_number_of_iterations = node["min-iterations"].get<size_t>();
                    res->settings.max_number_of_iterations = node["max-iterations"].get<size_t>();
                    res->settings.viscosity = node["viscosity"].get<float>();

                    if (node.contains("single-layer-boundary-enabled")) {
                        res->settings.single_layer_boundary = node["single-layer-boundary-enabled"].get<bool>();
                        if (res->settings.single_layer_boundary) {
                            res->settings.single_layer_boundary_gamma_1 = node["single-layer-settings"]["gamma-1"].get<float>();
                            // TODO: move gamme here as gamma-2
                        }
                    }

                    simulation.data.fluid_solver = res;
                } else {
                    error_count++;
                    Log::error("[LOADING] Unknown neighborhood search type '" +
                            node["neigborhood-search"]["type"].get<std::string>() + "'!");
                }
            } else {
                error_count++;
                Log::error("[LOADING] Unknown kernel type '" + node["kernel"]["type"].get<std::string>() + "'!");
            }
        } else {
            error_count++;
            Log::error("[LOADING] Unknown solver type '" + node["type"].get<std::string>() + "'!");
        }

        // timestep
        simulation.data.timestep_generator = load_timestep(node["timestep"]);
    }

    std::shared_ptr<ISimulationVisualizer> SimulationSerializer::load_visualizer(const nlohmann::json& node) {
        if (node["type"].get<std::string>() == "no-visualizer") {
            return nullptr;
        } else if (node["type"].get<std::string>() == "continous") {
            auto r = std::make_shared<ContinousVisualizer>();

            // default parameters
            r->settings.viewport.left = node["viewport"]["left"].get<float>();
            r->settings.viewport.right = node["viewport"]["right"].get<float>();
            r->settings.viewport.top = node["viewport"]["top"].get<float>();
            r->settings.viewport.bottom = node["viewport"]["bottom"].get<float>();
            r->parameters.render_target.width = node["render-target"]["width"].get<size_t>();
            r->parameters.render_target.height = node["render-target"]["height"].get<size_t>();

            // custom parameters for the continous visualizer
            r->settings.clear_color = node["settings"]["background"].get<Image::Color>();
            r->settings.minimum_render_density = node["settings"]["minimum-density"].get<float>();

            return r;
        } else {
            auto res = deserialize_unknown_visualizer(node);
            if (res != nullptr) {
                return res;
            } else {
                warning_count++;
                Log::warning("[LOADING] Unknown visualizer type '" + node["type"].get<std::string>() + "'!");
            }
        }

        return nullptr;
    }


    nlohmann::json SimulationSerializer::save_visualizer(const std::shared_ptr<ISimulationVisualizer>& visualizer) {
        nlohmann::json node;

        if (visualizer == nullptr) {
            node["type"] = "no-visualizer";
        } else if (std::dynamic_pointer_cast<const ContinousVisualizer>(visualizer) != nullptr) {
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
        } else {
            auto res = serialize_unknown_visualizer(visualizer);
            if (res.has_value()) {
                node = std::move(res.value());
            } else {
                warning_count++;
                Log::warning("[SAVING] Unsupported visualizer type!");
            }
        }

        return node;
    }


    SimulationSerializer::SimulationSerializer(const std::string& filepath)
        : filepath(filepath) {
    }

    SimulationSerializer::SimulationSerializer(const std::string& filepath, const Settings& settings)
        : filepath(filepath), settings(settings) {
    }

    bool SimulationSerializer::has_errors() const {
        return error_count > 0;
    }

    bool SimulationSerializer::has_warnings() const {
        return warning_count > 0;
    }


    SimulatorVisualizerBundle SimulationSerializer::load_from_file() {
        // reset error statistics
        error_count = 0;
        warning_count = 0;

        // setup basic parameters
        SimulatorVisualizerBundle res;
        res.simulator = std::make_shared<Simulator>();
        res.simulator->parameters.notify_that_data_changed();

        res.simulator->data.collection = std::make_shared<ParticleCollection>();

        // check version
        nlohmann::json config;
        {
            std::ifstream stream(filepath);
            stream >> config;
        }

        if (!config.contains("version") || config["version"].get<int>() != 1) {
            Log::error("[LOADING] Invalid or missing version. Version has to be 1.");
            error_count++;
            return res;
        }


        // load the data
        try {
            load_scenario(config["scenario"], *res.simulator);
            load_solver(*res.simulator, config["solver"]);
            res.visualizer = load_visualizer(config["visualizer"]);
        } catch (const std::exception& e) {
            this->error_count++;
            Log::error("[LOADING] An exception occured during loading: " + std::string(e.what()));
            return res;
        }


        return res;
    }


    void SimulationSerializer::save_to_file(const SimulatorVisualizerBundle& simulation) {
        // reset error statistics
        error_count = 0;
        warning_count = 0;

        nlohmann::json config;

        // set version
        config["version"] = 1;


        // save values
        config["scenario"] = save_scenario(*simulation.simulator);
        config["solver"] = save_solver(*simulation.simulator);
        config["visualizer"] = save_visualizer(simulation.visualizer);

        // save data to file
        std::ofstream filestream(filepath);
        filestream << std::setw(4) << config << std::endl;
    }

    void SimulationSerializer::load_particles(ParticleCollection& collection, const std::string& filepath) {
        Serialization::ParticleSerializer stream(filepath);
        stream.deserialize(collection);
    }

    void SimulationSerializer::save_particles(ParticleCollection& collection, const std::string& filepath) {
        Serialization::ParticleSerializer ser(filepath);
        ser.serialize(collection);
    }


    std::shared_ptr<ISimulationVisualizer> SimulationSerializer::deserialize_unknown_visualizer(const nlohmann::json& node) {
        return nullptr;
    }
    std::optional<nlohmann::json> SimulationSerializer::serialize_unknown_visualizer(
            const std::shared_ptr<ISimulationVisualizer>& visualizer) {
        return {};
    }

    nlohmann::json SimulationSerializer::save_particle_remover_3d(const std::shared_ptr<ParticleRemover3D>& remover) {
        nlohmann::json res;
        res["type"] = "particle-remover-3d";

        res["volume"]["center"] = remover->parameters.volume.center;
        res["volume"]["distance-from-center"] = remover->parameters.volume.distance_from_center;
        res["remove-if-outside"] = remover->parameters.remove_if_outside;

        return res;
    }
    std::shared_ptr<ParticleRemover3D> SimulationSerializer::load_particle_remover_3d(const nlohmann::json& node) {
        auto remover = std::make_shared<ParticleRemover3D>();

        remover->parameters.volume.center = node["volume"]["center"].get<glm::vec3>();
        remover->parameters.volume.distance_from_center = node["volume"]["distance-from-center"].get<glm::vec3>();
        remover->parameters.remove_if_outside = node["remove-if-outside"].get<bool>();

        return remover;
    }

    void SimulationSerializer::save_basic_sensor_data(nlohmann::json& sensor_object, const std::shared_ptr<Sensor>& sensor) {
        sensor_object["name"] = sensor->parameters.name;
        sensor_object["save-to-file"] = sensor->parameters.save_to_file;
        sensor_object["keep-data-in-memory"] = sensor->parameters.keep_data_in_memory;
        sensor_object["filename"] = sensor->parameters.filename;
    }

    void SimulationSerializer::load_basic_sensor_data(const nlohmann::json& sensor_object, std::shared_ptr<Sensor> sensor) {
        sensor->parameters.name = sensor_object["name"].get<std::string>();
        sensor->parameters.save_to_file = sensor_object["save-to-file"].get<bool>();
        sensor->parameters.keep_data_in_memory = sensor_object["keep-data-in-memory"].get<bool>();
        if (!sensor_object.contains("filename")) {
            warning_count++;
            sensor->parameters.filename = "xyz.sensor";
        } else {
            sensor->parameters.filename = sensor_object["filename"].get<std::string>();
        }
    }
    nlohmann::json SimulationSerializer::save_boundary_preprocessor(const std::shared_ptr<BoundaryPreprocessor>& ent) {
        nlohmann::json res;
        res["type"] = "boundary-preprocessor";

        return res;
    }
    std::shared_ptr<BoundaryPreprocessor> SimulationSerializer::load_boundary_preprocessor(const nlohmann::json& node) {
        auto res = std::make_shared<BoundaryPreprocessor>();

        return res;
    }


} // namespace LibFluid