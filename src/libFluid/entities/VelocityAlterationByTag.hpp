#pragma once

#include "entities/SimulationEntity.hpp"

namespace LibFluid
{
    class VelocityAlterationByTag final : public SimulationEntity
    {
      public:
        VelocityAlterationByTag();

        struct VelocityAlterationByTagParameters : public DataChangeStruct
        {
            glm::vec3 velocity = glm::vec3(0.0f);
            uint32_t tag = -1;
        } parameters;

        void execute_simulation_step(const Timepoint &timepoint, bool before_solver) override;
        void initialize() override;
        void create_compatibility_report(CompatibilityReport &report) override;
    };
} // namespace LibFluid
