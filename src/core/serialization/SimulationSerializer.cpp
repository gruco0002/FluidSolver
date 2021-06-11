#include "SimulationSerializer.hpp"

#include <core/Log.hpp>
#include <core/entities/ParticleRemover.hpp>
#include <core/entities/ParticleSpawner.hpp>
#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include <core/fluidSolver/SESPHFluidSolver.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <core/sensors/ParticleStatistics.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include <core/timestep/DynamicCFLTimestep.hpp>
#include <core/visualizer/ContinousVisualizer.hpp>
#include <core/visualizer/GLParticleRenderer.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace YAML
{
    template <> struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.SetStyle(YAML::EmitterStyle::Flow);
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
            {
                return false;
            }
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template <> struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.SetStyle(YAML::EmitterStyle::Flow);
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
            {
                return false;
            }
            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

    template <> struct convert<FluidSolver::Image::Color>
    {
        static Node encode(const FluidSolver::Image::Color& rhs)
        {
            Node node;
            node.SetStyle(YAML::EmitterStyle::Flow);
            node.push_back(rhs.r);
            node.push_back(rhs.g);
            node.push_back(rhs.b);
            node.push_back(rhs.a);
            return node;
        }

        static bool decode(const Node& node, FluidSolver::Image::Color& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
            {
                return false;
            }
            rhs.r = (uint8_t)node[0].as<int>();
            rhs.g = (uint8_t)node[1].as<int>();
            rhs.b = (uint8_t)node[2].as<int>();
            rhs.a = (uint8_t)node[3].as<int>();
            return true;
        }
    };
} // namespace YAML


namespace FluidSolver
{

    // We are using this internal serializer to remove unnecessary parameters from the functions
    // and to avoid "leaking" the YAML header files into the Fluid Solver header files.
    struct InternalSerializer
    {
        size_t errors = 0;
        size_t warnings = 0;
        std::string filepath;
        SimulationSerializer::Settings settings;


        YAML::Node save_particle_spawner(const std::shared_ptr<ParticleSpawner>& spawner)
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

        std::shared_ptr<ParticleSpawner> load_particle_spawner(const YAML::Node& node)
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

        YAML::Node save_particle_remover(const std::shared_ptr<ParticleRemover>& remover)
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


        std::shared_ptr<ParticleRemover> load_particle_remover(const YAML::Node& node)
        {
            auto remover = std::make_shared<ParticleRemover>();

            remover->parameters.area.left = node["area"]["left"].as<float>();
            remover->parameters.area.right = node["area"]["right"].as<float>();
            remover->parameters.area.top = node["area"]["top"].as<float>();
            remover->parameters.area.bottom = node["area"]["bottom"].as<float>();
            remover->parameters.remove_if_outside = node["remove-if-outside"].as<bool>();

            return remover;
        }

        YAML::Node save_global_density_sensor(const std::shared_ptr<Sensors::GlobalDensitySensor>& sen)
        {
            YAML::Node node;

            node["type"] = "global-density-sensor";
            node["name"] = sen->parameters.name;
            node["save-to-file"] = sen->parameters.save_to_file;
            node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

            return node;
        }

        std::shared_ptr<Sensors::GlobalDensitySensor> load_global_density_sensor(const YAML::Node& node)
        {
            auto res = std::make_shared<Sensors::GlobalDensitySensor>();
            res->parameters.name = node["name"].as<std::string>();
            res->parameters.save_to_file = node["save-to-file"].as<bool>();
            res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();

            return res;
        }

        YAML::Node save_global_pressure_sensor(const std::shared_ptr<Sensors::GlobalPressureSensor>& sen)
        {
            YAML::Node node;

            node["type"] = "global-pressure-sensor";
            node["name"] = sen->parameters.name;
            node["save-to-file"] = sen->parameters.save_to_file;
            node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

            return node;
        }

        std::shared_ptr<Sensors::GlobalPressureSensor> load_global_pressure_sensor(const YAML::Node& node)
        {
            auto res = std::make_shared<Sensors::GlobalPressureSensor>();
            res->parameters.name = node["name"].as<std::string>();
            res->parameters.save_to_file = node["save-to-file"].as<bool>();
            res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();

            return res;
        }

