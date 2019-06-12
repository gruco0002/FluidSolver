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
    myFile
            << "Time(s);Average Density;Relative Energy;Kinetic Energy;Potential Energy;Maximal Velocity;CFL Number; Dead Particles"
            << std::endl;

    // reset data
    currentTime = 0;
    startEnergy = fluidSolver->statisticCollector->CalculateEnergy();

    // initial log
    calculateAndLogData();
}

void DataLogger::TimeStepPassed() {
    currentTime += fluidSolver->TimeStep;

    calculateAndLogData();
}

void DataLogger::FinishLogging() {
    // close file
    myFile.close();
}

void DataLogger::log(float time, float avgDensity, float energy, float kineticEnergy, float potentialEnergy,
                     float maxVelocity, float cflNumber, uint32_t deadParticleCount) {
    myFile << std::to_string(time) << ";" << std::to_string(avgDensity) << ";" << std::to_string(energy) << ";"
           << std::to_string(kineticEnergy) << ";" << std::to_string(potentialEnergy) << ";"
           << std::to_string(maxVelocity) << ";" << std::to_string(cflNumber) << ";" << std::to_string(deadParticleCount)
           << std::endl;
    if (alwaysFlush)
        myFile.flush();
}

void DataLogger::calculateAndLogData() {

    float currentKineticEnergy = fluidSolver->statisticCollector->CalculateKineticEnergy();
    float currentPotentialEnergy = fluidSolver->statisticCollector->CalculatePotentialEnergy();
    float averageDensity = fluidSolver->statisticCollector->CalculateAverageDensity();
    float totalEnergy = fluidSolver->statisticCollector->CalculateEnergy(currentKineticEnergy, currentPotentialEnergy);
    float maxVelocity = fluidSolver->statisticCollector->CalculateMaximumVelocity();
    float cflNumber = fluidSolver->statisticCollector->GetCFLNumber(maxVelocity);
    uint32_t deadParticles = fluidSolver->statisticCollector->GetDeadParticleCount();

    log(currentTime, averageDensity, totalEnergy - startEnergy, currentKineticEnergy, currentPotentialEnergy,
        maxVelocity, cflNumber, deadParticles);
}

