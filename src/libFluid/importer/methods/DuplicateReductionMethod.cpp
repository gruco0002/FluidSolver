#include "DuplicateReductionMethod.hpp"

#include <unordered_set>

namespace LibFluid::Importer
{

    std::vector<glm::vec3> DuplicateReductionMethod::reduce_samples(const std::vector<glm::vec3> &samples) const
    {
        struct SetFunctionsForGlmVec3
        {
            size_t operator()(const glm::vec3 &v) const
            {
                return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
            }

            bool operator()(const glm::vec3 &a, const glm::vec3 &b) const
            {
                return a.x == b.x && a.y == b.y && a.z == b.z;
            }
        };

        std::unordered_set<glm::vec3, SetFunctionsForGlmVec3, SetFunctionsForGlmVec3> sample_set(samples.begin(),
                                                                                                 samples.end());

        std::vector<glm::vec3> reduced(sample_set.begin(), sample_set.end());
        return reduced;
    }
} // namespace LibFluid::Importer