        YAML::Node save_global_velocity_sensor(const std::shared_ptr<Sensors::GlobalVelocitySensor>& sen)
        {
            YAML::Node node;

            node["type"] = "global-velocity-sensor";
            node["name"] = sen->parameters.name;
            node["save-to-file"] = sen->parameters.save_to_file;
            node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

            return node;
        }

        std::shared_ptr<Sensors::GlobalVelocitySensor> load_global_velocity_sensor(const YAML::Node& node)
        {
            auto res = std::make_shared<Sensors::GlobalVelocitySensor>();
            res->parameters.name = node["name"].as<std::string>();
            res->parameters.save_to_file = node["save-to-file"].as<bool>();
            res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();
            return res;
        }

        YAML::Node save_global_energy_sensor(const std::shared_ptr<Sensors::GlobalEnergySensor>& sen)
        {
            YAML::Node node;

            node["type"] = "global-energy-sensor";
            node["name"] = sen->parameters.name;
            node["save-to-file"] = sen->parameters.save_to_file;
            node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

            node["relative-zero-height"] = sen->settings.relative_zero_height;

            return node;
        }

        std::shared_ptr<Sensors::GlobalEnergySensor> load_global_energy_sensor(const YAML::Node& node)
        {
            auto res = std::make_shared<Sensors::GlobalEnergySensor>();
            res->parameters.name = node["name"].as<std::string>();
            res->parameters.save_to_file = node["save-to-file"].as<bool>();
            res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();
            res->settings.relative_zero_height = node["relative-zero-height"].as<float>();
            return res;
        }

        YAML::Node save_global_particle_count_sensor(const std::shared_ptr<Sensors::GlobalParticleCountSensor>& sen)
        {
            YAML::Node node;

            node["type"] = "global-particle-count-sensor";
            node["name"] = sen->parameters.name;
            node["save-to-file"] = sen->parameters.save_to_file;
            node["keep-data-in-memory-after-saving"] = sen->parameters.keep_data_in_memory_after_saving;

            return node;
        }

        std::shared_ptr<Sensors::GlobalParticleCountSensor> load_global_particle_count_sensor(const YAML::Node& node)
        {
            auto res = std::make_shared<Sensors::GlobalParticleCountSensor>();
            res->parameters.name = node["name"].as<std::string>();
            res->parameters.save_to_file = node["save-to-file"].as<bool>();
            res->parameters.keep_data_in_memory_after_saving = node["keep-data-in-memory-after-saving"].as<bool>();
            return res;
        }

        std::string get_full_particle_data_path()
        {
            auto p = std::filesystem::path(filepath);
            auto combined = p.parent_path() / std::filesystem::path(settings.particle_data_relative_filepath);
            return combined.string();
        }

        YAML::Node save_scenario(const Simulation& simulation)
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
            }

