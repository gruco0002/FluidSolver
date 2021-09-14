#include "ParticleStatistics.hpp"

#include "FluidAssert.hpp"
#include "Simulation.hpp"

#include <algorithm>

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
    for (size_t i = 0; i < collection->size(); i++)
    {

        const ParticleData& pData = collection->get<ParticleData>(i);
        const ParticleInfo& iData = collection->get<ParticleInfo>(i);

        if (iData.type == ParticleType::ParticleTypeNormal)
        {
            d.average += pData.density;
            d.minimum = std::min(d.minimum, pData.density);
            d.maximum = std::max(d.maximum, pData.density);
            counter++;
        }
    }

    if (counter > 0)
    {
        d.average = d.average / counter;
    }

    // save the calculated results
    data.push_back(timepoint, d);
}

void FluidSolver::Sensors::GlobalDensitySensor::save_data_to_file(SensorWriter& writer)
{

    if (writer.begin_header())
    {
        writer.push_back_header<Timepoint>("Timepoint");
        writer.push_back_header<float>("Density Avg");
        writer.push_back_header<float>("Density Min");
        writer.push_back_header<float>("Density Max");
        writer.end_header();
    }


    // data writing
    for (size_t i = saved_data_until; i < data.size(); i++)
    {
        auto& tmp = data.data()[i];
        writer << data.times()[i] << tmp.average << tmp.minimum << tmp.maximum << SensorWriter::Control::Next;
    }


    if (parameters.keep_data_in_memory_after_saving)
    {
        saved_data_until = data.size();
    }
    else
    {
        data.clear();
        saved_data_until = 0;
    }
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
    for (size_t i = 0; i < collection->size(); i++)
    {

        const ParticleData& pData = collection->get<ParticleData>(i);
        const ParticleInfo& iData = collection->get<ParticleInfo>(i);

        if (iData.type == ParticleType::ParticleTypeNormal)
        {
            d.average += pData.pressure;
            d.minimum = std::min(d.minimum, pData.pressure);
            d.maximum = std::max(d.maximum, pData.pressure);
            counter++;
        }
    }

    if (counter > 0)
    {
        d.average = d.average / counter;
    }

    // save the calculated results
    data.push_back(timepoint, d);
}

void FluidSolver::Sensors::GlobalPressureSensor::save_data_to_file(SensorWriter& writer)
{
    if (writer.begin_header())
    {
        writer.push_back_header<Timepoint>("Timepoint");
        writer.push_back_header<float>("Pressure Avg");
        writer.push_back_header<float>("Pressure Min");
        writer.push_back_header<float>("Pressure Max");
        writer.end_header();
    }


    // data writing
    for (size_t i = saved_data_until; i < data.size(); i++)
    {
        auto& tmp = data.data()[i];
        writer << data.times()[i] << tmp.average << tmp.minimum << tmp.maximum << SensorWriter::Control::Next;
    }


    if (parameters.keep_data_in_memory_after_saving)
    {
        saved_data_until = data.size();
    }
    else
    {
        data.clear();
        saved_data_until = 0;
    }
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
    for (size_t i = 0; i < collection->size(); i++)
    {

        const MovementData& mData = collection->get<MovementData>(i);
        const ParticleInfo& iData = collection->get<ParticleInfo>(i);

        if (iData.type == ParticleType::ParticleTypeNormal)
        {
            float velocity = glm::length(mData.velocity);
            d.average += velocity;
            d.minimum = std::min(d.minimum, velocity);
            d.maximum = std::max(d.maximum, velocity);
            counter++;
        }
    }

    if (counter > 0)
    {
        d.average = d.average / counter;
    }

    // save the calculated results
    data.push_back(timepoint, d);
}

void FluidSolver::Sensors::GlobalVelocitySensor::save_data_to_file(SensorWriter& writer)
{
    if (writer.begin_header())
    {
        writer.push_back_header<Timepoint>("Timepoint");
        writer.push_back_header<float>("Velocity Avg");
        writer.push_back_header<float>("Velocity Min");
        writer.push_back_header<float>("Velocity Max");
        writer.end_header();
    }


    // data writing
    for (size_t i = saved_data_until; i < data.size(); i++)
    {
        auto& tmp = data.data()[i];
        writer << data.times()[i] << tmp.average << tmp.minimum << tmp.maximum << SensorWriter::Control::Next;
    }


    if (parameters.keep_data_in_memory_after_saving)
    {
        saved_data_until = data.size();
    }
    else
    {
        data.clear();
        saved_data_until = 0;
    }
}

void FluidSolver::Sensors::GlobalEnergySensor::save_data_to_file(SensorWriter& writer)
{
    if (writer.begin_header())
    {
        writer.push_back_header<Timepoint>("Timepoint");
        writer.push_back_header<float>("Kinetic");
        writer.push_back_header<float>("Potential");
        writer.end_header();
    }


    // data writing
    for (size_t i = saved_data_until; i < data.size(); i++)
    {
        auto& tmp = data.data()[i];
        writer << data.times()[i] << tmp.kinetic << tmp.potential << SensorWriter::Control::Next;
    }


    if (parameters.keep_data_in_memory_after_saving)
    {
        saved_data_until = data.size();
    }
    else
    {
        data.clear();
        saved_data_until = 0;
    }
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

    for (size_t i = 0; i < collection->size(); i++)
    {

        const MovementData& mData = collection->get<MovementData>(i);
        const ParticleInfo& iData = collection->get<ParticleInfo>(i);
        const ParticleData& pData = collection->get<ParticleData>(i);

        if (iData.type == ParticleType::ParticleTypeNormal)
        {
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

    for (size_t i = 0; i < collection->size(); i++)
    {
        const ParticleInfo& iData = collection->get<ParticleInfo>(i);

        if (iData.type == ParticleType::ParticleTypeNormal)
        {
            d.normal_particles++;
        }
        else if (iData.type == ParticleType::ParticleTypeBoundary)
        {
            d.boundary_particles++;
        }
        else if (iData.type == ParticleType::ParticleTypeDead)
        {
            d.inactive_particles++;
        }
    }

    // save the calculated results
    data.push_back(timepoint, d);
}

void FluidSolver::Sensors::GlobalParticleCountSensor::save_data_to_file(SensorWriter& writer)
{
    if (writer.begin_header())
    {
        writer.push_back_header<Timepoint>("Timepoint");
        writer.push_back_header<size_t>("Normal");
        writer.push_back_header<size_t>("Boundary");
        writer.push_back_header<size_t>("Inactive");
        writer.end_header();
    }


    // data writing
    for (size_t i = saved_data_until; i < data.size(); i++)
    {
        auto& tmp = data.data()[i];
        writer << data.times()[i] << tmp.normal_particles << tmp.boundary_particles << tmp.inactive_particles
               << SensorWriter::Control::Next;
    }


    if (parameters.keep_data_in_memory_after_saving)
    {
        saved_data_until = data.size();
    }
    else
    {
        data.clear();
        saved_data_until = 0;
    }
}
