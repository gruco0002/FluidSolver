#pragma once

#include "LibFluidTypes.hpp"
#include "helpers/DataChangeStruct.hpp"

namespace LibFluid {
    struct IISPHSettings : public DataChangeStruct {
        pFloat MaxDensityErrorAllowed = 0.001f;

        size_t MinNumberOfIterations = 2;
        size_t MaxNumberOfIterations = 100;

        pFloat Omega = 0.5f;
        pFloat Gamma = 0.7f;

        pFloat Viscosity = 5.0f;
    };
} // namespace LibFluid