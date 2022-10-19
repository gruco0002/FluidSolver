#pragma once

#include "helpers/DataChangeStruct.hpp"

namespace LibFluid {
    struct SESPHSettings : public DataChangeStruct {
        float StiffnessK = 100000.0f;
        float Viscosity = 5.0f;
    };
}; // namespace LibFluid