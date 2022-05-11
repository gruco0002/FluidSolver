#pragma once

namespace FluidSolver
{
    class SimulatorVisualizerBundle;
    class Simulation;
    class ITimestepGenerator;
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
        class CompressedNeighborStorageSensor;

    } // namespace Sensors
    class SensorWriter;
    class OutputManager;

    class Compatibility;
    struct CompatibilityIssue;


} // namespace FluidSolver