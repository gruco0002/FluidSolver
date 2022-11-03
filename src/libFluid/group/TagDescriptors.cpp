#include "TagDescriptors.hpp"
namespace LibFluid {

    TagDescriptors::Descriptor* TagDescriptors::get_descriptor_by_tag(uint32_t particle_tag) {
        for (auto& descriptor : descriptors) {
            if (descriptor.particle_tag == particle_tag) {
                return &descriptor;
            }
        }
        return nullptr;
    }

} // namespace LibFluid