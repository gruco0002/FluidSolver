
#include "IParticleCollection.hpp"

namespace FluidSolver {
    bool FluidParticle::operator==(const FluidParticle &rhs) const {
        return Position == rhs.Position &&
               Velocity == rhs.Velocity &&
               Acceleration == rhs.Acceleration &&
               Mass == rhs.Mass &&
               Pressure == rhs.Pressure &&
               Density == rhs.Density &&
               Type == rhs.Type &&
               PredictedVelocity == rhs.PredictedVelocity &&
               NonPressureAcceleration == rhs.NonPressureAcceleration &&
               SourceTerm == rhs.SourceTerm &&
               DiagonalElement == rhs.DiagonalElement;
    }

    bool FluidParticle::operator!=(const FluidParticle &rhs) const {
        return !(rhs == *this);
    }
}