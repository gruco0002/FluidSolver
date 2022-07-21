#pragma once
#include "CompatibilityReport.hpp"

namespace LibFluid {

    /**
     * This interface is used to keep the codebase cleaner.
     */
    class Reportable {
      public:
        virtual void create_compatibility_report(CompatibilityReport& report) = 0;

        virtual ~Reportable() = default;
    };
} // namespace FluidSolver