            return res;
        }

        void load_scenario(const YAML::Node& node, Simulation& simulation)
        {

            // loading particles from file
            auto particles = node["particles"].as<std::string>();
            auto particles_file = std::filesystem::path(filepath).parent_path() / std::filesystem::path(particles);
            if (!std::filesystem::exists(particles_file))
            {
                Log::error("[LOADING] Particles file was not found under '" + particles_file.string() + "'!");
                errors++;
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
                        warnings++;
                        Log::warning("[LOADING] Unknown entity type: '" + ent_node["type"].as<std::string>() + "'!");
                    }
                }
            }

            // loading sensors
            if (node["sensors"])
            {
                for (auto& sen_node : node["sensors"])
                {
                    if (sen_node["type"].as<std::string>() == "global-density-sensor")
                    {
                        simulation.parameters.sensors.push_back(load_global_density_sensor(sen_node));
                    }
                    else if (sen_node["type"].as<std::string>() == "global-pressure-sensor")
                    {
                        simulation.parameters.sensors.push_back(load_global_pressure_sensor(sen_node));
                    }
                    else if (sen_node["type"].as<std::string>() == "global-velocity-sensor")
                    {
                        simulation.parameters.sensors.push_back(load_global_velocity_sensor(sen_node));
                    }
                    else if (sen_node["type"].as<std::string>() == "global-energy-sensor")
                    {
                        simulation.parameters.sensors.push_back(load_global_energy_sensor(sen_node));
                    }
                    else if (sen_node["type"].as<std::string>() == "global-particle-count-sensor")
                    {
                        simulation.parameters.sensors.push_back(load_global_particle_count_sensor(sen_node));
                    }
                    else
                    {
                        warnings++;
                        Log::warning("[LOADING] Unknown sensor type '" + sen_node["type"].as<std::string>() + "'!");
                    }
                }
            }
        }

        YAML::Node save_timestep(const std::shared_ptr<ITimestep>& timestep)
        {
            YAML::Node node;

            if (std::dynamic_pointer_cast<ConstantTimestep>(timestep))
            {
                auto t = std::dynamic_pointer_cast<ConstantTimestep>(timestep);

                node["type"] = "constant";
                node["timestep"] = t->settings.timestep;
            }
            else if (std::dynamic_pointer_cast<DynamicCFLTimestep>(timestep))
            {
                auto t = std::dynamic_pointer_cast<DynamicCFLTimestep>(timestep);

                node["type"] = "dynamic-cfl";
                node["max-timestep"] = t->settings.max_timestep;
                node["min-timestep"] = t->settings.min_timestep;
                node["cfl"] = t->settings.cfl_number;
            }
            else
            {
                errors++;
                Log::error("[SAVING] Unsupported timestep type!");
            }


            return node;
        }

        std::shared_ptr<ITimestep> load_timestep(const YAML::Node& node)
        {

            if (node["type"].as<std::string>() == "constant")
            {
                auto res = std::make_shared<ConstantTimestep>();
                res->settings.timestep = node["timestep"].as<float>();
                return res;
            }
            else if (node["type"].as<std::string>() == "dynamic-cfl")
            {
                auto res = std::make_shared<DynamicCFLTimestep>();
                res->settings.max_timestep = node["max-timestep"].as<float>();
                res->settings.min_timestep = node["min-timestep"].as<float>();
                res->settings.cfl_number = node["cfl"].as<float>();
                return res;
            }
            else
            {
                errors++;
                Log::error("[LOADING] Unknown timestep type '" + node["type"].as<std::string>() + "'!");
            }
            return nullptr;
        }

        YAML::Node save_solver(const Simulation& simulation)
        {
            YAML::Node node;

            // solver, neighborhood search, kernel
            auto s = simulation.parameters.fluid_solver;

            if (std::dynamic_pointer_cast<
                    SESPHFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated>>(s))
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
            else
            {
                errors++;
                Log::error("[SAVING] Unsupported combination of fluid solver components!");
            }


            // timestep
            node["timestep"] = save_timestep(simulation.parameters.timestep);


            return node;
        }

        void load_solver(Simulation& simulation, const YAML::Node& node)
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
                        errors++;
                        Log::error("[LOADING] Unknown neighborhood search type '" +
                                   node["neigborhood-search"]["type"].as<std::string>() + "'!");
                    }
                }
                else
                {
                    errors++;
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
                        errors++;
                        Log::error("[LOADING] Unknown neighborhood search type '" +
                                   node["neigborhood-search"]["type"].as<std::string>() + "'!");
                    }
                }
                else
                {
                    errors++;
                    Log::error("[LOADING] Unknown kernel type '" + node["kernel"]["type"].as<std::string>() + "'!");
                }
            }
            else
            {
                errors++;
                Log::error("[LOADING] Unknown solver type '" + node["type"].as<std::string>() + "'!");
            }

            // timestep
            simulation.parameters.timestep = load_timestep(node["timestep"]);
        }

        std::shared_ptr<ISimulationVisualizer> load_visualizer(const YAML::Node& node)
        {
            if (node["type"].as<std::string>() == "gl-particle-renderer")
            {
                if (!GLRenderer::is_opengl_available())
                {
                    errors++;
                    Log::error("[LOADING] Visualizer is not supported in this context. OpenGL was not initialized but "
                               "visualizer requires OpenGL!");
                    return nullptr;
                }
                auto r = std::make_shared<GLParticleRenderer>();

                // default parameters
                r->settings.viewport.left = node["viewport"]["left"].as<float>();
                r->settings.viewport.right = node["viewport"]["right"].as<float>();
                r->settings.viewport.top = node["viewport"]["top"].as<float>();
                r->settings.viewport.bottom = node["viewport"]["bottom"].as<float>();
                r->parameters.render_target.width = node["render-target"]["width"].as<size_t>();
                r->parameters.render_target.height = node["render-target"]["height"].as<size_t>();

                // custom paramters for the particle renderer
                r->settings.topValue = node["settings"]["top"]["value"].as<float>();
                r->settings.topColor = node["settings"]["top"]["color"].as<glm::vec4>();
                r->settings.bottomValue = node["settings"]["bottom"]["value"].as<float>();
                r->settings.bottomColor = node["settings"]["bottom"]["color"].as<glm::vec4>();
                r->settings.colorSelection =
                    (GLParticleRenderer::Settings::ColorSelection)node["settings"]["value-selection"].as<int>();
                r->settings.boundaryParticleColor = node["settings"]["colors"]["boundary"].as<glm::vec4>();
                r->settings.backgroundClearColor = node["settings"]["colors"]["background"].as<glm::vec4>();
                r->settings.showMemoryLocation = node["settings"]["show-memory-location"].as<bool>();

                return r;
            }
            else if (node["type"].as<std::string>() == "no-visualizer")
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
                warnings++;
                Log::warning("[LOADING] Unknown visualizer type '" + node["type"].as<std::string>() + "'!");
            }

            return nullptr;
        }


        YAML::Node save_visualizer(const std::shared_ptr<ISimulationVisualizer>& visualizer)
        {
            YAML::Node node;
            if (std::dynamic_pointer_cast<const GLParticleRenderer>(visualizer) != nullptr)
            {
                auto r = std::dynamic_pointer_cast<const GLParticleRenderer>(visualizer);
                node["type"] = "gl-particle-renderer";

                // default parameters
                node["viewport"]["left"] = r->settings.viewport.left;
                node["viewport"]["right"] = r->settings.viewport.right;
                node["viewport"]["top"] = r->settings.viewport.top;
                node["viewport"]["bottom"] = r->settings.viewport.bottom;
                node["render-target"]["width"] = r->parameters.render_target.width;
                node["render-target"]["height"] = r->parameters.render_target.height;

                // custom parameters for the particle renderer
                node["settings"]["top"]["value"] = r->settings.topValue;
                node["settings"]["top"]["color"] = r->settings.topColor;
                node["settings"]["bottom"]["value"] = r->settings.bottomValue;
                node["settings"]["bottom"]["color"] = r->settings.bottomColor;
                node["settings"]["value-selection"] = (int)r->settings.colorSelection;
                node["settings"]["colors"]["boundary"] = r->settings.boundaryParticleColor;
                node["settings"]["colors"]["background"] = r->settings.backgroundClearColor;
                node["settings"]["show-memory-location"] = r->settings.showMemoryLocation;
            }
            else if (visualizer == nullptr)
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
                warnings++;
                Log::warning("[SAVING] Unsupported visualizer type!");
            }

            return node;
        }
    };


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

        // create internal interface
        InternalSerializer s;
        s.filepath = this->filepath;
        s.settings = settings;

        // load the data
        try
        {
            s.load_scenario(config["scenario"], res);
            s.load_solver(res, config["solver"]);
            res.parameters.visualizer = s.load_visualizer(config["visualizer"]);
        }
        catch (const std::exception& e)
        {
            this->error_count++;
            Log::error("[LOADING] An exception occured during loading: " + std::string(e.what()));
            return res;
        }


        // transfer error information
        this->error_count = s.errors;
        this->warning_count = s.warnings;

        return res;
    }


    void SimulationSerializer::save_to_file(const Simulation& simulation)
    {
        YAML::Node config;

        // set version
        config["version"] = "1";

        // create internal interface
        InternalSerializer s;
        s.filepath = this->filepath;
        s.settings = this->settings;

        // save values
        config["scenario"] = s.save_scenario(simulation);
        config["solver"] = s.save_solver(simulation);
        config["visualizer"] = s.save_visualizer(simulation.parameters.visualizer);

        // transfer error information
        this->error_count = s.errors;
        this->warning_count = s.warnings;

        // save data to file
        std::ofstream filestream(filepath);
        filestream << config;
    }

    void SimulationSerializer::load_particles(ParticleCollection& collection, const std::string& filepath)
    {
        // reading in file data
        std::ifstream data(filepath, std::ios::in | std::ios::binary);
        nlohmann::json j = nlohmann::json::from_msgpack(data);

        // getting metadata
        size_t count = j["count"];
        collection.resize(count);

        // adding types if needed
        if (j["data"].contains("movementData"))
        {
            if (j["data"]["movementData"].size() != count)
            {
                Log::error("[LOADING] Particle data has insufficient movementData available!");
            }
            if (!collection.is_type_present<MovementData>())
                collection.add_type<MovementData>();
        }

        if (j["data"].contains("movementData3D"))
        {
            if (j["data"]["movementData3D"].size() != count)
            {
                Log::error("[LOADING] Particle data has insufficient movementData3D available!");
            }
            if (!collection.is_type_present<MovementData3D>())
                collection.add_type<MovementData3D>();
        }

        if (j["data"].contains("particleInfo"))
        {
            if (j["data"]["particleInfo"].size() != count)
            {
                Log::error("[LOADING] Particle data has insufficient particleInfo available!");
            }
            if (!collection.is_type_present<ParticleInfo>())
                collection.add_type<ParticleInfo>();
        }

        if (j["data"].contains("particleData"))
        {
            if (j["data"]["particleData"].size() != count)
            {
                Log::error("[LOADING] Particle data has insufficient particleData available!");
            }
            if (!collection.is_type_present<ParticleData>())
                collection.add_type<ParticleData>();
        }

        if (j["data"].contains("externalForces"))
        {
            if (j["data"]["externalForces"].size() != count)
            {
                Log::error("[LOADING] Particle data has insufficient externalForces available!");
            }
            if (!collection.is_type_present<ExternalForces>())
                collection.add_type<ExternalForces>();
        }

        if (j["data"].contains("externalForces3D"))
        {
            if (j["data"]["externalForces3D"].size() != count)
            {
                Log::error("[LOADING] Particle data has insufficient externalForces3D available!");
            }
            if (!collection.is_type_present<ExternalForces3D>())
                collection.add_type<ExternalForces3D>();
        }


        // load data from particles
        for (size_t i = 0; i < collection.size(); i++)
        {
            if (collection.is_type_present<MovementData>())
            {
                auto& m = collection.get<MovementData>(i);

                m.position.x = j["data"]["movementData"][i]["p"][0];
                m.position.y = j["data"]["movementData"][i]["p"][1];
                m.velocity.x = j["data"]["movementData"][i]["v"][0];
                m.velocity.y = j["data"]["movementData"][i]["v"][1];
                m.acceleration.x = j["data"]["movementData"][i]["a"][0];
                m.acceleration.y = j["data"]["movementData"][i]["a"][1];
            }

            if (collection.is_type_present<MovementData3D>())
            {
                auto& m = collection.get<MovementData3D>(i);
                m.position.x = j["data"]["movementData3D"][i]["p"][0];
                m.position.y = j["data"]["movementData3D"][i]["p"][1];
                m.position.z = j["data"]["movementData3D"][i]["p"][2];
                m.velocity.x = j["data"]["movementData3D"][i]["v"][0];
                m.velocity.y = j["data"]["movementData3D"][i]["v"][1];
                m.velocity.z = j["data"]["movementData3D"][i]["v"][2];
                m.acceleration.x = j["data"]["movementData3D"][i]["a"][0];
                m.acceleration.y = j["data"]["movementData3D"][i]["a"][1];
                m.acceleration.z = j["data"]["movementData3D"][i]["a"][2];
            }

            if (collection.is_type_present<ParticleInfo>())
            {
                auto& p = collection.get<ParticleInfo>(i);
                p.tag = j["data"]["particleInfo"][i]["t"];
                p.type = j["data"]["particleInfo"][i]["k"];
            }

            if (collection.is_type_present<ParticleData>())
            {
                auto& d = collection.get<ParticleData>(i);
                d.density = j["data"]["particleData"][i]["d"];
                d.mass = j["data"]["particleData"][i]["m"];
                d.pressure = j["data"]["particleData"][i]["p"];
            }

            if (collection.is_type_present<ExternalForces>())
            {
                auto& e = collection.get<ExternalForces>(i);
                e.non_pressure_acceleration.x = j["data"]["externalForces"][i]["n"][0];
                e.non_pressure_acceleration.y = j["data"]["externalForces"][i]["n"][1];
            }

            if (collection.is_type_present<ExternalForces3D>())
            {
                auto& e = collection.get<ExternalForces3D>(i);
                e.non_pressure_acceleration.x = j["data"]["externalForces"][i]["n"][0];
                e.non_pressure_acceleration.y = j["data"]["externalForces"][i]["n"][1];
                e.non_pressure_acceleration.z = j["data"]["externalForces"][i]["n"][2];
            }
        }
    }

    void SimulationSerializer::save_particles(ParticleCollection& collection, const std::string& filepath)
    {

        // create header information
        nlohmann::json j;

        j["type"] = "particle-data-file";
        j["version"] = 1;

        j["count"] = collection.size();
        j["data"] = nlohmann::json::object();

        // create all fields for the respective types
        if (collection.is_type_present<MovementData>())
        {
            j["data"]["movementData"] = nlohmann::json::array();
        }

        if (collection.is_type_present<MovementData3D>())
        {
            j["data"]["movementData3D"] = nlohmann::json::array();
        }

        if (collection.is_type_present<ParticleInfo>())
        {
            j["data"]["particleInfo"] = nlohmann::json::array();
        }

        if (collection.is_type_present<ParticleData>())
        {
            j["data"]["particleData"] = nlohmann::json::array();
        }

        if (collection.is_type_present<ExternalForces>())
        {
            j["data"]["externalForces"] = nlohmann::json::array();
        }

        if (collection.is_type_present<ExternalForces3D>())
        {
            j["data"]["externalForces3D"] = nlohmann::json::array();
        }


        // write particle data
        for (size_t i = 0; i < collection.size(); i++)
        {

            if (collection.is_type_present<MovementData>())
            {
                auto& m = collection.get<MovementData>(i);
                j["data"]["movementData"].push_back({
                    {"p", {m.position.x, m.position.y}},
                    {"v", {m.velocity.x, m.velocity.y}},
                    {"a", {m.acceleration.x, m.acceleration.y}},
                });
            }

            if (collection.is_type_present<MovementData3D>())
            {
                auto& m = collection.get<MovementData3D>(i);
                j["data"]["movementData3D"].push_back({
                    {"p", {m.position.x, m.position.y, m.position.z}},
                    {"v", {m.velocity.x, m.velocity.y, m.velocity.z}},
                    {"a", {m.acceleration.x, m.acceleration.y, m.acceleration.z}},
                });
            }

            if (collection.is_type_present<ParticleInfo>())
            {
                auto& p = collection.get<ParticleInfo>(i);
                j["data"]["particleInfo"].push_back({{"t", p.tag}, {"k", p.type}});
            }

            if (collection.is_type_present<ParticleData>())
            {
                auto& d = collection.get<ParticleData>(i);
                j["data"]["particleData"].push_back({{"d", d.density}, {"m", d.mass}, {"p", d.pressure}});
            }

            if (collection.is_type_present<ExternalForces>())
            {
                auto& e = collection.get<ExternalForces>(i);
                j["data"]["externalForces"].push_back(
                    {{"n", {e.non_pressure_acceleration.x, e.non_pressure_acceleration.y}}});
            }

            if (collection.is_type_present<ExternalForces3D>())
            {
                auto& e = collection.get<ExternalForces3D>(i);
                j["data"]["externalForces3D"].push_back(
                    {{"n",
                      {e.non_pressure_acceleration.x, e.non_pressure_acceleration.y, e.non_pressure_acceleration.z}}});
            }
        }


        // write to file
        auto binary_data = nlohmann::json::to_msgpack(j);
        std::ofstream data(filepath, std::ios::out | std::ios::binary);
        data.write(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
    }


} // namespace FluidSolver