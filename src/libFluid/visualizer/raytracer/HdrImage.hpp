#pragma once


#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace LibFluid::Raytracer {

    class HdrImage {
      public:
        HdrImage(size_t width, size_t height);
        HdrImage(size_t width, size_t height, std::vector<glm::vec3> data);
        HdrImage();
        explicit HdrImage(const std::string& filepath);

        size_t width() const;
        size_t height() const;
        size_t size() const;
        glm::vec3* data();
        const glm::vec3* data() const;

        void set(size_t x, size_t y, const glm::vec3& color);
        glm::vec3& get(size_t x, size_t y);
        const glm::vec3& get(size_t x, size_t y) const;

        void save_as_hdr(const std::string& filepath) const;
        bool load_from_hdr(const std::string& filepath);

        std::vector<uint8_t> get_as_hdr_file_data() const;
        bool load_from_hdr_file_data(const std::vector<uint8_t>& data);

        HdrImage apply_box_blur(size_t size) const;

      private:
        std::vector<glm::vec3> m_data;
        size_t m_width = 0;
        size_t m_height = 0;

        void process_hdr_file_data(const float* data, int width, int height);
    };

} // namespace LibFluid::Raytracer
