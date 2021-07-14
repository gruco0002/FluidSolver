#pragma once

namespace FluidSolver
{
    class Simulation;
    class ITimestep;
    class ISimulationVisualizer;
    class ParticleCollection;
    class IFluidSolverBase;

    // entities
    class IEntity;
    class ParticleSpawner;
    class ParticleRemover;

    // sensors
    class ISensor;
    namespace Sensors
    {
        class GlobalDensitySensor;
        class GlobalPressureSensor;
        class GlobalVelocitySensor;
        class GlobalEnergySensor;
        class GlobalParticleCountSensor;
        class SensorPlane;

    } // namespace Sensors
    class SensorWriter;

    class Compatibility;
    struct CompatibilityIssue;


} // namespace FluidSolver