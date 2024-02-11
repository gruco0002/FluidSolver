#pragma once

#include "LibFluidForward.hpp"

#include <memory>

namespace FluidStudio::SimulatorHelpers
{

    bool is_3d_simulation(const std::shared_ptr<LibFluid::Simulator> &simulator);
}
