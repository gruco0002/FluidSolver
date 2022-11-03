#include "JsonHelpers.hpp"
#include "LibFluidAssert.hpp"


namespace glm {
    void to_json(nlohmann::json& j, const glm::vec2& v) {
        j = nlohmann::json::array({v.x, v.y});
    }
    void from_json(const nlohmann::json& j, glm::vec2& v) {
        FLUID_ASSERT(j.is_array() && j.size() == 2, "Assumed a json array of size 2!");
        v.x = j[0].get<float>();
        v.y = j[1].get<float>();
    }
    void to_json(nlohmann::json& j, const glm::vec3& v) {
        j = nlohmann::json::array({v.x, v.y, v.z});
    }
    void from_json(const nlohmann::json& j, glm::vec3& v) {
        FLUID_ASSERT(j.is_array() && j.size() == 3, "Assumed a json array of size 3!");
        v.x = j[0].get<float>();
        v.y = j[1].get<float>();
        v.z = j[2].get<float>();
    }
    void to_json(nlohmann::json& j, const glm::vec4& v) {
        j = nlohmann::json::array({v.x, v.y, v.z, v.w});
    }
    void from_json(const nlohmann::json& j, glm::vec4& v) {
        FLUID_ASSERT(j.is_array() && j.size() == 4, "Assumed a json array of size 4!");
        v.x = j[0].get<float>();
        v.y = j[1].get<float>();
        v.z = j[2].get<float>();
        v.w = j[3].get<float>();
    }
} // namespace glm

namespace LibFluid {

    void to_json(nlohmann::json& j, const Image::Color& c) {
        j = nlohmann::json::array({(int)c.r, (int)c.g, (int)c.b, (int)c.a});
    }
    void from_json(const nlohmann::json& j, Image::Color& c) {
        FLUID_ASSERT(j.is_array() && j.size() == 4, "Assumed a json array of size 4!");
        c.r = (uint8_t)j[0].get<int>();
        c.g = (uint8_t)j[1].get<int>();
        c.b = (uint8_t)j[2].get<int>();
        c.a = (uint8_t)j[3].get<int>();
    }
} // namespace LibFluid
