//
// Created by corbi on 08.01.2020.
//

#ifndef FLUIDSOLVER_PARTICLESELECTION_HPP
#define FLUIDSOLVER_PARTICLESELECTION_HPP

#include <cstdint>
#include <unordered_set>
#include "IParticleSelection.hpp"

namespace FluidSolver {
    class ParticleSelection : public IParticleSelection {
    public:
        bool IsParticleSelected(uint32_t particleIndex, IParticleCollection *particleCollection) override;

        void AddParticleToSelection(uint32_t particleIndex);

        void RemoveParticleFromSelection(uint32_t particleIndex);

        void ClearSelection();

        size_t GetSize();

    private:
        std::unordered_set<uint32_t> selectedParticles;

    };
}


#endif //FLUIDSOLVER_PARTICLESELECTION_HPP
