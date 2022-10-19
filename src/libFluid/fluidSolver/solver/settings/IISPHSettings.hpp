#pragma once

#include "helpers/DataChangeStruct.hpp"

namespace LibFluid {
    struct IISPHSettings : public DataChangeStruct {
        float MaxDensityErrorAllowed = 0.001f;

        size_t MinNumberOfIterations = 2;
        size_t MaxNumberOfIterations = 100;

        float Omega = 0.5f;
        float Gamma = 0.7f;

        float Viscosity = 5.0f;
    };
} // namespace LibFluid