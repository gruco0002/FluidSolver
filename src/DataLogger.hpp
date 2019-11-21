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

    DataLogger(FluidSolver::SPHFluidSolver *fluidSolver, const std::string &fileName);

    FluidSolver::SPHFluidSolver *fluidSolver = nullptr;

    std::string fileName = "data.csv";

    bool alwaysFlush = true;

    void StartLogging();

    void TimeStepPassed();

    void FinishLogging();

    float zeroHeight = -500.0f;

private:

    std::ofstream myFile;

    float startEnergy;

    float currentTime;

    void log(float time, float avgDensity, float energy, float kineticEnergy, float potentialEnergy, float maxVelocity,  float cflNumber, uint32_t deadParticleCount);

    void calculateAndLogData();


};


#endif //FLUIDSOLVER_DATALOGGER_HPP
