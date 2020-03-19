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
        std::vector<size_t> particleIDToParticleIndexMap;
        std::vector<size_t> particleIndexToParticleIDMap;

        std::vector<FluidParticle> particleData;

    public:
        size_t GetParticleID(size_t index) override;

        size_t GetIndex(size_t particleID) override;

        size_t AddParticle(const FluidParticle &fluidParticle) override;

        size_t AddEmptyParticle() override;

        float GetMass(uint32_t index) override;

        void SetMass(uint32_t index, float value) override;

        float GetPressure(uint32_t index) override;

        void SetPressure(uint32_t index, float value) override;

        glm::vec2 GetPosition(uint32_t index) override;

        void SetPosition(uint32_t index, glm::vec2 value) override;

        glm::vec2 GetVelocity(uint32_t index) override;

        void SetVelocity(uint32_t index, glm::vec2 value) override;

        glm::vec2 GetAcceleration(uint32_t index) override;

        void SetAcceleration(uint32_t index, glm::vec2 value) override;

        uint32_t GetSize() override;

        float GetDensity(uint32_t index) override;

        void SetDensity(uint32_t index, float value) override;

        ParticleType GetParticleType(uint32_t index) override;

        void SetParticleType(uint32_t index, ParticleType value) override;

        glm::vec2 GetNonPressureAcceleration(uint32_t index) override;

        void SetNonPressureAcceleration(uint32_t index, glm::vec2 value) override;

        glm::vec2 GetPredictedVelocity(uint32_t index) override;

        void SetPredictedVelocity(uint32_t index, glm::vec2 value) override;

        float GetSourceTerm(uint32_t index) override;

        void SetSourceTerm(uint32_t index, float value) override;

        float GetDiagonalElement(uint32_t index) override;

        void SetDiagonalElement(uint32_t index, float value) override;

        FluidParticle* GetData();

    protected:
        uint64_t GetSortKey(size_t index) override;

        void PrecalculateSortKeys(const sortKeyFunction_t &sortKeyFunction) override;

        void SwapElements(size_t i, size_t j) override;

    };
}

#endif //FLUIDSOLVER_COMPACTPARTICLECOLLECTION_HPP
