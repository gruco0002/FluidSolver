#pragma once

#include "visualizer/Image.hpp"
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace glm {

    // glm::vec2
    void to_json(nlohmann::json&, const glm::vec2&);
    void from_json(const nlohmann::json&, glm::vec2&);

    // glm::vec3
    void to_json(nlohmann::json&, const glm::vec3&);
    void from_json(const nlohmann::json&, glm::vec3&);

    // glm::vec4
    void to_json(nlohmann::json&, const glm::vec4&);
    void from_json(const nlohmann::json&, glm::vec4&);

} // namespace glm

namespace LibFluid {

    // LibFluid::Image::Color
    void to_json(nlohmann::json&, const LibFluid::Image::Color&);
    void from_json(const nlohmann::json&, LibFluid::Image::Color&);


} // namespace LibFluid