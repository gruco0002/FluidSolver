#pragma once

#include "serialization/SimulationSerializer.hpp"

namespace FluidUi
{

    class ExtendedSimulationSerializer : public FluidSolver::SimulationSerializer {

      public:
        using FluidSolver::SimulationSerializer::SimulationSerializer;

        virtual std::shared_ptr<FluidSolver::ISimulationVisualizer> deserialize_unknown_visualizer(
            const YAML::Node&) override;

        virtual std::optional<YAML::Node> serialize_unknown_visualizer(
            const std::shared_ptr<FluidSolver::ISimulationVisualizer>&) override;
    };

} // namespace FluidUi