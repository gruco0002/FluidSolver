#pragma once

#include "fluidSolver/IFluidSolver.hpp"
#include "fluidSolver/solver/settings/IISPHSettings.hpp"
#include "fluidSolver/solver/settings/IISPHSettings3D.hpp"
#include "fluidSolver/solver/settings/SESPHSettings.hpp"
#include "fluidSolver/solver/settings/SESPHSettings3D.hpp"
#include "serialization/serializers/SerializerBase.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization {
    class SolverSerializer final : public SerializerBase {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<IFluidSolverBase> solver);

      private:
        void serialize_sesph_settings(nlohmann::json& node, const SESPHSettings& settings);
        void serialize_sesph_3d_settings(nlohmann::json& node, const SESPHSettings3D& settings);
        void serialize_iisph_settings(nlohmann::json& node, const IISPHSettings& settings);
        void serialize_iisph_3d_settings(nlohmann::json& node, const IISPHSettings3D& settings);
    };
} // namespace LibFluid::Serialization