#include "SimulationComponent.hpp"

#include "entities/ParticleRemover3D.hpp"

namespace FluidStudio {
    bool Component::operator==(const Component& other) const {
        return kind == other.kind && index == other.index;
    }
    bool Component::operator!=(const Component& other) const {
        return !(*this == other);
    }
    bool Component::can_delete() const {
        switch (kind) {
            case Kind::Sensor:
            case Kind::Entity:
                return true;
        }

        return false;
    }
    const char* Component::get_entity_type_name(const std::shared_ptr<LibFluid::IEntity> entity) {
        if (std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(entity)) {
            return "Particle Remover 3D";
        }
        return "";
    }

} // namespace FluidStudio