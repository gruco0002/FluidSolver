#include "TimelineService.hpp"

#include "DirectoryHelper.hpp"
#include "Simulator.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "serialization/ParticleSerializer.hpp"

namespace FluidStudio
{

    void TimelineService::CachedFile::delete_file()
    {
        if (std::filesystem::exists(path))
        {
            std::filesystem::remove(path);
        }
    }

    void TimelineService::CachedFile::create_file(std::shared_ptr<LibFluid::ParticleCollection> particle_collection)
    {
        LibFluid::Serialization::ParticleSerializer serializer(path);
        serializer.serialize(*particle_collection);
    }

    std::shared_ptr<LibFluid::ParticleCollection> TimelineService::CachedFile::read_file()
    {
        auto res = std::make_shared<LibFluid::ParticleCollection>();
        LibFluid::Serialization::ParticleSerializer deserializer(path);
        deserializer.deserialize(*res);

        return res;
    }

    void TimelineService::reset()
    {
        clear_cache_folder();
        cached.clear();
        current_index = 0;
    }

    void TimelineService::save_timestep_result()
    {
        // create and save the cached file
        CachedFile c;
        c.timepoint = simulator->get_current_timepoint();
        c.path = get_cache_folder() / (std::to_string(c.timepoint.timestep_number) + ".data");
        c.create_file(simulator->data.collection);

        if (cached.size() > 0)
        {
            if (current_index != cached.size() - 1)
            {
                // we are stepped back, delete everything in the future
                cached.resize(current_index + 1);
            }

            current_index++;
        }
        else
        {
            current_index = 0;
        }
        cached.push_back(c);
    }

    size_t TimelineService::size() const
    {
        return cached.size();
    }

    void TimelineService::step_to(size_t index)
    {
        if (index >= cached.size())
            return;

        current_index = index;

        auto &c = cached[index];

        simulator->data.collection = c.read_file();
        simulator->data.notify_that_data_changed();
        simulator->set_timepoint(c.timepoint);
        simulator->initialize();
    }

    std::filesystem::path TimelineService::get_cache_folder()
    {
        auto path = std::filesystem::path(FluidStudio::DirectoryHelper::studio_directory()) / "cache";
        std::filesystem::create_directories(path);
        return path;
    }

    void TimelineService::clear_cache_folder()
    {
        auto path = get_cache_folder();
        std::vector<std::filesystem::path> to_delete;
        for (auto &p : std::filesystem::recursive_directory_iterator(path))
        {
            if (p.path().extension() == ".data" && p.is_regular_file())
            {
                to_delete.push_back(p.path());
            }
        }

        for (auto p : to_delete)
        {
            std::filesystem::remove(p);
        }
    }

    size_t TimelineService::current() const
    {
        return current_index;
    }

    void TimelineService::override_timestep_result()
    {
        // create and save the cached file
        CachedFile c;
        c.timepoint = simulator->get_current_timepoint();
        c.path = get_cache_folder() / (std::to_string(c.timepoint.timestep_number) + ".data");
        c.create_file(simulator->data.collection);

        if (cached.size() > 0)
        {
            // check if we are stepped back
            if (current_index != cached.size() - 1)
            {
                // we are stepped back, delete everything in the future (after the current timestep)
                cached.resize(current_index + 1);
            }

            // update the current timestep
            cached[current_index] = c;
        }
        else
        {
            // save the timestep, since there are no other timesteps
            current_index = 0;
            cached.push_back(c);
        }
    }

} // namespace FluidStudio
