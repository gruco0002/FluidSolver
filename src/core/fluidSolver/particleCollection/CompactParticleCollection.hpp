#ifndef FLUIDSOLVER_COMPACTPARTICLECOLLECTION_HPP
#define FLUIDSOLVER_COMPACTPARTICLECOLLECTION_HPP

#include "IParticleCollection.hpp"
#include <vector>

namespace FluidSolver {

    /**
     * The compact particle collection stores the particle data as compact vector of a particle structure containing all
     * the data of one particle.
     * For more information see FluidSolver::IParticleCollection
     */
    class CompactParticleCollection : public IParticleCollection {

    private:

        std::vector<uint64_t> sortKeys;
        std::vector<particleIndex_t> particleIDToParticleIndexMap;
        std::vector<particleID_t> particleIndexToParticleIDMap;

        std::vector<FluidParticle> particleData;

    public:
        particleID_t GetParticleID(particleIndex_t index) override;

        particleIndex_t GetIndex(particleID_t particleID) override;

        particleIndex_t AddParticle(const FluidParticle &fluidParticle) override;

        particleIndex_t AddEmptyParticle() override;

        void Clear() override;

        float GetMass(particleIndex_t index) override;

        void SetMass(particleIndex_t index, float value) override;

        float GetPressure(particleIndex_t index) override;

        void SetPressure(particleIndex_t index, float value) override;

        glm::vec2 GetPosition(particleIndex_t index) override;

        void SetPosition(particleIndex_t index, glm::vec2 value) override;

        glm::vec2 GetVelocity(particleIndex_t index) override;

        void SetVelocity(particleIndex_t index, glm::vec2 value) override;

        glm::vec2 GetAcceleration(particleIndex_t index) override;

        void SetAcceleration(particleIndex_t index, glm::vec2 value) override;

        particleAmount_t GetSize() override;

        float GetDensity(particleIndex_t index) override;

        void SetDensity(particleIndex_t index, float value) override;

        ParticleType GetParticleType(particleIndex_t index) override;

        void SetParticleType(particleIndex_t index, ParticleType value) override;

        glm::vec2 GetNonPressureAcceleration(particleIndex_t index) override;

        void SetNonPressureAcceleration(particleIndex_t index, glm::vec2 value) override;

        glm::vec2 GetPredictedVelocity(particleIndex_t index) override;

        void SetPredictedVelocity(particleIndex_t index, glm::vec2 value) override;

        float GetSourceTerm(particleIndex_t index) override;

        void SetSourceTerm(particleIndex_t index, float value) override;

        float GetDiagonalElement(particleIndex_t index) override;

        void SetDiagonalElement(particleIndex_t index, float value) override;

        FluidParticle *GetData();

    protected:
        uint64_t GetSortKey(particleIndex_t index) override;

        void PrecalculateSortKeys(const sortKeyFunction_t &sortKeyFunction) override;

        void SwapElements(particleIndex_t i, particleIndex_t j) override;

    };
}

#endif //FLUIDSOLVER_COMPACTPARTICLECOLLECTION_HPP
