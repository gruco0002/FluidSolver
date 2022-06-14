#include "TimelineService.hpp"

#include "Simulator.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "DirectoryHelper.hpp"
#include "serialization/ParticleSerializer.hpp"

namespace FluidUi {
    void TimelineService::CachedFile::delete_file() {
        if(std::filesystem::exists(path)){
            std::filesystem::remove(path);
        }
    }
    void TimelineService::CachedFile::create_file(std::shared_ptr<FluidSolver::ParticleCollection> particle_collection) {
        FluidSolver::ParticleSerializer serializer(path);
        serializer.serialize(*particle_collection);
    }
    std::shared_ptr<FluidSolver::ParticleCollection> TimelineService::CachedFile::read_file() {
        auto res = std::make_shared<FluidSolver::ParticleCollection>();
        FluidSolver::ParticleSerializer deserializer(path);
        deserializer.deserialize(*res);

        return res;
    }

    void TimelineService::reset() {
        clear_cache_folder();
        cached.clear();
        current_index = 0;
    }
    void TimelineService::save_timestep_result() {
        CachedFile c;
        c.timepoint = simulator->get_current_timepoint();
        c.path = get_cache_folder() / (std::to_string(c.timepoint.timestep_number) + ".data");
        c.create_file(simulator->data.collection);

        if(cached.size() > 0) {
            if (current_index != cached.size() - 1) {
                // we are stepped back, delete everything in the future
                cached.resize(current_index + 1);
            }

            current_index++;
        }else{
            current_index = 0;
        }
        cached.push_back(c);

    }
    size_t TimelineService::size() const {
        return cached.size();
    }
    void TimelineService::step_to(size_t index) {
        if(index >= cached.size())
            return;

        current_index = index;

        auto& c = cached[index];

        simulator->data.collection = c.read_file();
        simulator->data.notify_that_data_changed();
        simulator->set_timepoint(c.timepoint);
        simulator->initialize();


    }
    std::filesystem::path TimelineService::get_cache_folder() {
        auto path = std::filesystem::path(FluidStudio::DirectoryHelper::studio_directory()) / "cache";
        std::filesystem::create_directories(path);
        return path;
    }
    void TimelineService::clear_cache_folder() {
        auto path = get_cache_folder();
        std::vector<std::filesystem::path> to_delete;
        for (auto &p : std::filesystem::recursive_directory_iterator(path))
        {
            if (p.path().extension() == ".data" && p.is_regular_file()){
                to_delete.push_back(p.path());
            }
        }

        for(auto p : to_delete){
            std::filesystem::remove(p);
        }

    }
    size_t TimelineService::current() const {
        return current_index;
    }
} // namespace FluidUi
