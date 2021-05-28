#ifndef FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP
#define FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP

#include <core/Simulation.hpp>
#include <cxxopts.hpp>

class FluidSolverConsole {

  public:
    explicit FluidSolverConsole(cxxopts::Options& options);

    void start(const cxxopts::ParseResult& parseResult);

    struct
    {
        float simulation_length = 1.0f;
        bool verbose = false;
        std::string filepath;
    } settings;

  private:
    void setup_options(cxxopts::Options& options);

    void read_settings(const cxxopts::ParseResult& parseResult);
};


#endif // FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP
