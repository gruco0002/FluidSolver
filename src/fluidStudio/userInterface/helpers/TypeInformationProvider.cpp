#include "TypeInformationProvider.hpp"

#include "entities/BoundaryPreprocessor.hpp"
#include "entities/ParticleRemover3D.hpp"
#include "entities/VelocityAlterationByTag.hpp"

#include "sensors/CompressedNeighborsStatistics.hpp"
#include "sensors/IisphSensor.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorPlane.hpp"

namespace FluidStudio::TypeInformationProvider {

    const char* get_entity_type_name(const std::shared_ptr<LibFluid::SimulationEntity>& entity) {
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

    const char* get_entity_description(const std::shared_ptr<LibFluid::SimulationEntity>& entity) {
        if (std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(entity)) {
            return "This entity removes particles from the simulation if they are inside or outside a specified volume.";
        }
        if (std::dynamic_pointer_cast<LibFluid::BoundaryPreprocessor>(entity)) {
            return "This entity recalculates the mass of each boundary particle and corrects invalid sampling.";
        }
        if (std::dynamic_pointer_cast<LibFluid::VelocityAlterationByTag>(entity)) {
            return "This entity sets the velocity of particles with a certain tag to a specified value before each solver step.";
        }
        return "";
    }

    const char* get_sensor_type_name(const std::shared_ptr<LibFluid::Sensor>& sensor) {
        if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalDensitySensor>(sensor)) {
            return "Global Density";
        } else if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalPressureSensor>(sensor)) {
            return "Global Pressure";
        } else if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalVelocitySensor>(sensor)) {
            return "Global Velocity";
        } else if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalEnergySensor>(sensor)) {
            return "Global Energy";
        } else if (std::dynamic_pointer_cast<LibFluid::Sensors::GlobalParticleCountSensor>(sensor)) {
            return "Global Particle Count";
        } else if (std::dynamic_pointer_cast<LibFluid::Sensors::SensorPlane>(sensor)) {
            return "3D Sensor Plane";
        } else if (std::dynamic_pointer_cast<LibFluid::Sensors::CompressedNeighborStorageSensor>(sensor)) {
            return "Compressed Neighbor Storage";
        } else if (std::dynamic_pointer_cast<LibFluid::Sensors::IISPHSensor>(sensor)) {
            return "IISPH";
        }
        return "UNKNOWN";
    }

    const char* particle_type_to_string(LibFluid::ParticleType type) {
        switch (type) {
            case LibFluid::ParticleTypeNormal:
                return "Normal";
            case LibFluid::ParticleTypeBoundary:
                return "Boundary";
            case LibFluid::ParticleTypeInactive:
                return "Inactive";
            default:
                return "Unknown";
        }
    }
} // namespace FluidStudio::TypeInformationProvider