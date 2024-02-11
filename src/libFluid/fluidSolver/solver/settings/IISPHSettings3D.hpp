#pragma once

#include "helpers/DataChangeStruct.hpp"

namespace LibFluid
{
    struct IISPHSettings3D : public DataChangeStruct
    {
        float max_density_error_allowed = 0.001f;

        size_t min_number_of_iterations = 2;
        size_t max_number_of_iterations = 100;

        float omega = 0.5f;

        // TODO: rename gamma to single_layer_boundary_gamma_2 if we can fully ignore gamma in multi layer scenarios
        float gamma = 0.7f;

        float viscosity = 5.0f;

        bool single_layer_boundary = false;
        float single_layer_boundary_gamma_1 = 1.1f;
    };
} // namespace LibFluid
