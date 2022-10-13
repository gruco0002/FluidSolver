#pragma once

#include "fluidSolver/IFluidSolver.hpp"
#include "serialization/serializers/SerializerBase.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization {
    class SolverSerializer final : public SerializerBase {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<IFluidSolverBase> solver);
    };
} // namespace LibFluid::Serialization