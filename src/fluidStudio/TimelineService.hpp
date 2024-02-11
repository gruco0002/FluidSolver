#pragma once

#include "LibFluidForward.hpp"
#include "time/Timepoint.hpp"
#include <filesystem>
#include <vector>

namespace FluidStudio
{

    class TimelineService
    {
      public:
        void reset();

        void save_timestep_result();

        void override_timestep_result();

        std::shared_ptr<LibFluid::Simulator> simulator = nullptr;

        size_t size() const;

        void step_to(size_t index);

        size_t current() const;

      private:
        struct CachedFile
        {
            std::filesystem::path path;
            LibFluid::Timepoint timepoint;

            void delete_file();
            void create_file(std::shared_ptr<LibFluid::ParticleCollection> particle_collection);
            std::shared_ptr<LibFluid::ParticleCollection> read_file();
        };

        size_t current_index = 0;

        std::vector<CachedFile> cached;

        std::filesystem::path get_cache_folder();

        void clear_cache_folder();
    };
} // namespace FluidStudio
