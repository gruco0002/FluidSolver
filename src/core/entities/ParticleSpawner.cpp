#include "ParticleSpawner.hpp"


void FluidSolver::ParticleSpawner::initialize() {
	time_left_over = 0.0f;
}

void FluidSolver::ParticleSpawner::execute_simulation_step(FluidSolver::pFloat timestep) {
	FLUID_ASSERT(sim.collection != nullptr)
	FLUID_ASSERT(sim.collection->is_type_present<ParticleInfo>())
	FLUID_ASSERT(sim.collection->is_type_present<MovementData>())
	FLUID_ASSERT(sim.collection->is_type_present<ParticleData>())

	FLUID_ASSERT(sim.particle_size > 0.0f)

	FLUID_ASSERT(parameters.direction != vec2(0.0f))
	FLUID_ASSERT(parameters.width > 0.0f)
	FLUID_ASSERT(parameters.initial_velocity > 0.0f)

	// reset stuff
	this->last_index_checked = 0;
	
	// add time
	time_left_over += timestep;

	
	// assume that the particles move away by their velocity
	if (time_left_over * parameters.initial_velocity >= sim.particle_size) {
		// there should be enough place to spawn the particles
		
		// calculated required data 
		vec2 normalized_direction = glm::normalize(parameters.direction);
		vec2 initial_velocity = normalized_direction * parameters.initial_velocity;
		glm::vec2 orth_direction = glm::vec2(normalized_direction.y, -normalized_direction.x);
		float width = parameters.width;

		// spawn particles along width
		for (float x = -width / 2.0f; x <= width / 2.0f; x += sim.particle_size) {

			glm::vec2 pos = parameters.position + orth_direction * x + (time_left_over * parameters.initial_velocity - sim.particle_size) * normalized_direction;
			
			// check if the position is free (avoid spawning particles inside each other, as this would represent invalid states)
			if (!is_position_free(pos))
				continue;			
			
			auto index = get_or_add_particle();
			spawn_particle(index, pos, initial_velocity);
		}

		// we can set it to zero, since we already included the offset of a larger time_left_over in the position calculation
		time_left_over = 0.0f;
	}


}

FluidSolver::pIndex_t FluidSolver::ParticleSpawner::get_or_add_particle()
{
	while (last_index_checked < sim.collection->size()) {
		if (sim.collection->get<ParticleInfo>(last_index_checked).type == ParticleType::ParticleTypeDead) {
			// reuse the current particle
			return last_index_checked;
		}
		last_index_checked++;
	}

	// could not find a dead particle --> add a new one
	auto index = sim.collection->add();
	return index;
}

void FluidSolver::ParticleSpawner::spawn_particle(pIndex_t index, const glm::vec2& position, const glm::vec2& initial_velocity)
{
	FLUID_ASSERT(index < sim.collection->size());
	
	// set the particle type to normal
	sim.collection->get<ParticleInfo>(index).type = ParticleType::ParticleTypeNormal;;

	// set default particle data
	auto& pd = sim.collection->get<ParticleData>(index);
	pd.density = parameters.rest_density;
	pd.mass = parameters.mass;
	pd.pressure = 0.0f;

	// set movement data and position
	auto& md = sim.collection->get<MovementData>(index);
	md.position = position;
	md.acceleration = glm::vec2(0.0f);
	md.velocity = initial_velocity;

}

bool FluidSolver::ParticleSpawner::is_position_free(const glm::vec2& position)
{
	FLUID_ASSERT(sim.neighborhood_interface != nullptr);
	FLUID_ASSERT(sim.collection != nullptr);

	const float epsilon = 0.95f;

	float min_distance_squared = (sim.particle_size * epsilon) * (sim.particle_size * epsilon);	

	auto neighbors = sim.neighborhood_interface->get_neighbors(position);
	for (auto& neighbor : neighbors) {
		auto& neighbor_pos = sim.collection->get<MovementData>(neighbor).position;
		vec2 diff = neighbor_pos - position;
		float distance_squared = glm::dot(diff, diff);
		if (distance_squared < min_distance_squared) {
			return false;
		}
	}

	return true;
}
