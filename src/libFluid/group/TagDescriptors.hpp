#pragma once

#include <optional>
#include <string>
#include <vector>

namespace LibFluid {
    class TagDescriptors {
      public:
        struct Descriptor {
            std::string title;
            std::string description;
            uint32_t particle_tag;
        };

        std::vector<Descriptor> descriptors;

        Descriptor* get_descriptor_by_tag(uint32_t particle_tag);
    };

} // namespace LibFluid
