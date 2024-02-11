#include "ParticleRemover3D.hpp"

#include "group/GroupActions.hpp"
#include "group/VolumeGroup.hpp"

namespace LibFluid
{

    void ParticleRemover3D::execute_simulation_step(const Timepoint &timepoint, bool before_solver)
    {
        initialize();

        VolumeGroup group;
        group.volume = parameters.volume;
        group.collection = simulation_data.collection;
        group.inside = !parameters.remove_if_outside;
        GroupActions::remove(group);
    }
    void ParticleRemover3D::initialize()
    {
        if (settings.has_data_changed())
        {
            settings.acknowledge_data_change();
        }
        if (simulation_data.has_data_changed())
        {
            simulation_data.acknowledge_data_change();
        }
    }
    void ParticleRemover3D::create_compatibility_report(CompatibilityReport &report)
    {
    }
} // namespace LibFluid
