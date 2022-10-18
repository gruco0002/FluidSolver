#pragma once

#include "DataChangeStruct.hpp"
#include "FluidTypes.hpp"

namespace LibFluid {
    struct SESPHSettings : public DataChangeStruct {
        pFloat StiffnessK = 100000.0f;
        pFloat Viscosity = 5.0f;
    };
}; // namespace LibFluid