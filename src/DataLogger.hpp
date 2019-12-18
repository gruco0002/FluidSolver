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

    FluidSolver::IFluidSolver *getFluidSolver() const;

    void setFluidSolver(FluidSolver::IFluidSolver *fluidSolver);

    float getTimestep() const;

    void setTimestep(float timestep);

    FluidSolver::StatisticCollector *getStatisticCollector() const;

    void setStatisticCollector(FluidSolver::StatisticCollector *statisticCollector);

private:

    FluidSolver::StatisticCollector *StatisticCollector = nullptr;

    float Timestep = 0.0f;

    FluidSolver::IFluidSolver *FluidSolver = nullptr;

    std::ofstream myFile;

    float startEnergy = 0.0f;

    float currentTime = 0.0f;

    void calculateAndLogData();

};


#endif //FLUIDSOLVER_DATALOGGER_HPP
