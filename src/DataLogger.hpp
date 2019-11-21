//
// Created by corbi on 28.05.2019.
//

#ifndef FLUIDSOLVER_DATALOGGER_HPP
#define FLUIDSOLVER_DATALOGGER_HPP

#include <string>
#include <iostream>
#include <core/fluidSolver/SPHFluidSolver.hpp>
#include <fstream>

class DataLogger {


public:

    explicit DataLogger(const std::string &fileName);


    std::string fileName = "data.csv";

    bool alwaysFlush = true;

    void StartLogging();

    void TimeStepPassed();

    void FinishLogging();

    float zeroHeight = -500.0f;

private:

    FluidSolver::StatisticCollector* StatisticCollector = nullptr;
    float Timestep = 0.0f;
public:
    float getTimestep() const;

    void setTimestep(float timestep);

public:
    FluidSolver::StatisticCollector *getStatisticCollector() const;

    void setStatisticCollector(FluidSolver::StatisticCollector *statisticCollector);

private:

    std::ofstream myFile;

    float startEnergy;

    float currentTime;

    void log(float time, float avgDensity, float energy, float kineticEnergy, float potentialEnergy, float maxVelocity,  float cflNumber, uint32_t deadParticleCount);

    void calculateAndLogData();


};


#endif //FLUIDSOLVER_DATALOGGER_HPP
