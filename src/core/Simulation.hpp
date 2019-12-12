//
// Created by corbi on 20.11.2019.
//

#ifndef FLUIDSOLVER_SIMULATION_HPP
#define FLUIDSOLVER_SIMULATION_HPP

#include <DataLogger.hpp>
#include <core/interface/IExternalForce.hpp>
#include <core/interface/ISimulationVisualizer.hpp>
#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/timestep/ITimestep.hpp>
#include "core/fluidSolver/SPHFluidSolver.hpp"

namespace FluidSolver {
    class Simulation {

    private:
        IParticleCollection *particleCollection = nullptr;

        IFluidSolver *fluidSolver = nullptr;

        StatisticCollector *statisticCollector = nullptr;

        DataLogger *dataLogger = nullptr;

        std::vector<ISimulationModifier *> simulationModifiers;

        std::vector<IExternalForce *> externalForces;

        ISimulationVisualizer *simulationVisualizer = nullptr;

        ITimestep *timestep = nullptr;

        float particleSize = 1.0f;
        float restDensity = 1.0f;
        float gravity = 9.81f;
    public:
        float getGravity() const;

        void setGravity(float gravity);


    public:
        IParticleCollection *getParticleCollection();

        void setParticleCollection(IParticleCollection *particleCollection);

        IFluidSolver *getFluidSolver();

        void setFluidSolver(IFluidSolver *fluidSolver);

        StatisticCollector *getStatisticCollector();

        void setStatisticCollector(StatisticCollector *statisticCollector);

        DataLogger *getDataLogger();

        void setDataLogger(DataLogger *dataLogger);

        ISimulationVisualizer *getSimulationVisualizer();

        void setSimulationVisualizer(ISimulationVisualizer *simulationVisualizer);

        float getParticleSize();

        void setParticleSize(float particleSize);

        float getRestDensity();

        void setRestDensity(float restDensity);

        ITimestep* getTimestep();

        void setTimestep(ITimestep* timestep);

        void addSimulationModifier(ISimulationModifier *modifier);

        void removeSimulationModifier(ISimulationModifier *modifier);

        void clearSimulationModifiers();

        void clearExternalForces();

        void addExternalForce(IExternalForce *force);

        void removeExternalForce(IExternalForce *force);


    public:

        void ExecuteSimulationStep();

        void VisualizeSimulation();

        void CollectStatistics();

    };

}

#endif //FLUIDSOLVER_SIMULATION_HPP
