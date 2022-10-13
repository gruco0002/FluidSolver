#pragma once

#include <filesystem>
#include <optional>

namespace LibFluid::Serialization {
    struct SerializationContext {
        std::filesystem::path json_filepath;
        std::optional<std::filesystem::path> particle_filepath;


    };
} // namespace LibFluid::Serialization
