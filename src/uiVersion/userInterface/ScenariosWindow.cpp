#include "ScenariosWindow.hpp"
#include "core/FluidAssert.hpp"
#include "../FluidSolverWindow.hpp"


void ScenariosWindow::render() {

}

void ScenariosWindow::initialize() {

}

void ScenariosWindow::update_path() {

}

void ScenariosWindow::reload() {

}

ScenariosWindow::~ScenariosWindow() {
for(auto ptr: scenarios)
    delete[] ptr;
}
