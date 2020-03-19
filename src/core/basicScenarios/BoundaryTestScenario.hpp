#ifndef FLUIDSOLVER_BOUNDARYTESTSCENARIO_HPP
#define FLUIDSOLVER_BOUNDARYTESTSCENARIO_HPP

#include <string>
#include "Scenario.hpp"

namespace FluidSolver {

    class BoundaryTestScenario : public Scenario {
    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        std::string GetName() override;

        int GetParticleCountY() override;
    };

}

#endif //FLUIDSOLVER_BOUNDARYTESTSCENARIO_HPP
