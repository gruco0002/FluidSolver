#ifndef FLUIDSOLVER_SIMULATION_HPP
#define FLUIDSOLVER_SIMULATION_HPP

//#include <core/statistics/DataLogger.hpp>
//#include <core/interface/IExternalForce.hpp>
#include <core/visualizer/ISimulationVisualizer.hpp>
#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include <core/timestep/ITimestep.hpp>
//#include <core/fluidSolver/kernel/IKernel.hpp>
//#include <core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp>

namespace FluidSolver {
    class Simulation {

    private:
        ParticleCollection *particleCollection = nullptr;

        IISPHFluidSolver *fluidSolver = nullptr;

        //StatisticCollector *statisticCollector = nullptr;

        //DataLogger *dataLogger = nullptr;

        //std::vector<ISimulationModifier *> simulationModifiers;

        //std::vector<IExternalForce *> externalForces;

        ISimulationVisualizer *simulationVisualizer = nullptr;

        ITimestep *timestep = nullptr;

        //IParticleSelection* particleSelection = new AllParticleSelection();

        //IKernel* kernel = nullptr;

        //INeighborhoodSearch* neighborhoodSearch = nullptr;

        float particleSize = 1.0f;
        float restDensity = 1.0f;
        float gravity = 9.81f;
    public:
        //IParticleSelection *getParticleSelection() const;

        //void setParticleSelection(IParticleSelection *particleSelection);

    public:
        float getGravity() const;

        void setGravity(float gravity);


    public:
        ParticleCollection *getParticleCollection();

        void setParticleCollection(ParticleCollection *particleCollection);

        IISPHFluidSolver *getFluidSolver();

        void setFluidSolver(IISPHFluidSolver *fluidSolver);

        //StatisticCollector *getStatisticCollector();

        //void setStatisticCollector(StatisticCollector *statisticCollector);

        //DataLogger *getDataLogger();

        //void setDataLogger(DataLogger *dataLogger);

        ISimulationVisualizer *getSimulationVisualizer();

        void setSimulationVisualizer(ISimulationVisualizer *simulationVisualizer);

        float getParticleSize();

        void setParticleSize(float particleSize);

        float getRestDensity();

        void setRestDensity(float restDensity);

        ITimestep *getTimestep();

        void setTimestep(ITimestep *timestep);

        //void addSimulationModifier(ISimulationModifier *modifier);

        //void removeSimulationModifier(ISimulationModifier *modifier);

        //void clearSimulationModifiers();

        //void clearExternalForces();

        //void addExternalForce(IExternalForce *force);

        //void removeExternalForce(IExternalForce *force);

        //void setNeighborhoodSearch(INeighborhoodSearch* neighborhoodSearch);

        //INeighborhoodSearch* getNeighborhoodSearch();

        //void setKernel(IKernel* kernel);

        //IKernel* getKernel();

        float getRadius();

    public:

        void ExecuteSimulationStep();

        void VisualizeSimulation();

        void CollectStatistics();

    };

}

#endif //FLUIDSOLVER_SIMULATION_HPP
