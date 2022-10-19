#pragma once

#include "LibFluidForward.hpp"

#include <memory>

namespace FluidStudio::ParticleCollectionHelper {

    uint32_t get_next_free_tag(const std::shared_ptr<LibFluid::ParticleCollection>& collection);

}
