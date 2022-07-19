#pragma once

namespace FluidSolver
{
    class SimulatorVisualizerBundle;
    class Simulator;
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
        class IISPHSensor;

    } // namespace Sensors
    class SensorWriter;
    class OutputManager;

    class CompatibilityReport;

    // other stuff
    struct DataChangeStruct;
    class Initializable;
    class Reportable;

} // namespace FluidSolver