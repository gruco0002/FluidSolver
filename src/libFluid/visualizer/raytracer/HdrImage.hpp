#pragma once

#include "RenderTarget.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace LibFluid::Raytracer {

    class HdrImage {
      public:
        size_t width() const;
        size_t height() const;
        size_t size() const;
        LightValue* data();
        const LightValue* data() const;

        void set(size_t x, size_t y, const LightValue& color);
        LightValue& get(size_t x, size_t y);
        const LightValue& get(size_t x, size_t y) const;

        // TODO: add a load function

      private:
        std::vector<LightValue> m_data;
        size_t m_width = 0;
        size_t m_height = 0;
    };

} // namespace LibFluid::Raytracer
