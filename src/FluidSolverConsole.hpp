//
// Created by corbi on 29.05.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP
#define FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP

#include "libraries/cxxopts/cxxopts.hpp"

class FluidSolverConsole {

public:
    explicit FluidSolverConsole(cxxopts::Options &options);

    void start(cxxopts::ParseResult &parseResult);

private:
    void setupOptions(cxxopts::Options &options);

};


#endif //FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP
