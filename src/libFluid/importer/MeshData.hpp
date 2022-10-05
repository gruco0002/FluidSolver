#pragma once

#include <array>
#include <glm/glm.hpp>
#include <vector>

namespace LibFluid::Importer {
    class MeshData {
      public:
        struct Triangle {
            std::array<glm::vec3, 3> vertices;
            std::array<glm::vec3, 3> normals;

            glm::vec3 get_minimum_coordinates() const;
            glm::vec3 get_maximum_coordinates() const;

            glm::vec3 get_closest_point_on_triangle(const glm::vec3& point) const;
        };

      public:

        explicit MeshData(std::vector<Triangle> triangles);

        std::vector<Triangle> triangles;

    };
} // namespace LibFluid::Importer
