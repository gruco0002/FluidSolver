//
// Created by corbi on 20.11.2019.
//

#ifndef FLUIDSOLVER_SIMULATION_HPP
#define FLUIDSOLVER_SIMULATION_HPP

#include <DataLogger.hpp>
#include <core/interface/IExternalForce.hpp>
#include <core/interface/ISimulationVisualizer.hpp>
#include <core/interface/IFluidSolver.hpp>
#include "SPHFluidSolver.hpp"

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

        float particleSize = 1.0f;
        float restDensity = 1.0f;
        float timestep = 0.001f;


    public:

        void ExecuteSimulationStep();

        void VisualizeSimulation();

        void CollectStatistics();

    };

}

#endif //FLUIDSOLVER_SIMULATION_HPP
