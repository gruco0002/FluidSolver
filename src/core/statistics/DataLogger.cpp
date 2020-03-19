#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include "DataLogger.hpp"

DataLogger::DataLogger(const std::string &fileName) : fileName(fileName) {}

void DataLogger::StartLogging() {
    if (logFinished)
        return;

    if (fileOpened)
        return;

    // open file
    myFile.open(fileName);
    fileOpened = true;

    std::string header = "Time(s)";

    for (auto val: StatisticCollector->getStats()) {
        header.append(";" + val->Name);
    }


    // write header
    myFile << header << std::endl;

    // reset data
    currentTime = 0;
    StatisticCollector->CalculateData();

    // initial log
    calculateAndLogData();
}

void DataLogger::TimeStepPassed() {
    if (logFinished)
        return;
    if (!fileOpened)
        return;

    currentTime += Timestep;

    calculateAndLogData();

    if (currentTime >= MaxLogLengthInSimulationSeconds) {
        FinishLogging();
    }
}

void DataLogger::FinishLogging() {
    if (logFinished)
        return;
    if (!fileOpened)
        return;

    // close file
    myFile.close();
    fileOpened = false;
    logFinished = true;
}


void DataLogger::calculateAndLogData() {

    // get values of statistics collector
    std::string data = std::to_string(currentTime);

    for (auto val: StatisticCollector->getStats()) {
        data.append(";" + val->ToString());
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

void DataLogger::ResetLogger() {
    FinishLogging();
    logFinished = false;
}

bool DataLogger::isLogFinished() const {
    return logFinished;
}

bool DataLogger::hasStarted() {
    return fileOpened;
}

