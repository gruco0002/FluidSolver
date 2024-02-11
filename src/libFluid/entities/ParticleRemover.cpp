#include "ParticleRemover.hpp"

#include "group/AreaGroup.hpp"
#include "group/GroupActions.hpp"

namespace LibFluid
{
    void ParticleRemover::initialize()
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
    void ParticleRemover::create_compatibility_report(CompatibilityReport &report)
    {
    }
    void ParticleRemover::execute_simulation_step(const Timepoint &timepoint, bool before_solver)
    {
        initialize();

        AreaGroup group;
        group.area = parameters.area;
        group.collection = simulation_data.collection;
        group.inside = !parameters.remove_if_outside;
        GroupActions::remove(group);
    }

} // namespace LibFluid
