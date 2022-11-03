#pragma once

#include "helpers/DataChangeStruct.hpp"

namespace LibFluid {
    struct SESPHSettings3D : public DataChangeStruct {
        float StiffnessK = 100000.0f;
        float Viscosity = 5.0f;

        bool single_layer_boundary = false;
        float single_layer_boundary_gamma_1 = 1.1f;
        float single_layer_boundary_gamma_2 = 1.1f;
    };
} // namespace LibFluid