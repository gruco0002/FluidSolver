#pragma once

#include "entities/SimulationEntity.hpp"
#include <cstddef>
#include <memory>
namespace FluidStudio {
    struct SimulationComponent {
        enum class Kind {
            None,
            Solver,
            Visualizer,
            Timestep,
            Sensor,
            Output,
            Entity
        } kind = Kind::None;
        size_t index = 0;

        bool operator==(const SimulationComponent& other) const;

        bool operator!=(const SimulationComponent& other) const;

        bool can_delete() const;


        static const char* get_entity_type_name(const std::shared_ptr<LibFluid::SimulationEntity> entity);
    };
} // namespace FluidStudio
