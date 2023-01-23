#include "ObjLoader.hpp"

#include "LibFluidAssert.hpp"
#include "helpers/Log.hpp"

#include <tiny_obj_loader.h>

namespace LibFluid::Importer {


    ObjLoader::ObjLoader(std::filesystem::path filepath) {
        this->filepath = std::move(filepath);
    }


    MeshData ObjLoader::load_as_meshdata() {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;


        // load obj file data
        {
            std::string warning;
            std::string error;

            auto filepath_as_string = filepath.string();
            bool loaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, filepath_as_string.c_str());

            if (!warning.empty()) {
                Log::warning("ObjLoader: " + warning);
            }

            if (!error.empty()) {
                Log::error("ObjLoader: " + error);
            }

            if (!loaded) {
                Log::error("ObjLoader: Error while loading obj file!");
                return MeshData({});
            }
        }

        // convert file data into mesh data
        std::vector<MeshData::Triangle> triangles;


        for (const auto& shape : shapes) {
            const auto& mesh = shape.mesh;
            size_t i = 0;
            for (const auto& num_vertices : mesh.num_face_vertices) {
                FLUID_ASSERT(num_vertices == 3, "ObjLoader only supports triangulated mesh data!");

                auto index_0 = mesh.indices[i + 0];
                auto index_1 = mesh.indices[i + 1];
                auto index_2 = mesh.indices[i + 2];

                FLUID_ASSERT(index_0.vertex_index * 3 + 2 < attrib.vertices.size(), "ObjLoader vertex index out of range!");
                FLUID_ASSERT(index_1.vertex_index * 3 + 2 < attrib.vertices.size(), "ObjLoader vertex index out of range!");
                FLUID_ASSERT(index_2.vertex_index * 3 + 2 < attrib.vertices.size(), "ObjLoader vertex index out of range!");

                MeshData::Triangle triangle {};

                triangle.vertices[0] = glm::vec3(attrib.vertices[index_0.vertex_index * 3 + 0], attrib.vertices[index_0.vertex_index * 3 + 1], attrib.vertices[index_0.vertex_index * 3 + 2]) * scale;
                triangle.vertices[1] = glm::vec3(attrib.vertices[index_1.vertex_index * 3 + 0], attrib.vertices[index_1.vertex_index * 3 + 1], attrib.vertices[index_1.vertex_index * 3 + 2]) * scale;
                triangle.vertices[2] = glm::vec3(attrib.vertices[index_2.vertex_index * 3 + 0], attrib.vertices[index_2.vertex_index * 3 + 1], attrib.vertices[index_2.vertex_index * 3 + 2]) * scale;


                if (index_0.normal_index == -1 || index_1.normal_index == -1 || index_2.normal_index == -1) {
                    // normals are not defined, calculate it from the vertices
                    auto normal = triangle.get_normal_from_vertices();
                    triangle.normals[0] = normal;
                    triangle.normals[1] = normal;
                    triangle.normals[2] = normal;
                } else {
                    FLUID_ASSERT(index_0.normal_index * 3 + 2 < attrib.normals.size(), "ObjLoader normal index out of range!");
                    FLUID_ASSERT(index_1.normal_index * 3 + 2 < attrib.normals.size(), "ObjLoader normal index out of range!");
                    FLUID_ASSERT(index_2.normal_index * 3 + 2 < attrib.normals.size(), "ObjLoader normal index out of range!");
                    triangle.normals[0] = glm::vec3(attrib.normals[index_0.normal_index * 3 + 0], attrib.normals[index_0.normal_index * 3 + 1], attrib.normals[index_0.normal_index * 3 + 2]);
                    triangle.normals[1] = glm::vec3(attrib.normals[index_1.normal_index * 3 + 0], attrib.normals[index_1.normal_index * 3 + 1], attrib.normals[index_1.normal_index * 3 + 2]);
                    triangle.normals[2] = glm::vec3(attrib.normals[index_2.normal_index * 3 + 0], attrib.normals[index_2.normal_index * 3 + 1], attrib.normals[index_2.normal_index * 3 + 2]);
                }

                triangles.push_back(triangle);

                i += num_vertices;
            }
        }


        return MeshData(triangles);
    }
} // namespace LibFluid::Importer