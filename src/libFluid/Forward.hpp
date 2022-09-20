#pragma once

namespace LibFluid {
    class SimulatorVisualizerBundle;
    class Simulator;
    class ITimestepGenerator;
    class ISimulationVisualizer;
    class ParticleCollection;
    class IFluidSolverBase;
    class NeighborhoodInterface;
    struct Timepoint;

    // entities
    class SimulationEntity;
    class ParticleSpawner;
    class ParticleRemover;
    class ParticleRemover3D;

    // sensors
    class Sensor;

    template <typename T>
    class SensorBase;

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