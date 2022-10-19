#include "ParticleCollectionHelper.hpp"

#include "LibFluidAssert.hpp"
#include "fluidSolver/ParticleCollection.hpp"

namespace FluidStudio::ParticleCollectionHelper {


    uint32_t get_next_free_tag(const std::shared_ptr<LibFluid::ParticleCollection>& collection) {
        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(collection->is_type_present<LibFluid::ParticleInfo>());

        uint32_t max_used_tag = 0;
        for (size_t i = 0; i < collection->size(); i++) {
            const auto& info = collection->get<LibFluid::ParticleInfo>(i);
            if (info.tag > max_used_tag) {
                max_used_tag = info.tag;
            }
        }

        return max_used_tag + 1;
    }
} // namespace FluidStudio::ParticleCollectionHelper
