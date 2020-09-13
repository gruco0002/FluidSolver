#ifndef FLUIDSOLVER_SCENARIO_HPP
#define FLUIDSOLVER_SCENARIO_HPP

#include "chaiscript/chaiscript.hpp"
#include "ScenarioData.hpp"
#include "ScriptInterface.hpp"

namespace FluidSolver {
    class Scenario {
     private:

        chaiscript::ChaiScript chai;

        void setup();

    public:

        ScriptInterface interface;

        const std::string filepath;

        explicit Scenario(const std::string &filepath);

        ScenarioData* create_data();

        Scenario(const Scenario&) = delete;
    };

}


#endif //FLUIDSOLVER_SCENARIO_HPP
