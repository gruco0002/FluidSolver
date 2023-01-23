#include "MeshData.hpp"

#include "LibFluidAssert.hpp"
#include "LibFluidMath.hpp"

namespace LibFluid::Importer {


    MeshData::MeshData(std::vector<Triangle> triangles) {
        this->triangles = std::move(triangles);
    }

    float MeshData::get_area() const {
        float area = 0.0f;
        for (const auto& triangle : triangles) {
            area += triangle.get_area();
        }
        return area;
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


    std::pair<glm::vec3, float> get_nearest_point_on_line(const glm::vec3& origin, const glm::vec3& line_vector, const glm::vec3& point) {
        glm::vec3 origin_to_point = point - origin;

        float t = glm::dot(line_vector, origin_to_point);

        glm::vec3 point_on_line = origin + t * line_vector;

        // returns the point on the line and the factor t of the line equation such that
        // point_on_line = origin + t * line_vector
        return {point_on_line, t};
    }

    glm::vec3 MeshData::Triangle::get_closest_point_on_triangle(const glm::vec3& point) const {
        // define our triangle vertices as a, b and c
        const auto& a = vertices[0];
        const auto& b = vertices[1];
        const auto& c = vertices[2];

        // get the normal and origin of the plane defined by the triangle
        glm::vec3 normal = get_normal_from_vertices();
        const auto& origin = a;

        // project the point onto the plane defined by the triangle
        glm::vec3 origin_to_point = point - origin;
        float distance_from_point_to_plane = glm::dot(origin_to_point, normal);

        glm::vec3 point_projected_onto_plane = point - normal * distance_from_point_to_plane;

        // check if the point lies within the triangle, if yes we found the solution
        glm::vec3 barycentric = get_barycentric_coordinates_of(point_projected_onto_plane);

        if (barycentric.x >= 0.0f && barycentric.x <= 1.0f && barycentric.y >= 0.0f && barycentric.y <= 1.0f && barycentric.z >= 0.0f && barycentric.z <= 1.0f) {
            // see https://math.stackexchange.com/questions/4322/check-whether-a-point-is-within-a-3d-triangle
            // the point lies within the triangle
            return point_projected_onto_plane;
        }

        // the point was not inside the triangle, hence it lies outside
        // the nearest point is therefore one of the corners or lies on one of the edges

        // first determine distances to corners
        float distance_to_a = glm::length(point - a);
        float distance_to_b = glm::length(point - b);
        float distance_to_c = glm::length(point - c);

        // determine distances and intersections with edges
        auto line_ab_intersection = get_nearest_point_on_line(a, b - a, point);
        auto line_ac_intersection = get_nearest_point_on_line(a, c - a, point);
        auto line_bc_intersection = get_nearest_point_on_line(b, c - b, point);

        // find the best match
        float current_distance = distance_to_a;
        glm::vec3 current_point = a;

        // check if the other corners are closer
        if (distance_to_b < current_distance) {
            current_point = b;
        }
        if (distance_to_c < current_distance) {
            current_point = c;
        }

        // check if the intersections with the edges are close and if those intersections lie on the edge of the triangle

        if (line_ab_intersection.second >= 0.0f && line_ab_intersection.second <= 1.0f) {
            // the line intersection is within the edge segment of the triangle
            float distance_to_intersection = glm::length(point - line_ab_intersection.first);
            if (distance_to_intersection < current_distance) {
                current_point = line_ab_intersection.first;
            }
        }

        if (line_ac_intersection.second >= 0.0f && line_ac_intersection.second <= 1.0f) {
            // the line intersection is within the edge segment of the triangle
            float distance_to_intersection = glm::length(point - line_ac_intersection.first);
            if (distance_to_intersection < current_distance) {
                current_point = line_ac_intersection.first;
            }
        }

        if (line_bc_intersection.second >= 0.0f && line_bc_intersection.second <= 1.0f) {
            // the line intersection is within the edge segment of the triangle
            float distance_to_intersection = glm::length(point - line_bc_intersection.first);
            if (distance_to_intersection < current_distance) {
                current_point = line_bc_intersection.first;
            }
        }

        return current_point;
    }

    glm::vec3 MeshData::Triangle::get_barycentric_coordinates_of(const glm::vec3& cartesian_point) const {
        // see https://math.stackexchange.com/questions/4322/check-whether-a-point-is-within-a-3d-triangle
        const auto& a = vertices[0];
        const auto& b = vertices[1];
        const auto& c = vertices[2];

        const auto& p = cartesian_point;
        auto normal = get_normal_from_vertices();

        float area_abc = get_area();
        float area_pbc = glm::length(glm::cross((b - p), (c - p))) / 2.0f;
        float area_pca = glm::length(glm::cross((c - p), (a - p))) / 2.0f;

        FLUID_ASSERT(area_abc > 0.0f, "Triangle with area smaller or equal to zero is not sound!");

        float alpha = area_pbc / area_abc;
        float beta = area_pca / area_abc;
        float gamma = 1.0f - alpha - beta;

        return {alpha, beta, gamma};
    }

    glm::vec3 MeshData::Triangle::get_normal_from_vertices() const {
        // define our triangle vertices as a, b and c
        const auto& a = vertices[0];
        const auto& b = vertices[1];
        const auto& c = vertices[2];

        // get the normal of the plane defined by the triangle
        glm::vec3 normal = glm::cross((b - a), (c - a));
        normal = normal / glm::length(normal);

        return normal;
    }

    float MeshData::Triangle::get_area() const {
        // define our triangle vertices as a, b and c
        const auto& a = vertices[0];
        const auto& b = vertices[1];
        const auto& c = vertices[2];

        // get the normal of the plane defined by the triangle
        glm::vec3 normal = glm::cross((b - a), (c - a));

        float area = glm::length(normal) / 2.0f;

        return area;
    }


} // namespace LibFluid::Importer