#include "SimulationComponent.hpp"

#include "entities/BoundaryPreprocessor.hpp"
#include "entities/ParticleRemover3D.hpp"
#include "entities/VelocityAlterationByTag.hpp"

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
    const char* SimulationComponent::get_entity_type_name(const std::shared_ptr<LibFluid::SimulationEntity> entity) {
        if (std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(entity)) {
            return "Particle Remover 3D";
        }
        if (std::dynamic_pointer_cast<LibFluid::BoundaryPreprocessor>(entity)) {
            return "Boundary Preprocessor";
        }
        if (std::dynamic_pointer_cast<LibFluid::VelocityAlterationByTag>(entity)) {
            return "Velocity Alteration by Tag";
        }
        return "";
    }

} // namespace FluidStudio