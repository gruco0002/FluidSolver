//
// Created by corbi on 28.05.2019.
//

#include "DataLogger.hpp"

DataLogger::DataLogger(const std::string &fileName) : fileName(fileName) {}

void DataLogger::StartLogging() {
    // open file
    myFile.open(fileName);

    // write header
    myFile
            << "Time(s);Average Density;Relative Energy;Kinetic Energy;Potential Energy;Maximal Velocity;CFL Number;Dead Particles"
            << std::endl;

    // reset data
    currentTime = 0;
    startEnergy = StatisticCollector->CalculateEnergy();

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

void DataLogger::log(float time, float avgDensity, float energy, float kineticEnergy, float potentialEnergy,
                     float maxVelocity, float cflNumber, uint32_t deadParticleCount) {
    myFile << std::to_string(time) << ";" << std::to_string(avgDensity) << ";" << std::to_string(energy) << ";"
           << std::to_string(kineticEnergy) << ";" << std::to_string(potentialEnergy) << ";"
           << std::to_string(maxVelocity) << ";" << std::to_string(cflNumber) << ";"
           << std::to_string(deadParticleCount)
           << std::endl;
    if (alwaysFlush)
        myFile.flush();
}

void DataLogger::calculateAndLogData() {

    float currentKineticEnergy = StatisticCollector->CalculateKineticEnergy();
    float currentPotentialEnergy = StatisticCollector->CalculatePotentialEnergy();
    float averageDensity = StatisticCollector->CalculateAverageDensity();
    float totalEnergy = StatisticCollector->CalculateEnergy(currentKineticEnergy, currentPotentialEnergy);
    float maxVelocity = StatisticCollector->CalculateMaximumVelocity();
    float cflNumber = StatisticCollector->GetCFLNumber(maxVelocity);
    uint32_t deadParticles = StatisticCollector->GetDeadParticleCount();

    log(currentTime, averageDensity, totalEnergy - startEnergy, currentKineticEnergy, currentPotentialEnergy,
        maxVelocity, cflNumber, deadParticles);
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

