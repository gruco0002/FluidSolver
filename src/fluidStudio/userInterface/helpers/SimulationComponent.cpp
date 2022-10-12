#include "SimulationComponent.hpp"


namespace FluidStudio {
    bool SimulationComponent::operator==(const SimulationComponent& other) const {
        return kind == other.kind && index == other.index;
    }
    bool SimulationComponent::operator!=(const SimulationComponent& other) const {
        return !(*this == other);
    }
    bool SimulationComponent::can_delete() const {
        switch (kind) {
            case Kind::Sensor:
            case Kind::Entity:
                return true;
        }

        return false;
    }


} // namespace FluidStudio