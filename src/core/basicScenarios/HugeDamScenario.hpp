#ifndef FLUIDSOLVER_HUGEDAMSCENARIO_HPP
#define FLUIDSOLVER_HUGEDAMSCENARIO_HPP

#include <string>
#include "Scenario.hpp"

namespace FluidSolver {


    class HugeDamScenario : public Scenario {

    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

        std::string GetName() override;

    };
}

#endif //FLUIDSOLVER_HUGEDAMSCENARIO_HPP
