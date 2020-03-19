//
// Created by corbi on 06.06.2019.
//

#include "DeathBox.hpp"

void FluidSolver::DeathBox::ModifySimulation(FluidSolver::IParticleCollection *collection, SimulationInfo &info) {

    #pragma omp parallel for
    for (int64_t  i = 0; i < collection->GetSize(); i++) {
        auto type = collection->GetParticleType(i);
        if (type == ParticleTypeBoundary) {
            continue; // don't calculate unnecessary values for the boundary particles.
        }
        if (type == ParticleTypeDead) {
            continue; // don*t calculate unnecessary values for dead particles.
        }

        glm::vec2 position = collection->GetPosition(i);

        if (position.y <= top && position.y >= bottom && position.x >= left && position.x <= right) {
            // inside
            if (!outside) {
                collection->SetParticleType(i, ParticleTypeDead);
            }
        } else {
            // outside
            if (outside) {
                collection->SetParticleType(i, ParticleTypeDead);
            }
        }
    }

}

FluidSolver::DeathBox::DeathBox(float top, float left, float bottom, float right) : top(top), left(left),
                                                                                    bottom(bottom), right(right) {}
