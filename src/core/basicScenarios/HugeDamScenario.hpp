//
// Created by corbi on 28.05.2019.
//

#ifndef FLUIDSOLVER_HUGEDAMSCENARIO_HPP
#define FLUIDSOLVER_HUGEDAMSCENARIO_HPP

#include "Scenario.hpp"

namespace FluidSolver{



class HugeDamScenario : public Scenario {

public:
    void ResetData(IParticleCollection *particleCollection, float restDensity) override;

    int GetParticleCountX() override;

    int GetParticleCountY() override;

};
}

#endif //FLUIDSOLVER_HUGEDAMSCENARIO_HPP
