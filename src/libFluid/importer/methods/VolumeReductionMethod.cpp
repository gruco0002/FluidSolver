#include "VolumeReductionMethod.hpp"
namespace LibFluid::Importer {

    std::vector<glm::vec3> VolumeReductionMethod::reduce_samples(const std::vector<glm::vec3>& samples) const {
        // Idea: We add particles as long as their volume does not fall below a certain value
        // But we always add particles if one of their neighbors has a volume above a certain value (too few samples in its area)


        // TODO: implement
        return samples;
    }
} // namespace LibFluid::Importer