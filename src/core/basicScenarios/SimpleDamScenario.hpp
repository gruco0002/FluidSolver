//
// Created by corbi on 28.05.2019.
//

#ifndef FLUIDSOLVER_SIMPLEDAMSCENARIO_HPP
#define FLUIDSOLVER_SIMPLEDAMSCENARIO_HPP

#include <string>
#include "Scenario.hpp"

namespace FluidSolver {


    class SimpleDamScenario : public Scenario {
    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

        std::string GetName() override;

    };

}

#endif //FLUIDSOLVER_SIMPLEDAMSCENARIO_HPP
