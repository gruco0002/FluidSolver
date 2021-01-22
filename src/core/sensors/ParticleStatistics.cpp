#include "ParticleStatistics.hpp"
#include "core/FluidAssert.hpp"
#include <algorithm>

#include "core/Simulation.hpp"

void FluidSolver::Sensors::GlobalDensitySensor::initialize()
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
}

void FluidSolver::Sensors::GlobalDensitySensor::calculate_and_store(const Timepoint& timepoint)
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
	FLUID_ASSERT(parameters.simulation_parameters->collection != nullptr);
	auto collection = parameters.simulation_parameters->collection;
	FLUID_ASSERT(collection->is_type_present<ParticleData>());
	FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

	// calculate data;
	MMAData d;

	size_t counter = 0;
	for (size_t i = 0; i < collection->size(); i++) {

		const ParticleData& pData = collection->get<ParticleData>(i);
		const ParticleInfo& iData = collection->get<ParticleInfo>(i);

		if (iData.type == ParticleType::ParticleTypeNormal) {
			d.average += pData.density;
			d.minimum = std::min(d.minimum, pData.density);
			d.maximum = std::max(d.maximum, pData.density);
			counter++;
		}
	}

	if (counter > 0) {
		d.average = d.average / counter;
	}

	// save the calculated results
	data.push_back(timepoint, d);

}

void FluidSolver::Sensors::GlobalPressureSensor::initialize()
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
}

void FluidSolver::Sensors::GlobalPressureSensor::calculate_and_store(const Timepoint& timepoint)
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
	FLUID_ASSERT(parameters.simulation_parameters->collection != nullptr);
	auto collection = parameters.simulation_parameters->collection;
	FLUID_ASSERT(collection->is_type_present<ParticleData>());
	FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

	// calculate data;
	MMAData d;

	size_t counter = 0;
	for (size_t i = 0; i < collection->size(); i++) {

		const ParticleData& pData = collection->get<ParticleData>(i);
		const ParticleInfo& iData = collection->get<ParticleInfo>(i);

		if (iData.type == ParticleType::ParticleTypeNormal) {
			d.average += pData.pressure;
			d.minimum = std::min(d.minimum, pData.pressure);
			d.maximum = std::max(d.maximum, pData.pressure);
			counter++;
		}
	}

	if (counter > 0) {
		d.average = d.average / counter;
	}

	// save the calculated results
	data.push_back(timepoint, d);
}

void FluidSolver::Sensors::GlobalVelocitySensor::initialize()
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
}

void FluidSolver::Sensors::GlobalVelocitySensor::calculate_and_store(const Timepoint& timepoint)
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
	FLUID_ASSERT(parameters.simulation_parameters->collection != nullptr);
	auto collection = parameters.simulation_parameters->collection;
	FLUID_ASSERT(collection->is_type_present<MovementData>());
	FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

	// calculate data;
	MMAData d;

	size_t counter = 0;
	for (size_t i = 0; i < collection->size(); i++) {

		const MovementData& mData = collection->get<MovementData>(i);
		const ParticleInfo& iData = collection->get<ParticleInfo>(i);

		if (iData.type == ParticleType::ParticleTypeNormal) {
			float velocity = glm::length(mData.velocity);
			d.average += velocity;
			d.minimum = std::min(d.minimum, velocity);
			d.maximum = std::max(d.maximum, velocity);
			counter++;
		}
	}

	if (counter > 0) {
		d.average = d.average / counter;
	}

	// save the calculated results
	data.push_back(timepoint, d);
}

void FluidSolver::Sensors::GlobalEnergySensor::initialize()
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
}

void FluidSolver::Sensors::GlobalEnergySensor::calculate_and_store(const Timepoint& timepoint)
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
	FLUID_ASSERT(parameters.simulation_parameters->collection != nullptr);
	auto collection = parameters.simulation_parameters->collection;
	FLUID_ASSERT(collection->is_type_present<MovementData>());
	FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
	FLUID_ASSERT(collection->is_type_present<ParticleData>());

	// calculate data;
	EnergyData d;

	for (size_t i = 0; i < collection->size(); i++) {

		const MovementData& mData = collection->get<MovementData>(i);
		const ParticleInfo& iData = collection->get<ParticleInfo>(i);
		const ParticleData& pData = collection->get<ParticleData>(i);

		if (iData.type == ParticleType::ParticleTypeNormal) {
			float velocity = glm::length(mData.velocity);
			float relative_height = mData.position.y - settings.relative_zero_height;

			d.kinetic += 0.5f * pData.mass * velocity * velocity;
			d.potential += parameters.simulation_parameters->gravity * pData.mass * relative_height;
		}
	}

	// save the calculated results
	data.push_back(timepoint, d);
}


void FluidSolver::Sensors::GlobalParticleCountSensor::initialize()
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
}

void FluidSolver::Sensors::GlobalParticleCountSensor::calculate_and_store(const Timepoint& timepoint)
{
	FLUID_ASSERT(parameters.simulation_parameters != nullptr);
	FLUID_ASSERT(parameters.simulation_parameters->collection != nullptr);
	auto collection = parameters.simulation_parameters->collection;
	FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

	// calculate data;
	ParticleCountData d;

	for (size_t i = 0; i < collection->size(); i++) {
		const ParticleInfo& iData = collection->get<ParticleInfo>(i);

		if (iData.type == ParticleType::ParticleTypeNormal) {
			d.normal_particles++;
		}
		else if (iData.type == ParticleType::ParticleTypeBoundary) {
			d.boundary_particles++;
		}
		else if (iData.type == ParticleType::ParticleTypeDead) {
			d.inactive_particles++;
		}
	}

	// save the calculated results
	data.push_back(timepoint, d);
}
