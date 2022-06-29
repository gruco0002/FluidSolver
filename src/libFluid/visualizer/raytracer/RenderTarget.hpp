#pragma once

#include <vector>


namespace FluidSolver::Raytracer {

    struct LightValue {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;

        void add(const LightValue& other);
        void mul(float factor);
        void mul(const LightValue& other);
        bool is_zero() const;
    };

    class RenderTarget {
      public:
        void clear();
        void set_size(size_t width, size_t height);

        size_t get_width() const;
        size_t get_height() const;

        LightValue get(size_t x, size_t y) const;
        void set(size_t x, size_t y, const LightValue& light_value);


      private:
        static constexpr bool row_major_order = true;

        size_t width;
        size_t height;
        std::vector<LightValue> data;

        size_t get_array_index_by_position(size_t x, size_t y) const;
    };


} // namespace FluidSolver::Raytracer