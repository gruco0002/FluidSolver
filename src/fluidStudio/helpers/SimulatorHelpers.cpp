#include "SimulatorHelpers.hpp"

#include "Simulator.hpp"
#include "fluidSolver/Particle.hpp"
#include "fluidSolver/ParticleCollection.hpp"

namespace FluidStudio::SimulatorHelpers
{

    bool is_3d_simulation(const std::shared_ptr<LibFluid::Simulator> &simulator)
    {
        if (simulator->data.collection->is_type_present<LibFluid::MovementData3D>())
        {
            return true;
        }
        return false;
    }
} // namespace FluidStudio::SimulatorHelpers
