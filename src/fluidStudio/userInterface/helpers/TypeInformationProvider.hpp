#pragma once

#include "entities/SimulationEntity.hpp"
#include "sensors/Sensor.hpp"

namespace FluidStudio::TypeInformationProvider {

    const char* get_entity_type_name(const std::shared_ptr<LibFluid::SimulationEntity>& entity);

    const char* get_entity_description(const std::shared_ptr<LibFluid::SimulationEntity>& entity);

    const char* get_sensor_type_name(const std::shared_ptr<LibFluid::Sensor>& sensor);

    const char* particle_type_to_string(LibFluid::ParticleType type);


} // namespace FluidStudio::TypeInformationProvider