#ifndef FLUIDSOLVER_SCENARIO_HPP
#define FLUIDSOLVER_SCENARIO_HPP

#include "ScenarioData.hpp"
#include "ScriptInterface.hpp"

namespace FluidSolver {
    class Scenario {
    private:

        ScriptInterface interface;

        void setup();

    public:

        ScenarioData data;

        const std::string filepath;

        explicit Scenario(const std::string &filepath);

        Scenario(const Scenario &) = delete;
    };

}


#endif //FLUIDSOLVER_SCENARIO_HPP
