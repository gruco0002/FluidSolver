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
    };
} // namespace FluidStudio
