
#ifndef FLUIDSOLVER_STRIPEDPARTICLECOLLECTION_HPP
#define FLUIDSOLVER_STRIPEDPARTICLECOLLECTION_HPP

#include <vector>
#include <functional>
#include "IParticleCollection.hpp"

namespace FluidSolver {

    /**
     * The striped particle collection manages its data as separate arrays and is therefore called striped.
     * Each particle attribute is organized in its own dynamic array.
     * For more information see FluidSolver::IParticleCollection
     */
    class StripedParticleCollection : public IParticleCollection {

    private:

        std::vector<uint64_t> sortKeys;
        std::vector<particleIndex_t> particleIDToParticleIndexMap;
        std::vector<particleID_t> particleIndexToParticleIDMap;

        std::vector<uint8_t> typeData;
        std::vector<glm::vec2> positionData;
        std::vector<glm::vec2> velocityData;
        std::vector<glm::vec2> accelerationData;
        std::vector<float> massData;
        std::vector<float> pressureData;
        std::vector<float> densityData;
        std::vector<glm::vec2> nonPressureAccelerationData;
        std::vector<glm::vec2> predictedVelocityData;
        std::vector<float> sourceTermData;
        std::vector<float> diagonalElementData;

        particleAmount_t size = 0;
    protected:
        uint64_t GetSortKey(particleIndex_t index) override;

    private:
        void PrecalculateSortKeys(const sortKeyFunction_t &sortKeyFunction) override;

    protected:
        void SwapElements(particleIndex_t i, particleIndex_t j) override;

        virtual void AddEntryToEachArray();

    public:


        particleIndex_t AddParticle(const FluidParticle &fluidParticle) override;

        particleIndex_t AddEmptyParticle() override;

        void Clear() override;

    public:

        uint8_t *TypeData();

        glm::vec2 *PositionData();

        glm::vec2 *VelocityData();

        glm::vec2 *AccelerationData();

        float *MassData();

        float *PressureData();

        float *DensityData();

    public:

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

        particleID_t GetParticleID(particleIndex_t index) override;

        particleIndex_t GetIndex(particleID_t particleID) override;


    };

}


#endif //FLUIDSOLVER_STRIPEDPARTICLECOLLECTION_HPP
