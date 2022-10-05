#include "MeshData.hpp"

#include "FluidMath.hpp"

namespace LibFluid::Importer {


    MeshData::MeshData(std::vector<Triangle> triangles) {
        this->triangles = std::move(triangles);
    }

    glm::vec3 MeshData::Triangle::get_minimum_coordinates() const {
        return {
                Math::min(vertices[0].x, Math::min(vertices[1].x, vertices[2].x)),
                Math::min(vertices[0].y, Math::min(vertices[1].y, vertices[2].y)),
                Math::min(vertices[0].z, Math::min(vertices[1].z, vertices[2].z))};
    }

    glm::vec3 MeshData::Triangle::get_maximum_coordinates() const {
        return {
                Math::max(vertices[0].x, Math::max(vertices[1].x, vertices[2].x)),
                Math::max(vertices[0].y, Math::max(vertices[1].y, vertices[2].y)),
                Math::max(vertices[0].z, Math::max(vertices[1].z, vertices[2].z))};
    }

    glm::vec3 MeshData::Triangle::get_closest_point_on_triangle(const glm::vec3& point) const {
        // define our triangle vertices as a, b and c
        const auto& a = vertices[0];
        const auto& b = vertices[1];
        const auto& c = vertices[2];

        // get the normal and origin of the plane defined by the triangle
        glm::vec3 normal = glm::cross((b - a), (c - a));
        normal = normal / glm::length(normal);

        const auto& origin = a;

        // project the point onto the plane defined by the triangle
        glm::vec3 origin_to_point = point - origin;
        float distance_from_point_to_plane = glm::dot(origin_to_point, normal);

        glm::vec3 point_projected_onto_plane = point - normal * distance_from_point_to_plane;

        // check if the point lies within the triangle, if yes we found the solution

        // TODO: implement

        // the point was not inside the triangle, hence it lies outside
        // the nearest point is therefore one of the corners or lies on one of the edges

        // TODO: implement

    }
} // namespace LibFluid::Importer