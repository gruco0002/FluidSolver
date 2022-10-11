#pragma once

#include "fluidSolver/Particle.hpp"
#include "importer/MeshData.hpp"

namespace LibFluid::Importer {
    class ParticleSampler {
      public:
        explicit ParticleSampler(MeshData mesh_data, float particle_size);

        void generate_samples();

        const std::vector<glm::vec3>& get_samples() const;

      private:
        MeshData mesh_data;
        float particle_size;

        std::vector<glm::vec3> samples;

        void add_samples_for_triangle(const MeshData::Triangle& triangle);

        void reduce_samples();

        void reduce_duplicates();

        void reduce_too_close_ones();

        glm::vec3 get_grid_aligned_vector(const glm::vec3& value, bool subtract) const;
    };

} // namespace LibFluid::Importer