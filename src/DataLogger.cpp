//
// Created by corbi on 28.05.2019.
//

#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include "DataLogger.hpp"

DataLogger::DataLogger(const std::string &fileName) : fileName(fileName) {}

void DataLogger::StartLogging() {
    // open file
    myFile.open(fileName);

    std::string header = "Time(s);Average Density;Relative Energy;Kinetic Energy;Potential Energy;Maximal Velocity;CFL Number;Dead Particles";

    if (dynamic_cast<FluidSolver::SPHFluidSolver *>(FluidSolver) != nullptr) {
        auto solver = dynamic_cast<FluidSolver::SPHFluidSolver *>(FluidSolver);
        // nothing to do
    } else if (dynamic_cast<FluidSolver::IISPHFluidSolver *>(FluidSolver) != nullptr) {
        auto solver = dynamic_cast<FluidSolver::IISPHFluidSolver *>(FluidSolver);
        header.append(";SolverIterations;PredictedDensityError");
    }

    // write header
    myFile << header << std::endl;

    // reset data
    currentTime = 0;
    StatisticCollector->CalculateData();
    startEnergy = StatisticCollector->getCalculatedEnergy();

    // initial log
    calculateAndLogData();
}

void DataLogger::TimeStepPassed() {
    currentTime += Timestep;

    calculateAndLogData();
}

void DataLogger::FinishLogging() {
    // close file
    myFile.close();
}


void DataLogger::calculateAndLogData() {

    // get values
    float currentKineticEnergy = StatisticCollector->getCalculatedKineticEnergy();
    float currentPotentialEnergy = StatisticCollector->getCalculatedPotentialEnergy();
    float averageDensity = StatisticCollector->getCalculatedAverageDensity();
    float totalEnergy = StatisticCollector->getCalculatedEnergy();
    float maxVelocity = StatisticCollector->getCalculatedMaximumVelocity();
    float cflNumber = StatisticCollector->getCalculatedCflNumber();
    uint32_t deadParticles = StatisticCollector->getCalculatedDeadParticleCount();

    // calculate relative energy
    float relativeEnergy = totalEnergy - startEnergy;

    // create basic data
    std::string data = std::to_string(currentTime) + ";" + std::to_string(averageDensity) + ";"
                       + std::to_string(relativeEnergy) + ";"
                       + std::to_string(currentKineticEnergy) + ";" + std::to_string(currentPotentialEnergy) + ";"
                       + std::to_string(maxVelocity) + ";" + std::to_string(cflNumber) + ";"
                       + std::to_string(deadParticles);

    // append data based on solver type
    if (dynamic_cast<FluidSolver::SPHFluidSolver *>(FluidSolver) != nullptr) {
        auto solver = dynamic_cast<FluidSolver::SPHFluidSolver *>(FluidSolver);
        // nothing to do
    } else if (dynamic_cast<FluidSolver::IISPHFluidSolver *>(FluidSolver) != nullptr) {
        auto solver = dynamic_cast<FluidSolver::IISPHFluidSolver *>(FluidSolver);
        data.append(";" + std::to_string(solver->getLastIterationCount()));
        data.append(";" + std::to_string(solver->getLastPredictedDensityError()));
    }

    // write data
    myFile << data << std::endl;


    if (alwaysFlush)
        myFile.flush();
}

FluidSolver::StatisticCollector *DataLogger::getStatisticCollector() const {
    return StatisticCollector;
}

void DataLogger::setStatisticCollector(FluidSolver::StatisticCollector *statisticCollector) {
    StatisticCollector = statisticCollector;
}

float DataLogger::getTimestep() const {
    return Timestep;
}

void DataLogger::setTimestep(float timestep) {
    Timestep = timestep;
}

FluidSolver::IFluidSolver *DataLogger::getFluidSolver() const {
    return FluidSolver;
}

void DataLogger::setFluidSolver(FluidSolver::IFluidSolver *fluidSolver) {
    FluidSolver = fluidSolver;
}

