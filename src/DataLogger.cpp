//
// Created by corbi on 28.05.2019.
//

#include "DataLogger.hpp"

DataLogger::DataLogger(FluidSolver::SPHFluidSolver *fluidSolver, const std::string &fileName) : fluidSolver(
        fluidSolver), fileName(fileName) {}

void DataLogger::StartLogging() {
    // open file
    myFile.open(fileName);

    // write header
    myFile << "Time(s);Average Density;Relative Energy;Maximal Velocity" << std::endl;

    // reset data
    currentTime = 0;
    startEnergy = fluidSolver->particleCollection->CalculateEnergy(zeroHeight, fluidSolver->Gravity);

    // initial log
    log(currentTime, fluidSolver->particleCollection->CalculateAverageDensity(fluidSolver->RestDensity), 0.0f,
        fluidSolver->particleCollection->CalculateMaximumVelocity());
}

void DataLogger::TimeStepPassed() {
    currentTime += fluidSolver->TimeStep;

    log(currentTime, fluidSolver->particleCollection->CalculateAverageDensity(fluidSolver->RestDensity),
        fluidSolver->particleCollection->CalculateEnergy(zeroHeight, fluidSolver->Gravity) - startEnergy,
        fluidSolver->particleCollection->CalculateMaximumVelocity());
}

void DataLogger::FinishLogging() {
// close file
    myFile.close();
}

void DataLogger::log(float time, float avgDensity, float energy, float MaxVelocity) {
    myFile << std::to_string(time) << ";" << std::to_string(avgDensity) << ";" << std::to_string(energy) << ";"
           << std::to_string(MaxVelocity) << std::endl;
    if (alwaysFlush)
        myFile.flush();
}
