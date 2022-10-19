#pragma once

#include "LibFluidTypes.hpp"
#include "helpers/DataChangeStruct.hpp"

namespace LibFluid {
    struct SESPHSettings : public DataChangeStruct {
        pFloat StiffnessK = 100000.0f;
        pFloat Viscosity = 5.0f;
    };
}; // namespace LibFluid